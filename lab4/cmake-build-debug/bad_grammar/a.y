{%
%}

{%

int nxt = 0;

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

tree* yyparse();

int main() {
    freopen("input.txt", "r", stdin);
    tree* r = yyparse();
    save_tree_to_dot(r, "actual.dot");
    system("dot -Tpng actual.dot -o actual.png");
    return 0;
}
%}

%union { int* x; };

%token <x> NUMBER PLUS STAR LEFT_BRACKET RIGHT_BRACKET MINUS
%type <x> E E1 T T1 F TOP Q F1

%%

TOP:
    E {} {
        $$ = new int(*($1));
        std::cout << *($$) << "\n";
    }
    ;
E:
    T E1 {} {
        $$ = new int(*($1) + *($2));
    }
    ;
E1:
    PLUS E {} { $$ = new int(*($2)); }
    | MINUS T Q {} { $$ = new int(-(*($2)) + *($3)); }
    | EMPTY {} { $$ = new int(0); }
    ;
Q:
    E1 {} { $$ = new int(*($1)); }
    | EMPTY {} { $$ = new int(0); }
    ;
T:
    F T1 {} { $$ = new int(*($1) * *($2)); }
    ;
T1:
    STAR F T1 {} { $$ = new int(*($2) * *($3)); }
    | EMPTY {} { $$ = new int(1); }
    ;
F:
    MINUS F1 {} { $$ = new int(-(*($2))); }
    | F1 {} { $$ = new int(*($1)); }
    ;
F1:
    NUMBER {} { $$ = new int(*($1)); }
    | LEFT_BRACKET E RIGHT_BRACKET {} { $$ = new int(*($2)); }
    ;
%%
