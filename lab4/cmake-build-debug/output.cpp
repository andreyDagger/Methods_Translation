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

struct parse_exception : exception {
    string message;
    parse_exception() : parse_exception("error") {}
    explicit parse_exception(int pos) : parse_exception("wrong token before position: " + to_string(pos)) {}
    explicit parse_exception(const string &message) : message(message) {}
    const char *what() const noexcept override {
        return message.c_str();
    }
};

enum class Token {
    END, LEFT_BRACKET, MINUS, NUMBER, PLUS, REAL, RIGHT_BRACKET, SLASH, SPACE, STAR, 
};

struct lexical_analyzer {
    string s;
    string cur_string;
    char cur_char;
    int cur_pos;
    Token cur_token;
    lexical_analyzer() = default;
    lexical_analyzer(const string &s) : s(s) {
        cur_pos = -1;
        next_char();
    }
    bool is_blank(char c) const {
        return c == ' ' || c == '\r' || c == '\n' || c == '\t';
    }
    void next_char() {
        cur_pos++;
        if (cur_pos == s.size()) {
            cur_char = 0;
        }
        cur_char = s[cur_pos];
    }
    void next_token() {
        while (is_blank(cur_char)) {
            next_char();
        }
        if (cur_char == 0) {
            cur_token = Token::END;
            cur_string = "";
        }
        else if (isspace(cur_char)) {
            next_char();
            cur_token = Token::SPACE;
            cur_string = "";
        } else if ((int)s.size() - cur_pos >= 1 && s.substr(cur_pos, 1) == "/") {
            cur_string.clear();
            for (int i = 0; i < 1; ++i, next_char()) {
                cur_string += cur_char;
            }
            cur_token = Token::SLASH;
        } else if ((int)s.size() - cur_pos >= 1 && s.substr(cur_pos, 1) == "+") {
            cur_string.clear();
            for (int i = 0; i < 1; ++i, next_char()) {
                cur_string += cur_char;
            }
            cur_token = Token::PLUS;
        } else if ((int)s.size() - cur_pos >= 1 && s.substr(cur_pos, 1) == "(") {
            cur_string.clear();
            for (int i = 0; i < 1; ++i, next_char()) {
                cur_string += cur_char;
            }
            cur_token = Token::LEFT_BRACKET;
        } else if ((int)s.size() - cur_pos >= 1 && s.substr(cur_pos, 1) == ")") {
            cur_string.clear();
            for (int i = 0; i < 1; ++i, next_char()) {
                cur_string += cur_char;
            }
            cur_token = Token::RIGHT_BRACKET;
        } else if ((int)s.size() - cur_pos >= 1 && s.substr(cur_pos, 1) == "*") {
            cur_string.clear();
            for (int i = 0; i < 1; ++i, next_char()) {
                cur_string += cur_char;
            }
            cur_token = Token::STAR;
        } else if ((int)s.size() - cur_pos >= 1 && s.substr(cur_pos, 1) == "-") {
            cur_string.clear();
            for (int i = 0; i < 1; ++i, next_char()) {
                cur_string += cur_char;
            }
            cur_token = Token::MINUS;
        } else if (isdigit(cur_char)) {
            cur_string.clear();
            while (isdigit(cur_char)) {
                cur_string += cur_char;
                next_char();
            }
            if (cur_char == '.') {
                cur_string += '.'; next_char();
                while (isdigit(cur_char)) {
                    cur_string += cur_char; next_char();
                }
                cur_token = Token::REAL;
            }
            else { cur_token = Token::NUMBER; }
        } else  if (false) {}
else  next_char();
    }
};


struct poly_int;

union attributes {
    poly_int* x;
    pair<int, poly_int>* pii;
    string* s;
};

struct tree {
    string name;
    attributes data;
    vector<tree *> children;

    tree() = default;
    explicit tree(const string& name) : name(name) {}
    explicit tree(const attributes &data) : data(data) {}
    explicit tree(const string& name, const attributes &data) : name(name), data(data) {}
};

int nxt = 0;
map<tree*, string> reduced;

void generate_dot(const tree *root, ofstream &out) {
    if (!root) return;
    int id = nxt++;

    for (const auto &child : root->children) {
        out << root->name << "_" << to_string(id) << " -> " << child->name << "_" << to_string(nxt) << ";";
        generate_dot(child, out);
    }
}

void save_tree_to_dot(const tree *root, const string &filename) {
    ofstream out(filename);
    if (!out) {
        cerr << "Error creating DOT file." << endl;
        return;
    }

    out << "digraph G {";
    generate_dot(root, out);
    out << "}";

    out.close();
}

struct poly_int {
    string str;
    variant<int, double> x;

    operator string() const {
        if (holds_alternative<int>(x))
            return to_string(get<int>(x));
        else
            return to_string(get<double>(x));
    }

    poly_int(int x) : x(x), str(to_string(x)) {}
    poly_int(double x) : x(x), str(to_string(x)) {}
    poly_int(int x, string str) : x(x), str(str) {}
    poly_int(double x, string str) : x(x), str(str) {}

    friend poly_int operator+(const poly_int& a, const poly_int& b) {
        if (holds_alternative<int>(a.x) && holds_alternative<int>(b.x))
            return poly_int(get<int>(a.x) + get<int>(b.x), a.str + " + " + b.str);
        else if (holds_alternative<int>(a.x) && holds_alternative<double>(b.x))
            return poly_int(get<int>(a.x) + get<double>(b.x), a.str + " + " + b.str);
        else if (holds_alternative<double>(a.x) && holds_alternative<int>(b.x))
            return poly_int(get<double>(a.x) + get<int>(b.x), a.str + " + " + b.str);
        else
            return poly_int(get<double>(a.x) + get<double>(b.x), a.str + " + " + b.str);
    }
    friend poly_int operator*(const poly_int& a, const poly_int& b) {
        if (holds_alternative<int>(a.x) && holds_alternative<int>(b.x))
            return poly_int(get<int>(a.x) * get<int>(b.x), a.str + " * " + b.str);
        else if (holds_alternative<int>(a.x) && holds_alternative<double>(b.x))
            return poly_int(get<int>(a.x) * get<double>(b.x), a.str + "*" + b.str);
        else if (holds_alternative<double>(a.x) && holds_alternative<int>(b.x))
            return poly_int(get<double>(a.x) * get<int>(b.x), a.str + " * " + b.str);
        else
            return poly_int(get<double>(a.x) * get<double>(b.x), a.str + " * " + b.str);
    }
    friend poly_int operator/(const poly_int& a, const poly_int& b) {
        if (holds_alternative<int>(a.x) && holds_alternative<int>(b.x))
            return poly_int(get<int>(a.x) / get<int>(b.x), a.str + " / " + b.str);
        else if (holds_alternative<int>(a.x) && holds_alternative<double>(b.x))
            return poly_int(get<int>(a.x) / get<double>(b.x), a.str + " / " + b.str);
        else if (holds_alternative<double>(a.x) && holds_alternative<int>(b.x))
            return poly_int(get<double>(a.x) / get<int>(b.x), a.str + " / " + b.str);
        else
            return poly_int(get<double>(a.x) / get<double>(b.x), a.str + " / " + b.str);
    }
    friend poly_int operator-(const poly_int& a) {
        if (holds_alternative<int>(a.x))
            return poly_int(-get<int>(a.x), "-" + a.str);
        else
            return poly_int(-get<double>(a.x), "-" + a.str);
    }
    friend poly_int operator-(const poly_int& a, const poly_int& b) {
        return a + (-b);
    }
    friend ostream& operator<<(ostream& os, const poly_int& a) {
        if (holds_alternative<int>(a.x)) os << a.str << ": " << get<int>(a.x);
        else os << a.str << ": " << get<double>(a.x);
        return os;
    }
};

tree* yyparse();

string get_reduced(tree* v) {
    if (!reduced.count(v)) return "?";
    return reduced[v];
}

vector<string> get_steps(tree* v) {
    vector<string> res;
    vector<vector<string>> to_steps;
    for (tree* to : v->children) {
        to_steps.push_back(get_steps(to));
    }
    for (int i = 0; i < to_steps.size(); ++i) {
        for (int num = 0; num < to_steps[i].size(); ++num) {
            res.emplace_back();
            for (int j = 0; j < to_steps.size(); ++j) {
                if (j < i)
                    res.back() += to_steps[j].back();
                else if (j == i)
                    res.back() += to_steps[j][num];
                else
                    res.back() += to_steps[j][0];
            }
        }
    }
    res.emplace_back(get_reduced(v));
    return res;
}

int main() {
    freopen("input.txt", "r", stdin);
    tree* r = yyparse();
    save_tree_to_dot(r, "actual.dot");
    system("dot -Tpng actual.dot -o actual.png");
    vector<string> steps = get_steps(r);
    string prv = "";
    for (auto& s : steps) {
        if (s != prv)
            cout << s << "\n";
        prv = s;
    }
    return 0;
}


struct parser {
    lexical_analyzer lex;
    tree* parse(const string& s) {
        lex = lexical_analyzer(s);
        lex.next_token();
        return TOP(nullptr);
    }

    tree* EMPTY(tree* par) {
        tree* r = new tree("EMPTY");
        return r;
    }
    tree* END(tree* par) {
        tree* r = new tree("END");
        return r;
    }
    tree* LEFT_BRACKET(tree* par) {
        tree* r = new tree("LEFT_BRACKET");
        r->data.s = new string(lex.cur_string); reduced[r] = lex.cur_string;
        return r;
    }
    tree* MINUS(tree* par) {
        tree* r = new tree("MINUS");
        r->data.s = new string(lex.cur_string); reduced[r] = lex.cur_string;
        return r;
    }
    tree* NUMBER(tree* par) {
        tree* r = new tree("NUMBER");
        r->data.x = new poly_int(std::stoi(lex.cur_string)); reduced[r] = lex.cur_string;
        return r;
    }
    tree* PLUS(tree* par) {
        tree* r = new tree("PLUS");
        r->data.s = new string(lex.cur_string); reduced[r] = lex.cur_string;
        return r;
    }
    tree* REAL(tree* par) {
        tree* r = new tree("REAL");
        r->data.x = new poly_int(std::stod(lex.cur_string)); reduced[r] = lex.cur_string;
        return r;
    }
    tree* RIGHT_BRACKET(tree* par) {
        tree* r = new tree("RIGHT_BRACKET");
        r->data.s = new string(lex.cur_string); reduced[r] = lex.cur_string;
        return r;
    }
    tree* SLASH(tree* par) {
        tree* r = new tree("SLASH");
        r->data.s = new string(lex.cur_string); reduced[r] = lex.cur_string;
        return r;
    }
    tree* SPACE(tree* par) {
        tree* r = new tree("SPACE");
        return r;
    }
    tree* STAR(tree* par) {
        tree* r = new tree("STAR");
        r->data.s = new string(lex.cur_string); reduced[r] = lex.cur_string;
        return r;
    }
    tree* E(tree* par) {
        tree* r = new tree("E");
        switch (lex.cur_token) {
        case Token::LEFT_BRACKET:
            r->children.push_back(T(r));
            r->children.push_back(E1(r));
            r->data.x = new poly_int(*(r->children[0]->data.x) + *(r->children[1]->data.x));
        reduced[r] = *(r->data.x);
            break;
        case Token::MINUS:
            r->children.push_back(T(r));
            r->children.push_back(E1(r));
            r->data.x = new poly_int(*(r->children[0]->data.x) + *(r->children[1]->data.x));
        reduced[r] = *(r->data.x);
            break;
        case Token::NUMBER:
            r->children.push_back(T(r));
            r->children.push_back(E1(r));
            r->data.x = new poly_int(*(r->children[0]->data.x) + *(r->children[1]->data.x));
        reduced[r] = *(r->data.x);
            break;
        case Token::REAL:
            r->children.push_back(T(r));
            r->children.push_back(E1(r));
            r->data.x = new poly_int(*(r->children[0]->data.x) + *(r->children[1]->data.x));
        reduced[r] = *(r->data.x);
            break;
        default:
            throw parse_exception(lex.cur_pos);
        }
        return r;
    }
    tree* E1(tree* par) {
        tree* r = new tree("E1");
        switch (lex.cur_token) {
        case Token::MINUS:
            r->children.push_back(MINUS(r));
            lex.next_token();
            r->children.push_back(T(r));
            r->children.push_back(Q(r));
            r->data.x = new poly_int(-(*(r->children[1]->data.x)) + *(r->children[2]->data.x));
        reduced[r] = string(*(r->data.x));
            break;
        case Token::PLUS:
            r->children.push_back(PLUS(r));
            lex.next_token();
            r->children.push_back(E(r));
            r->data.x = new poly_int(*(r->children[1]->data.x));
        reduced[r] = "+" + string(*(r->data.x));
            break;
        case Token::END:
            r->data.x = new poly_int(0);
        reduced[r] = "";
            break;
        case Token::RIGHT_BRACKET:
            r->data.x = new poly_int(0);
        reduced[r] = "";
            break;
        default:
            throw parse_exception(lex.cur_pos);
        }
        return r;
    }
    tree* F(tree* par) {
        tree* r = new tree("F");
        switch (lex.cur_token) {
        case Token::LEFT_BRACKET:
            r->children.push_back(F1(r));
            r->data.x = new poly_int(*(r->children[0]->data.x));
        reduced[r] = *(r->data.x);
            break;
        case Token::MINUS:
            r->children.push_back(MINUS(r));
            lex.next_token();
            r->children.push_back(F1(r));
            r->data.x = new poly_int(-(*(r->children[1]->data.x)));
        reduced[r] = *(r->data.x);
            break;
        case Token::NUMBER:
            r->children.push_back(F1(r));
            r->data.x = new poly_int(*(r->children[0]->data.x));
        reduced[r] = *(r->data.x);
            break;
        case Token::REAL:
            r->children.push_back(F1(r));
            r->data.x = new poly_int(*(r->children[0]->data.x));
        reduced[r] = *(r->data.x);
            break;
        default:
            throw parse_exception(lex.cur_pos);
        }
        return r;
    }
    tree* F1(tree* par) {
        tree* r = new tree("F1");
        switch (lex.cur_token) {
        case Token::LEFT_BRACKET:
            r->children.push_back(LEFT_BRACKET(r));
            lex.next_token();
            r->children.push_back(E(r));
            r->children.push_back(RIGHT_BRACKET(r));
            lex.next_token();
            r->data.x = new poly_int(*(r->children[1]->data.x));
        reduced[r] = *(r->data.x);
            break;
        case Token::NUMBER:
            r->children.push_back(NUMBER(r));
            lex.next_token();
            r->data.x = new poly_int(*(r->children[0]->data.x));
        reduced[r] = *(r->data.x);
            break;
        case Token::REAL:
            r->children.push_back(REAL(r));
            lex.next_token();
            r->data.x = new poly_int(*(r->children[0]->data.x));
        reduced[r] = *(r->data.x);
            break;
        default:
            throw parse_exception(lex.cur_pos);
        }
        return r;
    }
    tree* Q(tree* par) {
        tree* r = new tree("Q");
        switch (lex.cur_token) {
        case Token::MINUS:
            r->children.push_back(E1(r));
            r->data.x = new poly_int(*(r->children[0]->data.x));
        reduced[r] = *(r->data.x);
            break;
        case Token::PLUS:
            r->children.push_back(E1(r));
            r->data.x = new poly_int(*(r->children[0]->data.x));
        reduced[r] = *(r->data.x);
            break;
        case Token::END:
            r->data.x = new poly_int(0);
        reduced[r] = "";
            break;
        case Token::RIGHT_BRACKET:
            r->data.x = new poly_int(0);
        reduced[r] = "";
            break;
        default:
            throw parse_exception(lex.cur_pos);
        }
        return r;
    }
    tree* T(tree* par) {
        tree* r = new tree("T");
        switch (lex.cur_token) {
        case Token::LEFT_BRACKET:
            r->children.push_back(F(r));
            r->children.push_back(T1(r));
            if ((r->children[1]->data.pii)->first == 1) r->data.x = new poly_int(*(r->children[0]->data.x) / (r->children[1]->data.pii)->second);
        else r->data.x = new poly_int(*(r->children[0]->data.x) * (r->children[1]->data.pii)->second);
        reduced[r] = *(r->data.x);
            break;
        case Token::MINUS:
            r->children.push_back(F(r));
            r->children.push_back(T1(r));
            if ((r->children[1]->data.pii)->first == 1) r->data.x = new poly_int(*(r->children[0]->data.x) / (r->children[1]->data.pii)->second);
        else r->data.x = new poly_int(*(r->children[0]->data.x) * (r->children[1]->data.pii)->second);
        reduced[r] = *(r->data.x);
            break;
        case Token::NUMBER:
            r->children.push_back(F(r));
            r->children.push_back(T1(r));
            if ((r->children[1]->data.pii)->first == 1) r->data.x = new poly_int(*(r->children[0]->data.x) / (r->children[1]->data.pii)->second);
        else r->data.x = new poly_int(*(r->children[0]->data.x) * (r->children[1]->data.pii)->second);
        reduced[r] = *(r->data.x);
            break;
        case Token::REAL:
            r->children.push_back(F(r));
            r->children.push_back(T1(r));
            if ((r->children[1]->data.pii)->first == 1) r->data.x = new poly_int(*(r->children[0]->data.x) / (r->children[1]->data.pii)->second);
        else r->data.x = new poly_int(*(r->children[0]->data.x) * (r->children[1]->data.pii)->second);
        reduced[r] = *(r->data.x);
            break;
        default:
            throw parse_exception(lex.cur_pos);
        }
        return r;
    }
    tree* T1(tree* par) {
        tree* r = new tree("T1");
        switch (lex.cur_token) {
        case Token::SLASH:
            r->children.push_back(SLASH(r));
            lex.next_token();
            r->children.push_back(F(r));
            r->children.push_back(T1(r));
            if ((r->children[2]->data.pii)->first == 1)
            r->data.pii = new pair<int, poly_int>(1, *(r->children[1]->data.x) / (r->children[2]->data.pii)->second);
        else
            r->data.pii = new pair<int, poly_int>(1, *(r->children[1]->data.x) * (r->children[2]->data.pii)->second);
        reduced[r] = "/" + string((r->data.pii)->second);
            break;
        case Token::STAR:
            r->children.push_back(STAR(r));
            lex.next_token();
            r->children.push_back(F(r));
            r->children.push_back(T1(r));
            if ((r->children[2]->data.pii)->first == 1)
            r->data.pii = new pair<int, poly_int>(0, *(r->children[1]->data.x) / (r->children[2]->data.pii)->second);
        else
            r->data.pii = new pair<int, poly_int>(0, *(r->children[1]->data.x) * (r->children[2]->data.pii)->second);
        reduced[r] = "*" + string((r->data.pii)->second);
            break;
        case Token::END:
            r->data.pii = new pair<int, poly_int>(0, 1);
        reduced[r] = "";
            break;
        case Token::MINUS:
            r->data.pii = new pair<int, poly_int>(0, 1);
        reduced[r] = "";
            break;
        case Token::PLUS:
            r->data.pii = new pair<int, poly_int>(0, 1);
        reduced[r] = "";
            break;
        case Token::RIGHT_BRACKET:
            r->data.pii = new pair<int, poly_int>(0, 1);
        reduced[r] = "";
            break;
        default:
            throw parse_exception(lex.cur_pos);
        }
        return r;
    }
    tree* TOP(tree* par) {
        tree* r = new tree("TOP");
        switch (lex.cur_token) {
        case Token::LEFT_BRACKET:
            r->children.push_back(E(r));
            r->data.x = new poly_int(*(r->children[0]->data.x));
        reduced[r] = *(r->data.x);
            break;
        case Token::MINUS:
            r->children.push_back(E(r));
            r->data.x = new poly_int(*(r->children[0]->data.x));
        reduced[r] = *(r->data.x);
            break;
        case Token::NUMBER:
            r->children.push_back(E(r));
            r->data.x = new poly_int(*(r->children[0]->data.x));
        reduced[r] = *(r->data.x);
            break;
        case Token::REAL:
            r->children.push_back(E(r));
            r->data.x = new poly_int(*(r->children[0]->data.x));
        reduced[r] = *(r->data.x);
            break;
        default:
            throw parse_exception(lex.cur_pos);
        }
        return r;
    }
};

tree* yyparse() {
    parser p;
    tree* r;
    string content, tmp;
    while (getline(cin, tmp)) { content += tmp; content += '\n'; }
    try {
        r = p.parse(content);
    } catch (const parse_exception& e) {
        cerr << e.what() << '\n';
    }
    return r;
}
