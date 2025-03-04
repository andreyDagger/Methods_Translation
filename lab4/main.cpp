#include <variant>
#include <vector>
#include <string>
#include <cstring>
#include <algorithm>
#include <cassert>
#include <map>
#include <set>
#include <memory>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

struct file_open : exception {
    string message;
    explicit file_open(const string& message) : message("Couldn't open file: " + message) {}
    const char* what() const noexcept {
        return message.c_str();
    }
};

struct bad_lexer : exception {
    string message;
    explicit bad_lexer(int line) : bad_lexer("Parse error in line " + to_string(line)) {}
    explicit bad_lexer(const string& message) : message("Invalid lexer file: " + message) {}
    const char* what() const noexcept {
        return message.c_str();
    }
};

struct bad_grammar : exception {
    string message;
    explicit bad_grammar(const string& message) : message("Invalid grammar file: " + message) {}
    const char* what() const noexcept {
        return message.c_str();
    }
};

string join_strings(const vector<string>& a, const string& delim) {
    assert(!a.empty());
    string res = a[0];
    for (int i = 1; i < a.size(); ++i)
        res += delim + a[i];
    return res;
}

string tabs(int n) {
    return string(n * 4, ' ');
}

int find_first_since(const string &s, int i, const string &what) {
    while (i + what.size() <= s.size()) {
        if (equal(s.begin() + i, s.begin() + i + what.size(), what.begin()))
            return i;
        ++i;
    }
    return s.size();
}

string read_file(ifstream &in) {
    string s, tmp;
    while (getline(in, tmp)) {
        s += tmp;
        s += "\n";
    }
    return s;
}

string ltrim(std::string s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
                return !std::isspace(ch);
            }));
    return s;
}

string rtrim(std::string s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
                return !std::isspace(ch);
            }).base(),
            s.end());
    return s;
}

string trim(std::string s) {
    return ltrim(rtrim(s));
}

ofstream out;

void write_includes() {
    out << "#include <variant>\n"
           "#include <vector>\n"
           "#include <string>\n"
           "#include <cstring>\n"
           "#include <algorithm>\n"
           "#include <cassert>\n"
           "#include <map>\n"
           "#include <set>\n"
           "#include <memory>\n"
           "#include <iostream>\n"
           "#include <sstream>\n"
           "#include <fstream>\n";
    out << "using namespace std;\n\n";
}

void write_exceptions() {
    out << "struct parse_exception : exception {\n"
           "    string message;\n"
           "    parse_exception() : parse_exception(\"error\") {}\n"
           "    explicit parse_exception(int pos) : parse_exception(\"wrong token before position: \" + to_string(pos)) {}\n"
           "    explicit parse_exception(const string &message) : message(message) {}\n"
           "    const char *what() const noexcept override {\n"
           "        return message.c_str();\n"
           "    }\n"
           "};\n\n";
}

void write_union(const string &s) {
    int i = s.find("%union");
    if (i == string::npos) {
        throw bad_grammar("Couldn't find %union declaration");
    }
    i += 6;
    int j = find_first_since(s, i, "};");
    if (j >= s.size()) {
        throw bad_grammar("Invalid %union declaration, usage:\n%union {\n\tfields\n};");
    }
    ++j;
    out << "union attributes";
    out << s.substr(i, j - i + 1) << "\n\n";
}

enum class Special_tokens {
    SPACE,
    NUMBER,
    REAL,
    VARIABLE,
    QUOTED_STRING,
    ANY,
};

struct s_token {
    variant<string, Special_tokens> what_matches;
    string name;
    string code;
};

vector<s_token> get_tokens(ifstream &in) {
    string s;
    vector<s_token> res;
    for (int line = 1; getline(in, s); ++line) {
        s = trim(s);
        if (s.empty()) continue;
        res.emplace_back();
        int lb = s.find('{');
        int rb = s.rfind('}');
        if (lb == string::npos || rb == string::npos) {
            throw bad_lexer(line);
        }
        res.back().code = trim(s.substr(lb + 1, rb - lb - 1));
        auto what_matches = trim(s.substr(0, lb));
        if (what_matches == "[ \\t\\n]+")
            res.back().what_matches = Special_tokens::SPACE;
        else if (what_matches == "[0-9]+")
            res.back().what_matches = Special_tokens::NUMBER;
        else if (what_matches == "[0-9]+.[0-9]+")
            res.back().what_matches = Special_tokens::REAL;
        else if (what_matches == "[a-zA-Z]+")
            res.back().what_matches = Special_tokens::VARIABLE;
        else if (what_matches == "QUOTED_STRING")
            res.back().what_matches = Special_tokens::QUOTED_STRING;
        else if (what_matches == ".")
            res.back().what_matches = Special_tokens::ANY;
        else {
            assert(what_matches[0] == '"' && what_matches.back() == '"');
            res.back().what_matches = what_matches.substr(1, what_matches.size() - 2);
        }
        auto &code = res.back().code;
        int i = code.rfind("return ");
        if (i == string::npos) {
            continue;
        }
        int j = code.rfind(';');
        int k = i + 6;
        while (isspace(code[k])) ++k;
        res.back().name = code.substr(k, j - k);
        code.erase(code.begin() + i, code.end());
    }
    return res;
}

void write_lexer_fields() {
    out << tabs(1) << "string s;\n";
    out << tabs(1) << "string cur_string;\n";
    out << tabs(1) << "char cur_char;\n";
    out << tabs(1) << "int cur_pos;\n";
    out << tabs(1) << "Token cur_token;\n";
}

void write_lexer_constructors() {
    out << tabs(1) << "lexical_analyzer() = default;\n";
    out << tabs(1) << "lexical_analyzer(const string &s) : s(s) {\n"
                      "        cur_pos = -1;\n"
                      "        next_char();\n"
                      "    }\n";
}

void write_lexer_functions(const vector<s_token> &tokens) {
    out << tabs(1) << "bool is_blank(char c) const {\n"
                      "        return c == ' ' || c == '\\r' || c == '\\n' || c == '\\t';\n"
                      "    }\n";
    out << tabs(1) << "void next_char() {\n"
                      "        cur_pos++;\n"
                      "        if (cur_pos == s.size()) {\n"
                      "            cur_char = 0;\n"
                      "        }\n"
                      "        cur_char = s[cur_pos];\n"
                      "    }\n";

    out << tabs(1) << "void next_token() {\n";
    out << tabs(2) << "while (is_blank(cur_char)) {\n"
                      "            next_char();\n"
                      "        }\n";
    out << tabs(2) << "if (cur_char == 0) {\n"
                      "            cur_token = Token::END;\n"
                      "            cur_string = \"\";\n"
                      "        }\n";
    for (int i = 0; i < tokens.size(); ++i) {
        auto &[what_matches, name, code] = tokens[i];
        out << tabs(2 * (i == 0)) << "else ";
        if (holds_alternative<string>(what_matches)) {
            auto str = get<string>(what_matches);
            out << "if ((int)s.size() - cur_pos >= " + to_string(str.size()) + " && s.substr(cur_pos, " + to_string(str.size()) + ") == \"" + str + "\") {\n";
            out << tabs(3) << "cur_string.clear();\n";
            out << tabs(3) << "for (int i = 0; i < " + to_string(str.size()) + "; ++i, next_char()) {\n"
                << tabs(4) << "cur_string += cur_char;\n"
                << tabs(3) << "}\n";
            out << tabs(3) << "cur_token = Token::" + name + ";\n"
                << tabs(2) << "} ";
        } else if (holds_alternative<Special_tokens>(what_matches)) {
            auto tok = get<Special_tokens>(what_matches);
            if (tok == Special_tokens::SPACE) {
                out << "if (isspace(cur_char)) {\n"
                    << tabs(3) << "next_char();\n"
                    << tabs(3) << "cur_token = Token::" + name + ";\n"
                    << tabs(3) << "cur_string = \"\";\n"
                    << tabs(2) << "} ";
            } else if (tok == Special_tokens::NUMBER) {
                out << "if (isdigit(cur_char)) {\n";
                out << tabs(3) << "cur_string.clear();\n";
                out << tabs(3) << "while (isdigit(cur_char)) {\n";
                out << tabs(4) << "cur_string += cur_char;\n";
                out << tabs(4) << "next_char();\n";
                out << tabs(3) << "}\n";
                out << tabs(3) << "if (cur_char == '.') {\n";
                out << tabs(4) << "cur_string += '.'; next_char();\n";
                out << tabs(4) << "while (isdigit(cur_char)) {\n";
                out << tabs(5) << "cur_string += cur_char; next_char();\n";
                out << tabs(4) << "}\n";
                out << tabs(4) << "cur_token = Token::REAL;\n";
                out << tabs(3) << "}\n";
                out << tabs(3) << "else { cur_token = Token::NUMBER; }\n";
                out << tabs(2) << "} ";
            } else if (tok == Special_tokens::VARIABLE) {
                out << "if (isalpha(cur_char)) {\n";
                out << tabs(3) << "cur_string.clear();\n";
                out << tabs(3) << "while (isdigit(cur_char) || isalpha(cur_char)) {\n";
                out << tabs(4) << "cur_string += cur_char;\n";
                out << tabs(4) << "next_char();\n";
                out << tabs(3) << "}\n";
                out << tabs(3) << "cur_token = Token::VARIABLE;\n";
                out << tabs(2) << "} ";
            } else if (tok == Special_tokens::QUOTED_STRING) {
                out << "if (cur_char == '\"') {\n";
                out << tabs(3) << "cur_string.clear();\n";
                out << tabs(3) << "while (cur_char != '\"') {\n";
                out << tabs(4) << "cur_string += cur_char;";
                out << tabs(4) << "next_char();\n";
                out << tabs(3) << "}\n";
                out << tabs(3) << "cur_string += '\"';\n";
                out << tabs(3) << "cur_token = Token::QUOTED_STRING;\n";
                out << tabs(2) << "} ";
            } else if (tok == Special_tokens::ANY) {
                out << " next_char();\n";
            } else if (tok == Special_tokens::REAL) {
                out << " if (false) {}\n";
            }
        }
    }
    out << tabs(1) << "}\n";
}

map<string, string> generate_lexer(const char *lexer_fname) {
    ifstream lexer_file(lexer_fname, ios_base::in);
    if (lexer_file.fail()) {
        throw file_open(lexer_fname);
    }
    auto tokens = get_tokens(lexer_file);
    out << "enum class Token {\n";
    out << tabs(1) << "END, ";
    map<string, string> names;
    for (auto &[_1, name, code]: tokens) {
        if (!name.empty())
            names[name] = code;
    }
    for (auto &[name, code]: names) {
        out << name << ", ";
    }
    out << "\n};\n\n";

    out << "struct lexical_analyzer {\n";
    write_lexer_fields();
    write_lexer_constructors();
    write_lexer_functions(tokens);
    out << "};\n\n";
    names["END"] = {};
    names["EMPTY"] = {};
    return names;
}

void write_tree() {
    out << "struct tree {\n"
           "    string name;\n"
           "    attributes data;\n"
           "    vector<tree *> children;\n\n"
           "    tree() = default;\n"
           "    explicit tree(const string& name) : name(name) {}\n"
           "    explicit tree(const attributes &data) : data(data) {}\n"
           "    explicit tree(const string& name, const attributes &data) : name(name), data(data) {}\n"
           "};";
}

void write_code_defs(const string &s) {
    int l = s.find("{%"), r = s.find("%}");
    if (l == string::npos || r == string::npos || l >= r) {
        throw bad_grammar("Couldn't find user code definitions");
    }
    l += 2;
    out << s.substr(l, r - l) << "\n";
}

void write_code_impls(const string &s) {
    int off = s.find("%}") + 2;
    int l = find_first_since(s, off, "{%");
    int r = find_first_since(s, off, "%}");
    if (l == string::npos || r == string::npos || l >= r) {
        throw bad_grammar("Couldn't find user code implementation");
    }
    l += 2;
    out << s.substr(l, r - l) << "\n";
}

map<string, string> get_which_var(const string &s) {
    map<string, string> res;
    for (int i = 0; i < s.size();) {
        int j = find_first_since(s, i, "%token");
        int k = find_first_since(s, i, "%type");
        if (j == s.size() && k == s.size())
            break;
        int w;
        if (j < k) w = j + 6;
        else
            w = k + 5;
        int l = find(s.begin() + w, s.end(), '<') - s.begin();
        int r = find(s.begin() + w, s.end(), '>') - s.begin();
        string var = s.substr(l + 1, r - l - 1);
        w = r + 1;
        string tokens;
        while (w < s.size() && s[w] != '\n') {
            tokens += s[w];
            ++w;
        }
        stringstream ss(tokens);
        string tmp;
        while (ss >> tmp) {
            res[tmp] = var;
        }
        i = w;
    }
    return res;
}

map<string, vector<pair<vector<string>, pair<string, string>>>> get_rules(const string &s, string &first) {
    map<string, vector<pair<vector<string>, pair<string, string>>>> res;
    int l = s.find("%%");
    int r = s.rfind("%%");
    if (l == string::npos || r == string::npos || l + 1 >= r) {
        throw bad_grammar("Grammar description must be quoted in %%");
    }
    l += 2;
    for (int i = l; i < r; ++i) {
        if (s[i] == ':') {
            string nonterminal;
            for (int j = i - 1; j >= 0 && !isspace(s[j]); --j) {
                nonterminal = s[j] + nonterminal;
            }
            if (first.empty()) {
                first = nonterminal;
            }
            ++i;
            while (true) {
                string tos, code_before, code_after;
                int code_before_start = find(s.begin() + i, s.end(), '{') - s.begin();
                int code_before_end = find(s.begin() + i, s.end(), '}') - s.begin();
                int sc = find(s.begin() + i, s.end(), ';') - s.begin();
                if (sc < code_before_start)
                    break;
                int code_after_start = find(s.begin() + code_before_end + 1, s.end(), '{') - s.begin();
                int code_after_end = find(s.begin() + code_before_end + 1, s.end(), '}') - s.begin();
                tos = s.substr(i, code_before_start - i);
                code_before = s.substr(code_before_start + 1, code_before_end - code_before_start - 1);
                code_after = s.substr(code_after_start + 1, code_after_end - code_after_start - 1);
                res[nonterminal].emplace_back();
                stringstream ss(tos);
                string tmp;
                while (getline(ss, tmp, ' ')) {
                    tmp = trim(tmp);
                    if (tmp.empty()) continue;
                    res[nonterminal].back().first.push_back(tmp);
                }
                res[nonterminal].back().second = {code_before, code_after};
                i = code_after_end + 1;
                while (i < r) {
                    if (s[i] == '|' || s[i] == ';')
                        break;
                    ++i;
                }
                if (i == r || s[i] == ';')
                    break;
                if (s[i] == '|')
                    ++i;
            }
        }
    }
    return res;
}

set<string> get_can_be_empty(const vector<pair<string, vector<string>>> &rules,
                             const map<string, string> &terminals) {
    set<string> res;
    while (true) {
        bool change = false;
        for (auto &[left, tos]: rules) {
            if (res.count(left))
                continue;
            bool f = true;
            for (auto &x: tos) {
                f &= (x == "EMPTY" || res.count(x));
            }
            if (f) {
                change = true;
                res.insert(left);
            }
        }
        if (!change)
            break;
    }
    return res;
}

map<string, map<string, vector<string>>> get_FIRST(const vector<pair<string, vector<string>>> &rules,
                                                   const map<string, string> &terminals,
                                                   const set<string> &can_be_empty) {
    map<string, map<string, vector<string>>> res;
    for (auto &[t, _]: terminals) {
        res[t][t] = {t};
    }
    while (true) {
        bool change = false;
        for (auto &[left, tos]: rules) {
            int was_size = res[left].size();
            for (int j = 0; j < tos.size(); ++j) {
                if (terminals.count(tos[j]))
                    res[left][tos[j]] = tos;
                else {
                    for (auto &[first, _]: res[tos[j]]) {
                        res[left][first] = tos;
                    }
                }
                if (tos[j] != "EMPTY" && !can_be_empty.count(tos[j]))
                    break;
            }
            if (res[left].size() != was_size) {
                change = true;
            }
        }
        if (!change)
            break;
    }
    return res;
}

map<string, set<string>> get_FOLLOW(const string &start, const vector<pair<string, vector<string>>> &rules,
                                    const map<string, map<string, vector<string>>> &FIRST) {
    map<string, set<string>> res;
    res[start].insert("END");
    while (true) {
        bool change = false;
        for (auto &[left, tos]: rules) {
            for (int i = 0; i < tos.size(); ++i) {
                bool suffix_can_be_empty = true;
                for (int j = i + 1; j < tos.size(); ++j) {
                    if (!FIRST.at(tos[j]).count("EMPTY"))
                        suffix_can_be_empty = false;
                }
                int was_size = res[tos[i]].size();
                if (i + 1 < tos.size()) {
                    for (auto &[f, _]: FIRST.at(tos[i + 1])) {
                        if (f != "EMPTY") {
                            res[tos[i]].insert(f);
                        }
                    }
                }
                if (suffix_can_be_empty) {
                    res[tos[i]].insert(res[left].begin(), res[left].end());
                }
                if (was_size != res[tos[i]].size()) {
                    change = true;
                }
            }
        }
        if (!change)
            break;
    }
    return res;
}

string replace_placeholders(string code, const map<string, string> &which_var,
                            const string &left, const vector<string> &tos,
                            const string &common_var) {
    while (true) {
        int i = code.find('$');
        if (i == string::npos)
            break;
        if (code[i + 1] == '#') {
            code.replace(code.begin() + i, code.begin() + i + 2, "par->data." + common_var);
        } else if (code[i + 1] == '$') {
            if (!which_var.count(left)) {
                throw bad_grammar(left + " has no %token declaration, but used as an attribute");
            }
            code.replace(code.begin() + i, code.begin() + i + 2, "r->data." + which_var.at(left));
        } else if (code[i + 1] == '@') {
            code.replace(code.begin() + i, code.begin() + i + 2, "r");
        } else {
            int j = i + 1;
            while (j < code.size() && isdigit(code[j]))
                ++j;
            if (j == i + 1)
                throw bad_grammar("In rule " + left + " -> " + join_strings(tos, " ") + ", invalid usage of attributes accessing.\nUsage: $$ | $# | ${num}");
            int num = stoi(code.substr(i + 1, j - i - 1));
            if (num - 1 < 0 || num - 1 >= (int)tos.size())
                throw bad_grammar("In rule " + left + " -> " + join_strings(tos, " ") + ", " + to_string(num) + " must be in range [1;" + to_string(tos.size()) + "]");
            if (!which_var.count(tos[num - 1]))
                throw bad_grammar(tos[num - 1] + " has no %token declaration, but used as an attribute");
            code.replace(code.begin() + i, code.begin() + j, "r->children[" + to_string(num - 1) + "]->data." + which_var.at(tos[num - 1]));
        }
    }
    return code;
}

string replace_placeholders_in_terminal(string code, const map<string, string> &which_var, const string &name) {
    while (true) {
        int i = code.find("yylval.s");
        int j = code.find("yytext");
        int k = code.find("$@");
        if (i == string::npos && j == string::npos && k == string::npos)
            break;
        if (i != string::npos) {
            code.replace(code.begin() + i, code.begin() + i + 8, "r->data." + which_var.at(name));
        } else if (j != string::npos) {
            code.replace(code.begin() + j, code.begin() + j + 6, "lex.cur_string");
        } else if (k != string::npos) {
            code.replace(code.begin() + k, code.begin() + k + 2, "r");
        }
    }
    return code;
}

string beautify_code(int n, string s) {
    s = trim(s);
    if (s.empty()) return "";
    return tabs(n) + s + "\n";
}

void generate_parser(const map<string, string> &terminals, const char *parser_fname) {
    ifstream parser_file(parser_fname);
    string s = read_file(parser_file);
    write_code_defs(s);
    write_union(s);
    write_tree();
    write_code_impls(s);
    map<string, string> which_var = get_which_var(s);
    string common_var = which_var.empty() ? "" : which_var.begin()->second;
    string first;
    map<string, vector<pair<vector<string>, pair<string, string>>>> rules = get_rules(s, first);
    vector<pair<string, vector<string>>> rules_list;
    for (auto &[left, outs]: rules) {
        for (auto &[tos, code]: outs) {
            rules_list.emplace_back(left, tos);
        }
    }
    out << "struct parser {\n";
    out << tabs(1) << "lexical_analyzer lex;\n";
    out << tabs(1) << "tree* parse(const string& s) {\n";
    out << tabs(2) << "lex = lexical_analyzer(s);\n";
    out << tabs(2) << "lex.next_token();\n";
    out << tabs(2) << "return " + first + "(nullptr);\n";
    out << tabs(1) << "}\n\n";
    map<pair<string, vector<string>>, pair<string, string>> rule_to_code;
    for (auto &[left, outs]: rules) {
        for (auto &[tos, code]: outs) {
            rule_to_code[{left, tos}] = code;
        }
    }
    set<string> can_be_empty = get_can_be_empty(rules_list, terminals);
    map<string, map<string, vector<string>>> FIRST = get_FIRST(rules_list, terminals, can_be_empty);
    map<string, set<string>> FOLLOW = get_FOLLOW(first, rules_list, FIRST);

    for (auto& [term, code] : terminals) {
        out << tabs(1) << "tree* " + term + "(tree* par) {\n";
        out << tabs(2) << "tree* r = new tree(\"" + term + "\");\n";
        out << beautify_code(2, replace_placeholders_in_terminal(terminals.at(term), which_var, term));
        out << tabs(2) << "return r;\n";
        out << tabs(1) << "}\n";
    }

    for (auto &[left, outs]: rules) {
        out << tabs(1) << "tree* " + left + "(tree* par) {\n";
        out << tabs(2) << "tree* r = new tree(\"" + left + "\");\n";
        out << tabs(2) << "switch (lex.cur_token) {\n";
        bool can_be_empty = false;
        for (auto &[x, y]: outs) {
            if (x == vector<string>{"EMPTY"})
                can_be_empty = true;
        }
        for (auto &[f, to]: FIRST[left]) {
            if (f == "EMPTY") {
                continue;
            }
            out << tabs(2) << "case Token::" + f << ":\n";
            out << beautify_code(3,replace_placeholders(rule_to_code[{left, to}].first, which_var, left, to, common_var));
            for (auto &term: to) {
                if (terminals.count(term)) {
                    out << tabs(3) << "r->children.push_back(" + term + "(r));\n";
//                    out << tabs(3) << "r->children.push_back(new tree(\"" + term + "\", term_data));\n";
                    out << tabs(3) << "lex.next_token();\n";
                } else {
                    out << tabs(3) << "r->children.push_back(" + term + "(r));\n";
                }
            }
            out << beautify_code(3, replace_placeholders(rule_to_code[{left, to}].second, which_var, left, to, common_var));
            out << tabs(3) << "break;\n";
        }
        if (can_be_empty) {
            for (auto &f: FOLLOW[left]) {
                out << tabs(2) << "case Token::" + f << ":\n";
                out << beautify_code(3, replace_placeholders(rule_to_code[{left, {"EMPTY"}}].first, which_var, left, {"EMPTY"}, common_var));
                out << beautify_code(3, replace_placeholders(rule_to_code[{left, {"EMPTY"}}].second, which_var, left, {"EMPTY"}, common_var));
                out << tabs(3) << "break;\n";
            }
        }
        out << tabs(2) << "default:\n";
        out << tabs(3) << "throw parse_exception(lex.cur_pos);\n" ;
        out << tabs(2) << "}\n";
        out << tabs(2) << "return r;\n";
        out << tabs(1) << "}\n";
    }
    out << "};\n\n";
}

void write_yyparse() {
    out << "tree* yyparse() {\n";
    out << tabs(1) << "parser p;\n";
    out << tabs(1) << "tree* r;\n";
    out << tabs(1) << "string content, tmp;\n";
    out << tabs(1) << "while (getline(cin, tmp)) { content += tmp; content += '\\n'; }\n";
    out << tabs(1) << "try {\n";
    out << tabs(2) << "r = p.parse(content);\n";
    out << tabs(1) << "} catch (const parse_exception& e) {\n";
    out << tabs(2) << "cerr << e.what() << '\\n';\n";
    out << tabs(1) << "}\n";
    out << tabs(1) << "return r;\n";
    out << "}\n";
}

int32_t main(int argc, char **argv) {
    if (argc != 4) {
        cerr << "Usage: [LEXER.lex] [GRAMMAR.y] [OUTPUT_FILE.cpp]";
        return 1;
    }
    try {
        out.open(argv[3]);
        if (out.fail()) {
            throw file_open(argv[3]);
        }
        write_includes();
        write_exceptions();
        auto terminals = generate_lexer(argv[1]);
        generate_parser(terminals, argv[2]);
        write_yyparse();
    } catch (const exception& e) {
        cerr << e.what() << "\n";
        return 1;
    }

    return 0;
}