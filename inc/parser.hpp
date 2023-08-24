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

#ifndef YY_YY_INC_PARSER_HPP_INCLUDED
# define YY_YY_INC_PARSER_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    LITERAL = 258,
    FLOAT = 259,
    SYMB = 260,
    HEX = 261,
    REG = 262,
    LABEL = 263,
    MEMDIR = 264,
    WS = 265,
    GLOBAL = 266,
    EXTERN = 267,
    SECTION = 268,
    WORD = 269,
    SKIP = 270,
    ASCII = 271,
    EQU = 272,
    END = 273,
    COM = 274,
    INTR = 275,
    CALL = 276,
    QUOTE = 277,
    HALT = 278,
    IRET = 279,
    RET = 280,
    JMP = 281,
    BEQ = 282,
    BNE = 283,
    BGT = 284,
    PUSH = 285,
    POP = 286,
    XCHG = 287,
    ADD = 288,
    SUB = 289,
    MUL = 290,
    DIV = 291,
    NOT = 292,
    AND = 293,
    OR = 294,
    XOR = 295,
    SHL = 296,
    SHR = 297,
    LD = 298,
    ST = 299,
    CSRRD = 300,
    CSRWR = 301,
    DOLLAR = 302,
    PERCENT = 303,
    LPAR = 304,
    RPAR = 305,
    STATUS = 306,
    HANDLER = 307,
    CAUSE = 308,
    PLUS = 309
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 16 "../misc/bison.y"

  int ival;
  long lval;
  int reg;
  float fval;
  char *sval;
  char *hexval;
  char *label;
  char *memident;

#line 123 "../inc/parser.hpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_INC_PARSER_HPP_INCLUDED  */
