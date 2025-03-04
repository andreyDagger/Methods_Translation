/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    IDENTIFIER = 258,
    TYPE = 259,
    COLONCOLON = 260,
    ARROW = 261,
    EQUALITY = 262,
    NUMBER = 263,
    QUOTED_STRING = 264,
    SEMICOLON = 265,
    VBAR = 266,
    DOUBLEEQ = 267,
    GREATEREQ = 268,
    LESSEQ = 269,
    GREATER = 270,
    LESS = 271
  };
#endif
/* Tokens.  */
#define IDENTIFIER 258
#define TYPE 259
#define COLONCOLON 260
#define ARROW 261
#define EQUALITY 262
#define NUMBER 263
#define QUOTED_STRING 264
#define SEMICOLON 265
#define VBAR 266
#define DOUBLEEQ 267
#define GREATEREQ 268
#define LESSEQ 269
#define GREATER 270
#define LESS 271

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 88 "haskell_to_cpp.y"
std::string* s;
        std::vector<std::string>* vs;
        struct s_function* f;
        std::vector<struct s_function>* vf;
        struct s_function_definition* fd;
        struct s_pattern_matching* pm;
        std::vector<struct s_pattern_matching>* vpm;
        std::vector<std::pair<std::string, bool>>* vpsb;
        std::vector<std::pair<std::string, std::string>>* vpss;
       

#line 101 "y.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
