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

%union {
    int* val_int;
    bool* val_bool;
    std::pair<std::string, int>* val_psi;
};

%token <val_int> LESS LESS_OR_EQUAL GREATER GREATER_OR_EQUAL PLUS NUMBER
%type <val_int> Q Q1
%type <val_bool> TOP
%type <val_psi> E

%%

TOP:
    Q E {} {
        if ($2->first == "<") $$ = new bool(*($1) < $2->second);
        if ($2->first == "<=") $$ = new bool(*($1) <= $2->second);
        if ($2->first == ">") $$ = new bool(*($1) > $2->second);
        if ($2->first == ">=") $$ = new bool(*($1) >= $2->second);

        cout << *($$) << "\n";
    }
    ;
E:
    LESS Q {} {
        $$ = new std::pair<std::string, int>("<", *($2));
    }
    | LESS_OR_EQUAL Q {} {
        $$ = new std::pair<std::string, int>("<=", *($2));
    }
    | GREATER Q {} {
        $$ = new std::pair<std::string, int>(">", *($2));
    }
    | GREATER_OR_EQUAL Q {} {
        $$ = new std::pair<std::string, int>(">=", *($2));
    }
    ;
Q:
    NUMBER Q1 {} {
        $$ = new int(*($1) + *($2));
    }
    ;
Q1:
    PLUS Q {} {
        $$ = new int(*($2));
    }
    | EMPTY {} {
        $$ = new int(0);
    }
    ;
%%
