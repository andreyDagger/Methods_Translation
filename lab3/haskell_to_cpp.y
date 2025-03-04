%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include <vector>
#include <variant>

inline void yyerror(const char *s) { 
    std::cout << s << std::endl;
}

struct s_function_definition {
    std::string return_type;
    std::string function_name;
    std::vector<std::string> arg_types;
};

using s_guards = std::vector<std::pair<std::string, std::string>>;

struct s_pattern_matching {
    std::vector<std::pair<int, std::string>> known;
    s_guards gs;
};

struct s_function {
    s_function_definition function_definition;
    std::vector<s_pattern_matching> pattern_matchings;
};

void print_function(const s_function& f) {
    std::cout << f.function_definition.return_type << " " << f.function_definition.function_name;
    std::cout << "(";
    for (int i = 0; i < f.function_definition.arg_types.size(); ++i) {
        std::cout << f.function_definition.arg_types.at(i) << " var" << i;
        if (i + 1 < f.function_definition.arg_types.size()) {
            std::cout << ", ";
        }
    }
    std::cout << ") {" << std::endl;
    for (auto pm : f.pattern_matchings) {
        std::cout << "if (";
	for (int i = 0; i < pm.known.size(); ++i) {
            auto [num, val] = pm.known.at(i);
	    std::cout << "var" << num << " == " << val;
	    if (i + 1 < pm.known.size()) {
		std::cout << " && ";
	    }
	}
	if (pm.known.empty()) {
	    std::cout << "true";
        }
        std::cout << ") {" << std::endl;
        for (auto& [cond, res] : pm.gs) {
            std::cout << "if (" << cond << ") return " << res << ";" << std::endl;
        }
        std::cout << std::endl;
        std::cout << "}" << std::endl;
    }
    std::cout << "}";
    std::cout << std::endl;
}

void print_functions(const std::vector<struct s_function>& vf) {
    for (auto&& f : vf) {
        print_function(f);
        std::cout << std::endl;
    }
}

extern int yylex();
extern int yyparse();

int main(int argc, char** argv){
  if (argc != 3) {
      std::cerr << "Usage: [input_file] [output_file]" << std::endl;
      return 1;
  }
  freopen(argv[1], "r", stdin);
  freopen(argv[2], "w", stdout);
  yyparse();
  return 0;  
}

%}

%union {std::string* s;
        std::vector<std::string>* vs;
        struct s_function* f;
        std::vector<struct s_function>* vf;
        struct s_function_definition* fd;
        struct s_pattern_matching* pm;
        std::vector<struct s_pattern_matching>* vpm;
        std::vector<std::pair<std::string, bool>>* vpsb;
        std::vector<std::pair<std::string, std::string>>* vpss;
       }

%token <s> IDENTIFIER TYPE COLONCOLON ARROW EQUALITY NUMBER QUOTED_STRING SEMICOLON VBAR DOUBLEEQ GREATEREQ LESSEQ GREATER LESS
%type <s> input arg_type return_type arg result cond
%type <vs> arg_list
%type <fd> function_definition
%type <pm> pattern_matching
%type <vpm> pattern_matchings
%type <vpsb> pattern
%type <f> function
%type <vf> functions
%type <vpss> guards

%%

// Grammar rules
input:
    functions {
        print_functions(*$1);
    }
    ;

functions:
    function {
        $$ = new std::vector<s_function>{*$function};
    }
    | functions SEMICOLON function {
        $$->push_back(*$function);
    }
    ;

function:
    function_definition pattern_matchings {
        $$ = new s_function();
        $$->function_definition = *$function_definition;
        $$->pattern_matchings = *$pattern_matchings;
    }
    ;

function_definition:
    IDENTIFIER COLONCOLON arg_list ARROW return_type {
        $$ = new s_function_definition();
        $$->return_type = *$return_type;
        $$->function_name = *$IDENTIFIER;
        $$->arg_types = *$arg_list;
    }
    | IDENTIFIER COLONCOLON return_type {
        $$ = new s_function_definition();
        $$->return_type = *$return_type;
        $$->function_name = *$IDENTIFIER;
    }
    ;

pattern_matchings:
    pattern_matching {
        $$ = new std::vector<s_pattern_matching>{*$pattern_matching};
    }
    | pattern_matchings pattern_matching {
        $$->push_back(*$pattern_matching);
    }
    ;

pattern_matching:
    IDENTIFIER pattern EQUALITY result {
        $$ = new s_pattern_matching();
        $$->gs.emplace_back("true", *$result);
        for (int i = 0; i < $pattern->size(); ++i) {
            if ($pattern->at(i).second) {
                $$->known.emplace_back(i, $pattern->at(i).first);
            }
        }
    }
    | IDENTIFIER EQUALITY result {
        $$ = new s_pattern_matching();
        $$->gs.emplace_back("true", *$result);
    }
    | IDENTIFIER pattern guards {
        $$ = new s_pattern_matching();
        $$->gs = *$guards;
        for (int i = 0; i < $pattern->size(); ++i) {
            if ($pattern->at(i).second) {
                $$->known.emplace_back(i, $pattern->at(i).first);
            }
        }
    }
    | IDENTIFIER guards {
        $$ = new s_pattern_matching();
        $$->gs = *$guards;
    }
    ;
   
guards:
    VBAR cond EQUALITY result {
        $$ = new std::vector<std::pair<std::string, std::string>>{{*$cond, *$result}};
    }
    | guards VBAR cond EQUALITY result {
        $$->emplace_back(*$cond, *$result);
    }
    ;
    
cond:
    arg DOUBLEEQ arg {
        $$ = new std::string(*$1 + "==" + *$3);
    }
    | arg GREATEREQ arg {
        $$ = new std::string(*$1 + ">=" + *$3);
    }
    | arg LESSEQ arg {
        $$ = new std::string(*$1 + "<=" + *$3);
    }
    | arg GREATER arg {
        $$ = new std::string(*$1 + ">" + *$3);
    }
    | arg LESS arg {
        $$ = new std::string(*$1 + "<" + *$3);
    }
;

pattern:
    NUMBER {
        $$ = new std::vector<std::pair<std::string, bool>>{{*$1, true}};
    }
    | QUOTED_STRING {
        $$ = new std::vector<std::pair<std::string, bool>>{{*$1, true}};
    } 
    | IDENTIFIER {
        $$ = new std::vector<std::pair<std::string, bool>>{{*$1, false}};
    } 
    | pattern NUMBER {
        $$->emplace_back(*$NUMBER, true);
    } 
    | pattern QUOTED_STRING {
        $$->emplace_back(*$QUOTED_STRING, true);
    }
    | pattern IDENTIFIER {
        $$->emplace_back(*$IDENTIFIER, false);
    }
    ;

arg:
    NUMBER | QUOTED_STRING {
        $$ = new std::string(*$1);
    }
    | IDENTIFIER {
        $$ = new std::string(*$1);
    }
    ;

result:
    NUMBER | QUOTED_STRING {
        $$ = new std::string(*$1);
    }
    ;

arg_list:
    arg_type {
        $$ = new std::vector<std::string>{*$1};
    }
    | arg_list ARROW arg_type {
        $$->push_back(*$3);
    }
    ;

arg_type:
    TYPE {
        $$ = new std::string(*$1);
    }
    ;

return_type:
    TYPE {
        $$ = new std::string(*$1);
    }
    ;

%%

