{%

std::function<int(int, int)> PLUS = [](int x, int y) { return x + y; };
std::function<int(int, int)> MUL = [](int x, int y) { return x * y; };
std::function<int(int, int)> SECOND = [](int x, int y) { return y; };

struct node_op {
    int x;
    std::function<int(int, int)> f;

    node_op(int x, std::function<int(int, int)> f) : x(x), f(std::move(f)) {}
    node_op(int x) : node_op(x, SECOND) {}
};

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
    node_op* x;
};

%token <x> NUMBER STAR PLUS LEFT_BRACKET RIGHT_BRACKET
%type <x> E E1 T T1 F TOP

%%

TOP:
    E {
        $$ = new node_op(0, SECOND);
    }
    {
        std::cout << ($$)->x << "\n";
    }
    ;
E:
    T E1 {
        $$ = new node_op(0, PLUS);
    }
    {
        ($#)->x = (($#)->f)(($#)->x, ($$)->x);
    }
    ;
E1:
    PLUS E {
        $$ = new node_op(0, SECOND);
    }
    {
        ($#)->x = (($#)->f)(($#)->x, ($$)->x);
    }
    | EMPTY {} {
        $$ = new node_op(0);
        ($#)->x = (($#)->f)(($#)->x, ($$)->x);
    }
    ;
T:
    F T1 {
        $$ = new node_op(1, MUL);
    }
    {
        ($#)->x = (($#)->f)(($#)->x, ($$)->x);
    }
    ;
T1:
    STAR F T1 {
        $$ = new node_op(1, MUL);
    }
    {
        ($#)->x = (($#)->f)(($#)->x, ($$)->x);
    }
    | EMPTY {} {
        $$ = new node_op(1);
        ($#)->x = (($#)->f)(($#)->x, ($$)->x);
    }
    ;
F:
    NUMBER {} {
        $$ = new node_op(*($1));
        ($#)->x = (($#)->f)(($#)->x, ($$)->x);
    }
    | LEFT_BRACKET E RIGHT_BRACKET {
        $$ = new node_op(0, SECOND);
    }
    {
        ($#)->x = (($#)->f)(($#)->x, ($$)->x);
    }
    ;
%%
