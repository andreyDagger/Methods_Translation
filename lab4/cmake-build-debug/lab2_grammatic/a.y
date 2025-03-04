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

%union { };

%%

T:
    A T {} {}
    | EMPTY {} {}
    ;
A:
    VARIABLE V {} {}
    ;
V:
    S V1 {} {}
    ;
V1:
    COMMA V {} {}
    | SEMICOLON {} {}
    ;
S:
    Stars VARIABLE Arrays {} {}
    ;
Stars:
    STAR Stars {} {}
    | EMPTY {} {}
    ;
Arrays:
    LEFT_BRACKET NUMBER RIGHT_BRACKET Arrays {} {}
    | EMPTY {} {}
    ;
%%