%{
#include <stdio.h>
#include "TeaplaAst.h"

extern A_pos pos;
extern A_program root;

extern int yylex(void);
extern "C"{
extern void yyerror(char *s); 
extern int  yywrap();
}

%}

// TODO:
// your parser

// %union {
//   A_pos pos;
//   A_program program;
//   A_programElementList programElementList;
//   A_programElement programElement;
//   A_arithExpr arithExpr;
//   A_exprUnit exprUnit;
//   A_structDef structDef;
//   A_varDeclStmt varDeclStmt;
//   A_fnDeclStmt fnDeclStmt;
//   A_fnDef fnDef;
// }

// copy and rewrite from TeaplAst.h
%union {
  A_pos pos;
  A_type type;
  A_varDecl varDecl;
  A_varDef varDef;
  A_rightVal rightVal;
  A_arithExpr arithExpr;
  A_boolExpr boolExpr;
  // A_arithBiOpExpr arithBiOpExpr;
  A_arithUExpr arithUExpr;
  A_exprUnit exprUnit;
  A_fnCall fnCall;
  A_indexExpr indexExpr;
  A_arrayExpr arrayExpr;
  A_memberExpr memberExpr;
  A_boolUnit boolUnit;
  // A_boolBiOpExpr boolBiOpExpr;
  A_boolUOpExpr boolUOpExpr;
  A_comExpr comExpr;
  A_leftVal leftVal;
  A_assignStmt assignStmt;
  A_rightValList rightValList;
  A_varDefScalar varDefScalar;
  A_varDefArray varDefArray;
  A_varDeclScalar varDeclScalar;
  A_varDeclArray varDeclArray;
  A_varDeclStmt varDeclStmt;
  A_varDeclList varDeclList;
  A_structDef structDef;
  A_paramDecl paramDecl;
  A_fnDecl fnDecl;
  A_fnDef fnDef;
  A_codeBlockStmt codeBlockStmt;
  A_ifStmt ifStmt;
  A_whileStmt whileStmt;
  A_fnDeclStmt fnDeclStmt;
  A_callStmt callStmt;
  A_returnStmt returnStmt;
  A_programElement programElement;
  A_codeBlockStmtList codeBlockStmtList;
  A_programElementList programElementList;
  A_program program;
  A_tokenId tokenId;
  A_tokenNum tokenNum;
}

%token <pos> ADD
%token <pos> SUB
%token <pos> MUL
%token <pos> DIV
// %token <pos> SEMICOLON // ;

%token <pos> AND
%token <pos> OR

%token <pos> NOT

%token <pos> GT
%token <pos> LT
%token <pos> GTE
%token <pos> LTE
%token <pos> NEQ
%token <pos> EQ

%token <pos> AS

%token <pos> LPAR
%token <pos> RPAR
%token <pos> LSQ
%token <pos> RSQ
%token <pos> LBRA
%token <pos> RBRA

%token <pos> COMMA
%token <pos> SEMICOLON
%token <pos> RARROW
%token <pos> COLON
%token <pos> DOT

%token <pos> FN
%token <pos> LET

%token <pos> INT
%token <pos> STRUCT

%token <pos> IF
%token <pos> ELSE
%token <pos> WHILE

%token <pos> RET
%token <pos> BREAK
%token <pos> CONTINUE

%token <tokenId> ID 
%token <tokenNum> NUM


// %type <program> Program
// %type <arithExpr> ArithExpr
// %type <programElementList> ProgramElementList
// %type <programElement> ProgramElement
// %type <exprUnit> ExprUnit
// %type <structDef> StructDef
// %type <varDeclStmt> VarDeclStmt
// %type <fnDeclStmt> FnDeclStmt
// %type <fnDef> FnDef

%type <type> Type
%type <varDecl> VarDecl
%type <varDef> VarDef
%type <rightVal> RightVal
%type <arithExpr> ArithExpr
%type <boolExpr> BoolExpr
// %type <arithBiOpExpr> ArithBiOpExpr
%type <arithUExpr> ArithUExpr
%type <exprUnit> ExprUnit
%type <fnCall> FnCall
%type <indexExpr> IndexExpr
%type <arrayExpr> ArrayExpr
%type <memberExpr> MemberExpr
%type <boolUnit> BoolUnit
// %type <boolBiOpExpr> BoolBiOpExpr
%type <boolUOpExpr> BoolUOpExpr
%type <comExpr> ComExpr
%type <leftVal> LeftVal
%type <assignStmt> AssignStmt
%type <rightValList> RightValList
%type <varDefScalar> VarDefScalar
%type <varDefArray> VarDefArray
%type <varDeclScalar> VarDeclScalar
%type <varDeclArray> VarDeclArray
%type <varDeclStmt> VarDeclStmt
%type <varDeclList> VarDeclList
%type <structDef> StructDef
%type <paramDecl> ParamDecl
%type <fnDecl> FnDecl
%type <fnDef> FnDef
%type <codeBlockStmt> CodeBlockStmt
%type <ifStmt> IfStmt
%type <whileStmt> WhileStmt
%type <fnDeclStmt> FnDeclStmt
%type <callStmt> CallStmt
%type <returnStmt> ReturnStmt
%type <programElement> ProgramElement
%type <codeBlockStmtList> CodeBlockStmtList
%type <programElementList> ProgramElementList
%type <program> Program


%left ADD SUB
%left MUL DIV
%left AND OR
%right UMINUS NOT
%nonassoc GT LT GTE LTE NEQ EQ
%right AS
%left LPAR RPAR LSQ RSQ LBRA RBRA


%start Program

%%                   /* beginning of rules section */ 

Program: ProgramElementList 
{  
  root = A_Program($1);
  $$ = A_Program($1);
}
;

ProgramElementList: ProgramElement ProgramElementList
{
  $$ = A_ProgramElementList($1, $2);
}
|
{
  $$ = nullptr;
}
;

ProgramElement: VarDeclStmt
{
  $$ = A_ProgramVarDeclStmt($1->pos, $1);
}
| StructDef
{
  $$ = A_ProgramStructDef($1->pos, $1);
}
| FnDeclStmt
{
  $$ = A_ProgramFnDeclStmt($1->pos, $1);
}
| FnDef
{
  $$ = A_ProgramFnDef($1->pos, $1);
}
| SEMICOLON
{
  $$ = A_ProgramNullStmt($1);
}
;

ArithExpr: ArithExpr ADD ArithExpr
{
  $$ = A_ArithBiOp_Expr($1->pos, A_ArithBiOpExpr($1->pos, A_add, $1, $3));
}
| ArithExpr SUB ArithExpr
{
  $$ = A_ArithBiOp_Expr($1->pos, A_ArithBiOpExpr($1->pos, A_sub, $1, $3));
}
| ArithExpr MUL ArithExpr
{
  $$ = A_ArithBiOp_Expr($1->pos, A_ArithBiOpExpr($1->pos, A_mul, $1, $3));
}
| ArithExpr DIV ArithExpr
{
  $$ = A_ArithBiOp_Expr($1->pos, A_ArithBiOpExpr($1->pos, A_div, $1, $3));
}
| ExprUnit
{
  $$ = A_ExprUnit($1->pos, $1);
}
;

ExprUnit: NUM
{
  $$ = A_NumExprUnit($1->pos, $1->num);
}
| ID
{
  $$ = A_IdExprUnit($1->pos, $1->id);
}
| FnCall
{
  $$ = A_CallExprUnit($1->pos, $1);
}
| ArrayExpr
{
  $$ = A_ArrayExprUnit($1->pos, $1);
}
| MemberExpr
{
  $$ = A_MemberExprUnit($1->pos, $1);
}
| ArithUExpr
{
  $$ = A_ArithUExprUnit($1->pos, $1);
}
| LPAR ArithExpr RPAR
{
  $$ = A_ArithExprUnit($1, $2);
}
;

FnCall: ID LPAR RightValList RPAR
{
  $$ = A_FnCall($1->pos, $1->id, $3);
}
;

ArrayExpr: LeftVal LSQ IndexExpr RSQ
{
  $$ = A_ArrayExpr($1->pos, $1, $3);
}
;

MemberExpr: LeftVal DOT ID
{
  $$ = A_MemberExpr($1->pos, $1, $3->id);
}
;

ArithUExpr: SUB ExprUnit %prec UMINUS
{
  $$ = A_ArithUExpr($1, A_neg, $2);
}
;

RightValList: RightVal COMMA RightValList
{
  $$ = A_RightValList($1, $3);
}
| RightVal
{
  $$ = A_RightValList($1, nullptr);
}
|
{
  $$ = nullptr;
}
;

RightVal: ArithExpr
{
  $$ = A_ArithExprRVal($1->pos, $1);
}
| BoolExpr
{
  $$ = A_BoolExprRVal($1->pos, $1);
}
;

BoolExpr: BoolExpr AND BoolExpr
{
  $$ = A_BoolBiOp_Expr($1->pos, A_BoolBiOpExpr($1->pos, A_and, $1, $3));
}
| BoolExpr OR BoolExpr
{
  $$ = A_BoolBiOp_Expr($1->pos, A_BoolBiOpExpr($1->pos, A_or, $1, $3));
}
| BoolUnit
{
  $$ = A_BoolExpr($1->pos, $1);
}
;

BoolUnit: ComExpr
{
  $$ = A_ComExprUnit($1->pos, $1);
}
| LPAR BoolExpr RPAR
{
  $$ = A_BoolExprUnit($1, $2);
}
|
BoolUOpExpr
{
  $$ = A_BoolUOpExprUnit($1->pos, $1);
} 
;

ComExpr: ExprUnit GT ExprUnit
{
  $$ = A_ComExpr($1->pos, A_gt, $1, $3);
}
| ExprUnit GTE ExprUnit
{
  $$ = A_ComExpr($1->pos, A_ge, $1, $3);
}
| ExprUnit LT ExprUnit
{
  $$ = A_ComExpr($1->pos, A_lt, $1, $3);
}
| ExprUnit LTE ExprUnit
{
  $$ = A_ComExpr($1->pos, A_le, $1, $3);
}
| ExprUnit EQ ExprUnit
{
  $$ = A_ComExpr($1->pos, A_eq, $1, $3);
}
| ExprUnit NEQ ExprUnit
{
  $$ = A_ComExpr($1->pos, A_ne, $1, $3);
}
;

BoolUOpExpr: NOT BoolUnit
{
  $$ = A_BoolUOpExpr($1, A_not, $2);
}
;


IndexExpr: NUM
{
  $$ = A_NumIndexExpr($1->pos, $1->num);
}
| ID
{
  $$ = A_IdIndexExpr($1->pos, $1->id);
};

LeftVal: ID
{
  $$ = A_IdExprLVal($1->pos, $1->id);
}
| ArrayExpr
{
  $$ = A_ArrExprLVal($1->pos, $1);
}
| MemberExpr
{
  $$ = A_MemberExprLVal($1->pos, $1);
}
;

VarDeclStmt: LET VarDecl SEMICOLON
{
  $$ = A_VarDeclStmt($1, $2);
}
| LET VarDef SEMICOLON
{
  $$ = A_VarDefStmt($1, $2);
}
;

VarDecl: VarDeclScalar
{
  $$ = A_VarDecl_Scalar($1->pos, $1);
}
| VarDeclArray
{
  $$ = A_VarDecl_Array($1->pos, $1);
}
;

VarDeclScalar: ID COLON Type
{
  $$ = A_VarDeclScalar($1->pos, $1->id, $3);
}
;

Type: INT 
{
  $$ = A_NativeType($1, A_intTypeKind);
}
| ID
{
  $$ = A_StructType($1->pos, $1->id);
}
;

VarDeclArray: ID LSQ NUM RSQ COLON Type
{
  $$ = A_VarDeclArray($1->pos, $1->id, $3->num, $6);
}
;

VarDef: VarDefScalar
{
  $$ = A_VarDef_Scalar($1->pos, $1);
}
| VarDefArray
{
  $$ = A_VarDef_Array($1->pos, $1);
}
;

VarDefScalar: ID COLON Type AS RightVal
{
  $$ = A_VarDefScalar($1->pos, $1->id, $3, $5);
}
;

VarDefArray: ID LSQ NUM RSQ COLON Type AS LBRA RightValList RBRA
{
  $$ = A_VarDefArray($1->pos, $1->id, $3->num, $6, $9);
}
;

StructDef: STRUCT ID LBRA VarDeclList RBRA
{
  $$ = A_StructDef($1, $2->id, $4);
}
;

VarDeclList: VarDecl COMMA VarDeclList
{
  $$ = A_VarDeclList($1, $3);
}
| VarDecl
{
  $$ = A_VarDeclList($1, nullptr);
}
|
{
  $$ = nullptr;
}
;


FnDeclStmt: FnDecl SEMICOLON
{
  $$ = A_FnDeclStmt($1->pos, $1);
}
;

FnDecl: FN ID LPAR ParamDecl RPAR 
{
  $$ = A_FnDecl($1, $2->id, $4, nullptr);
}
| FN ID LPAR ParamDecl RPAR RARROW Type
{
  $$ = A_FnDecl($1, $2->id, $4, $7);
}
;

ParamDecl: VarDeclList
{
  $$ = A_ParamDecl($1);
}
;

FnDef: FnDecl LBRA CodeBlockStmtList RBRA
{
  $$ = A_FnDef($1->pos, $1, $3);
}
;

CodeBlockStmtList: CodeBlockStmt CodeBlockStmtList
{
  $$ = A_CodeBlockStmtList($1, $2);
}
|
{
  $$ = nullptr;
}
;

CodeBlockStmt: VarDeclStmt
{
  $$ = A_BlockVarDeclStmt($1->pos, $1);
}
| AssignStmt
{
  $$ = A_BlockAssignStmt($1->pos, $1);
}
| CallStmt
{
  $$ = A_BlockCallStmt($1->pos, $1);
}
| IfStmt
{
  $$ = A_BlockIfStmt($1->pos, $1);
}
| WhileStmt
{
  $$ = A_BlockWhileStmt($1->pos, $1);
}
| ReturnStmt
{
  $$ = A_BlockReturnStmt($1->pos, $1);
}
| CONTINUE SEMICOLON
{
  $$ = A_BlockContinueStmt($1);
}
| BREAK SEMICOLON
{
  $$ = A_BlockBreakStmt($1);
}
| SEMICOLON
{
  $$ = A_BlockNullStmt($1);
}
;

AssignStmt: LeftVal AS RightVal SEMICOLON
{
  $$ = A_AssignStmt($1->pos, $1, $3);
}
;

CallStmt: FnCall SEMICOLON
{
  $$ = A_CallStmt($1->pos, $1);
}
;

IfStmt: IF LPAR BoolExpr RPAR LBRA CodeBlockStmtList RBRA
{
  $$ = A_IfStmt($1, $3, $6, nullptr);
}
| IF LPAR BoolExpr RPAR LBRA CodeBlockStmtList RBRA ELSE LBRA CodeBlockStmtList RBRA
{
  $$ = A_IfStmt($1, $3, $6, $10);
}
;

WhileStmt: WHILE LPAR BoolExpr RPAR LBRA CodeBlockStmtList RBRA
{
  $$ = A_WhileStmt($1, $3, $6);
}
;

ReturnStmt: RET RightVal SEMICOLON
{
  $$ = A_ReturnStmt($1, $2);
}
| RET SEMICOLON
{
  $$ = A_ReturnStmt($1, nullptr);
}
;

%%

extern "C"{
void yyerror(char * s)
{
  fprintf(stderr, "%s\n",s);
}
int yywrap()
{
  return(1);
}
}


