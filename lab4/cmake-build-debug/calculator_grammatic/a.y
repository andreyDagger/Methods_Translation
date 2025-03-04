{%
struct poly_int;
%}

{%

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

%}

%union {
    poly_int* x;
    pair<int, poly_int>* pii;
    string* s;
};

%token <s> PLUS STAR LEFT_BRACKET RIGHT_BRACKET MINUS SLASH
%token <x> NUMBER REAL
%type <x> E E1 T F TOP Q F1
%type <pii> T1

%%

TOP:
    E {} {
        $$ = new poly_int(*($1));
        reduced[$@] = *($$);
    }
    ;
E:
    T E1 {} {
        $$ = new poly_int(*($1) + *($2));
        reduced[$@] = *($$);
    }
    ;
E1:
    PLUS E {} {
        $$ = new poly_int(*($2));
        reduced[$@] = "+" + string(*($$));
    }
    | MINUS T Q {} {
        $$ = new poly_int(-(*($2)) + *($3));
        reduced[$@] = string(*($$));
    }
    | EMPTY {} {
        $$ = new poly_int(0);
        reduced[$@] = "";
    }
    ;
Q:
    E1 {} {
        $$ = new poly_int(*($1));
        reduced[$@] = *($$);
    }
    | EMPTY {} {
        $$ = new poly_int(0);
        reduced[$@] = "";
    }
    ;
T:
    F T1 {} {
        if (($2)->first == 1) $$ = new poly_int(*($1) / ($2)->second);
        else $$ = new poly_int(*($1) * ($2)->second);
        reduced[$@] = *($$);
    }
    ;
T1:
    STAR F T1 {} {
        if (($3)->first == 1)
            $$ = new pair<int, poly_int>(0, *($2) / ($3)->second);
        else
            $$ = new pair<int, poly_int>(0, *($2) * ($3)->second);
        reduced[$@] = "*" + string(($$)->second);
    }
    | SLASH F T1 {} {
        if (($3)->first == 1)
            $$ = new pair<int, poly_int>(1, *($2) / ($3)->second);
        else
            $$ = new pair<int, poly_int>(1, *($2) * ($3)->second);
        reduced[$@] = "/" + string(($$)->second);
    }
    | EMPTY {} {
        $$ = new pair<int, poly_int>(0, 1);
        reduced[$@] = "";
    }
    ;
F:
    MINUS F1 {} {
        $$ = new poly_int(-(*($2)));
        reduced[$@] = *($$);
    }
    | F1 {} {
        $$ = new poly_int(*($1));
        reduced[$@] = *($$);
    }
    ;
F1:
    REAL {} {
        $$ = new poly_int(*($1));
        reduced[$@] = *($$);
    }
    | NUMBER {} {
        $$ = new poly_int(*($1));
        reduced[$@] = *($$);
    }
    | LEFT_BRACKET E RIGHT_BRACKET {} {
        $$ = new poly_int(*($2));
        reduced[$@] = *($$);
    }
    ;
%%
