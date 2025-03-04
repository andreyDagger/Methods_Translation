#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <set>
#include <variant>
#include <cassert>

using namespace std;

set<string> possible_types{
        "int",
        "long",
        "char",
        "string",
        "bool",
};

struct parse_exception : exception {
    string message;

    explicit parse_exception(const string &message) : message(message) {}
};

enum class Token {
    NAME, SEMICOLON, COMMA, END, STAR, NUMBER, LEFT_BRACKET, RIGHT_BRACKET
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
        } else if (cur_char == '[') {
            next_char();
            cur_token = Token::LEFT_BRACKET;
            cur_string = "[";
        } else if (cur_char == ']') {
            next_char();
            cur_token = Token::RIGHT_BRACKET;
            cur_string = "]";
        } else if (cur_char == '*') {
            next_char();
            cur_token = Token::STAR;
            cur_string = "*";
        } else if (cur_char == ';') {
            next_char();
            cur_token = Token::SEMICOLON;
            cur_string = ";";
        } else if (cur_char == ',') {
            next_char();
            cur_token = Token::COMMA;
            cur_string = ",";
        } else if (isdigit(cur_char)) {
            string num;
            while (isdigit(cur_char)) {
                num += cur_char;
                next_char();
            }
            cur_token = Token::NUMBER;
            cur_string = num;
        } else {
            if (!isalpha(cur_char)) {
                throw parse_exception(
                        "Error at pos " + to_string(cur_pos) + ": Variable name must start with english letter");
            }
            string name;
            while (!is_blank(cur_char) && cur_char != ';' && cur_char != ',' && cur_char != '[' && cur_char != ']' &&
                   cur_char != 0) {
                name += cur_char;
                next_char();
            }
            cur_token = Token::NAME;
            cur_string = name;
        }
    }
};

struct tree {
    string node;
    vector<tree *> children;

    tree(const string &node, const vector<tree *> &children) : node(node), children(children) {}

    explicit tree(const string &node) : node(node) {}
};

struct parser {
    lexical_analyzer lex;

    tree *T() {
        tree *r = new tree("T");
        switch (lex.cur_token) {
            case Token::NAME:
                r->children.push_back(A());
                r->children.push_back(T());
                break;
            case Token::END:
                r->children.push_back(new tree("END"));
                break;
            default:
                throw parse_exception("Invalid token before position " + to_string(lex.cur_pos) + ". Expected type");
        }
        return r;
    }

    tree *A() {
        tree *r = new tree("A");
        switch (lex.cur_token) {
            case Token::NAME:
                if (!possible_types.count(lex.cur_string)) {
                    throw parse_exception("Unknown type before position: " + to_string(lex.cur_pos));
                }
                r->children.push_back(new tree("N"));
                break;
            default:
                throw parse_exception(
                        "Invalid token before position " + to_string(lex.cur_pos) + ". Expected variable name");
        }
        lex.next_token();
        r->children.push_back(V());
        return r;
    }

    tree *V() {
        tree *r = new tree("V");
        if (lex.cur_token != Token::NAME && lex.cur_token != Token::STAR) {
            throw parse_exception(
                    "Invalid token before position " + to_string(lex.cur_pos) + ". Expected variable name or *");
        }
        r->children.push_back(S());
        switch (lex.cur_token) {
            case Token::SEMICOLON:
                r->children.push_back(new tree("SEMICOLON"));
                lex.next_token();
                break;
            case Token::COMMA:
                r->children.push_back(new tree("COMMA"));
                lex.next_token();
                r->children.push_back(V());
                break;
            default:
                throw parse_exception("Invalid token before position " + to_string(lex.cur_pos) + ". Expected , or ;");
        }
        return r;
    }

    tree *S1() {
        tree *r = new tree("S1");
        switch (lex.cur_token) {
            case Token::LEFT_BRACKET:
                r->children.push_back(new tree("LEFT_BRACKET"));
                lex.next_token();
                if (lex.cur_token != Token::NUMBER) {
                    throw parse_exception("Expected integral size of array before position " + to_string(lex.cur_pos));
                }
                r->children.push_back(new tree("SIZE"));
                lex.next_token();
                if (lex.cur_token != Token::RIGHT_BRACKET) {
                    throw parse_exception("Invalid token before position " + to_string(lex.cur_pos) + ". Expected ]");
                }
                r->children.push_back(new tree("RIGHT_BRACKET"));
                lex.next_token();
                if (lex.cur_token == Token::LEFT_BRACKET) {
                    r->children.push_back(S1());
                }
                break;
            default:
                throw parse_exception("Invalid token before position " + to_string(lex.cur_pos) + ". Expected [");
        }
        return r;
    }

    tree *S() {
        tree *r = new tree("S");
        switch (lex.cur_token) {
            case Token::NAME:
                r->children.push_back(new tree("N"));
                lex.next_token();
                break;
            case Token::STAR:
                r->children.push_back(new tree("STAR"));
                lex.next_token();
                r->children.push_back(S());
                break;
            default:
                throw parse_exception(
                        "Invalid token before position " + to_string(lex.cur_pos) + ". Expected variable name or *");
        }
        if (lex.cur_token == Token::LEFT_BRACKET) {
            r->children.push_back(S1());
        }
        return r;
    }

    tree *parse(const string &s) {
        lex = lexical_analyzer(s);
        lex.next_token();
        return T();
    }
};

int nxt = 0;

void generate_dot(const tree *root, ofstream &out) {
    if (!root) return;
    int id = nxt++;

    for (const auto &child : root->children) {
        out << root->node << "_" << to_string(id) << " -> " << child->node << "_" << to_string(nxt) << ";";
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

vector<variant<tree *, parse_exception>> expected{
        {},
        {new tree("T",
                  {
                          new tree("END", {})
                  }
        )},
        {
         new tree("T",
                  {
                          new tree("A",
                                   {
                                           new tree("N"),
                                           new tree("V", {
                                                   new tree("S", {
                                                           new tree("N")
                                                   }),
                                                   new tree("SEMICOLON")
                                           })
                                   }),
                          new tree("T", {
                                  new tree("END")
                          })
                  })
        },
        {
         new tree("T",
                  {
                          new tree("A",
                                   {
                                           new tree("N"),
                                           new tree("V", {
                                                   new tree("S", {
                                                           new tree("STAR"),
                                                           new tree("S", {
                                                                   new tree("STAR"),
                                                                   new tree("S", {
                                                                           new tree("N")
                                                                   })
                                                           })
                                                   }),
                                                   new tree("SEMICOLON")
                                           })
                                   }),
                          new tree("T", {
                                  new tree("END")
                          })
                  })
        },
        {
         new tree("T",
                  {
                          new tree("A",
                                   {
                                           new tree("N"),
                                           new tree("V", {
                                                   new tree("S", {
                                                           new tree("N")
                                                   }),
                                                   new tree("COMMA"),
                                                   new tree("V", {
                                                           new tree("S", {
                                                                   new tree("N")
                                                           }),
                                                           new tree("COMMA"),
                                                           new tree("V", {
                                                                   new tree("S", {
                                                                           new tree("N")
                                                                   }),
                                                                   new tree("SEMICOLON")
                                                           })
                                                   })
                                           })
                                   }),
                          new tree("T", {
                                  new tree("END")
                          })
                  })
        },
        {},
        {},
        {parse_exception("No semicolon")},
        {parse_exception("No semicolon")},
        {parse_exception("No comma")},
        {parse_exception("No semicolon")},
        {parse_exception("Many commas")},
        {
         new tree("T",
                  {
                          new tree("A",
                                   {
                                           new tree("N"),
                                           new tree("V", {
                                                   new tree("S", {
                                                           new tree("N"),
                                                           new tree("S1", {
                                                                   new tree("LEFT_BRACKET"),
                                                                   new tree("SIZE"),
                                                                   new tree("RIGHT_BRACKET"),
                                                           })
                                                   }),
                                                   new tree("SEMICOLON")
                                           })
                                   }),
                          new tree("T", {
                                  new tree("END")
                          })
                  })
        },
        {
         new tree("T",
                  {
                          new tree("A",
                                   {
                                           new tree("N"),
                                           new tree("V", {
                                                   new tree("S", {
                                                           new tree("N"),
                                                           new tree("S1", {
                                                                   new tree("LEFT_BRACKET"),
                                                                   new tree("SIZE"),
                                                                   new tree("RIGHT_BRACKET"),
                                                                   new tree("S1", {
                                                                           new tree("LEFT_BRACKET"),
                                                                           new tree("SIZE"),
                                                                           new tree("RIGHT_BRACKET"),
                                                                           new tree("S1", {
                                                                                   new tree("LEFT_BRACKET"),
                                                                                   new tree("SIZE"),
                                                                                   new tree("RIGHT_BRACKET"),
                                                                                   new tree("S1", {
                                                                                           new tree("LEFT_BRACKET"),
                                                                                           new tree("SIZE"),
                                                                                           new tree("RIGHT_BRACKET"),
                                                                                           new tree("S1", {
                                                                                                   new tree(
                                                                                                           "LEFT_BRACKET"),
                                                                                                   new tree("SIZE"),
                                                                                                   new tree(
                                                                                                           "RIGHT_BRACKET"),
                                                                                           })
                                                                                   })
                                                                           })
                                                                   })
                                                           })
                                                   }),
                                                   new tree("SEMICOLON")
                                           })
                                   }),
                          new tree("T", {
                                  new tree("END")
                          })
                  })
        },
        {},
        {},
        {parse_exception("No size")},
        {parse_exception("Unknon type integer")}
};

bool deep_equal(tree *t1, tree *t2) {
    assert(t1 && t2);
    if (t1->children.size() != t2->children.size())
        return false;
    if (t1->node != t2->node)
        return false;
    for (int i = 0; i < t1->children.size(); ++i) {
        if (!deep_equal(t1->children[i], t2->children[i]))
            return false;
    }
    return true;
}

void run_test(int t) {
    if (t == 5 || t == 6 || t == 14 || t == 15) { // Big tests can validate only by eyes
        return;
    }
    ifstream file("test" + to_string(t) + ".txt");
    if (!file.is_open()) {
        cerr << "Error opening file for test " << t << endl;
        return;
    }

    stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    string content = buffer.str();

    parser p;
    tree *r;
    try {
        r = p.parse(content);
        if (holds_alternative<parse_exception>(expected[t])) {
            cerr << "Test " << t << " failed:\nExpected error, but no error was thrown\n";
            return;
        }
        if (!deep_equal(get<tree *>(expected[t]), r)) {
            cerr << "Wrong tree in test " << t << endl;
            cerr << "Check expected.png and actual.png to see difference" << endl;
            save_tree_to_dot(get<tree *>(expected[t]), "expected.dot");
            system("dot -Tpng expected.dot -o expected.png");
            save_tree_to_dot(r, "actual.dot");
            system("dot -Tpng actual.dot -o actual.png");
            return;
        }
    } catch (const parse_exception &e) {
        if (holds_alternative<tree *>(expected[t])) {
            cerr << "Unexpected error in test " << t << endl;
        }
        return;
    }
//    save_tree_to_dot(r, "tree.dot");
//    system("dot -Tpng tree.dot -o tree.png");
}

int main() {
    for (int t = 1; t <= 17; ++t) {
        run_test(t);
    }
    return 0;
}

/*
Тесты:
1 - Файл из пробельных символов
2 - Базовое объявление
3 - Базовое объявление указателя
4 - Несколько переменных
5 - Несколько переменных с указателями
6 - Несколько описаний
7 - Отсутствие точки с запятой
8 - Отсутствие точки с запятой, много переменных
9 - Отсутствие запятой между именами
10 - Отсутствие точки с запятой между описаниями
11 - Пустые переменные (повторяющиеся запятые)
12 - Одномерный массив
13 - Многомерный массив
14 - Много массивов
15 - Куча всего
16 - Массив без указания размера
17 - Неправильный тип
 */