%{
#include <stdio.h>
#include <string.h>
#include "TeaplAst.h"
#include "y.tab.hpp"
extern int line, col;
int c;
int calc(char *s, int len);
char* tostr(char *s, int len);
%}


%start COMMENT1 COMMENT2

%%

<INITIAL>"+" { yylval.pos = A_Pos(line, col); col += yyleng; return ADD; }
<INITIAL>"-" { yylval.pos = A_Pos(line, col); col += yyleng; return SUB; }
<INITIAL>"*" { yylval.pos = A_Pos(line, col); col += yyleng; return MUL; }
<INITIAL>"/" { yylval.pos = A_Pos(line, col); col += yyleng; return DIV; }

<INITIAL>"&&" { yylval.pos = A_Pos(line, col); col += yyleng; return AND; }
<INITIAL>"||" { yylval.pos = A_Pos(line, col); col += yyleng; return OR; }

<INITIAL>"!" { yylval.pos = A_Pos(line, col); col += yyleng; return NOT; }

<INITIAL>">" { yylval.pos = A_Pos(line, col); col += yyleng; return GT; }
<INITIAL>"<" { yylval.pos = A_Pos(line, col); col += yyleng; return LT; }
<INITIAL>">=" { yylval.pos = A_Pos(line, col); col += yyleng; return GTE; }
<INITIAL>"<=" { yylval.pos = A_Pos(line, col); col += yyleng; return LTE; }
<INITIAL>"!=" { yylval.pos = A_Pos(line, col); col += yyleng; return NEQ; }
<INITIAL>"==" { yylval.pos = A_Pos(line, col); col += yyleng; return EQ; }

<INITIAL>"=" { yylval.pos = A_Pos(line, col); col += yyleng; return AS; }

<INITIAL>"(" { yylval.pos = A_Pos(line, col); col += yyleng; return LPAR; }
<INITIAL>")" { yylval.pos = A_Pos(line, col); col += yyleng; return RPAR; }
<INITIAL>"[" { yylval.pos = A_Pos(line, col); col += yyleng; return LSQ; }
<INITIAL>"]" { yylval.pos = A_Pos(line, col); col += yyleng; return RSQ; }
<INITIAL>"{" { yylval.pos = A_Pos(line, col); col += yyleng; return LBRA; }
<INITIAL>"}" { yylval.pos = A_Pos(line, col); col += yyleng; return RBRA; }

<INITIAL>"," { yylval.pos = A_Pos(line, col); col += yyleng; return COMMA; }
<INITIAL>";" { yylval.pos = A_Pos(line, col); col += yyleng; return SEMICOLON; }
<INITIAL>"->" { yylval.pos = A_Pos(line, col); col += yyleng; return RARROW; }
<INITIAL>":" { yylval.pos = A_Pos(line, col); col += yyleng; return COLON; }
<INITIAL>"." { yylval.pos = A_Pos(line, col); col += yyleng; return DOT; }

<INITIAL>"fn" { yylval.pos = A_Pos(line, col); col += yyleng; return FN; }
<INITIAL>"let" { yylval.pos = A_Pos(line, col); col += yyleng; return LET; }

<INITIAL>"int" { yylval.pos = A_Pos(line, col); col += yyleng; return INT; }
<INITIAL>"struct" { yylval.pos = A_Pos(line, col); col += yyleng; return STRUCT; }

<INITIAL>"if" { yylval.pos = A_Pos(line, col); col += yyleng; return IF; }
<INITIAL>"else" { yylval.pos = A_Pos(line, col); col += yyleng; return ELSE; }
<INITIAL>"while" { yylval.pos = A_Pos(line, col); col += yyleng; return WHILE; }

<INITIAL>"ret" { yylval.pos = A_Pos(line, col); col += yyleng; return RET; }
<INITIAL>"break" { yylval.pos = A_Pos(line, col); col += yyleng; return BREAK; }
<INITIAL>"continue" { yylval.pos = A_Pos(line, col); col += yyleng; return CONTINUE; }

<INITIAL>"//" { col += yyleng; BEGIN COMMENT1; }
<COMMENT1>"\n" { ++line; col = 1; BEGIN INITIAL; }
<COMMENT1>.    { col += yyleng; }

<INITIAL>"/*" { col += yyleng; BEGIN COMMENT2; }

<COMMENT2>"*/" { col += yyleng; BEGIN INITIAL; }
<COMMENT2>"\n" { ++line; col = 1; }
<COMMENT2>. { col += yyleng; }

<INITIAL>"\n" { line++; col = 1; }
<INITIAL>" " { col += 1; }
<INITIAL>"\t" { col += 4; }

<INITIAL>[a-z_A-Z][a-z_A-Z0-9]* { 
    yylval.tokenId = A_TokenId(A_Pos(line, col), tostr(yytext, yyleng));
    col += yyleng;
    return ID;
}

<INITIAL>[1-9][0-9]* {
    yylval.tokenNum = A_TokenNum(A_Pos(line, col), calc(yytext, yyleng));
    col+=yyleng;
    return NUM;
}

<INITIAL>0 {
    yylval.tokenNum = A_TokenNum(A_Pos(line, col), 0);
    ++col;
    return NUM;
}

<INITIAL>. { printf("Illegal input \"%c\"\n", yytext[0]); }

%%

// This function takes a string of digits and its length as input, and returns the integer value of the string.
int calc(char *s, int len) {
    int ret = 0;
    for(int i = 0; i < len; i++)
        ret = ret * 10 + (s[i] - '0');
    return ret;
}

char* tostr(char* s, int len) {
    char* ret = (char*)malloc(len + 1);
    if (ret == NULL) {
        printf("malloc failed");
        return NULL;
    }
    strncpy(ret, s, len);
    ret[len] = '\0';
    return ret;
}