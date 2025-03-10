#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <time.h>
#include "TeaplAst.h"
#include "TeaplaAst.h"
#include <unordered_map>

// token id to token type, including function name to return type
typedef struct tc_type_* tc_type;
typedef std::unordered_map<string, tc_type> typeMap; 

// func name to params
typedef std::unordered_map<string, vector<aA_varDecl>*> paramMemberMap; 

/* ============  Added  ============= */
typedef std::unordered_map<string, int> intMap; 

/* ============   End   ============= */


void check_Prog(std::ostream& out, aA_program p);
void check_VarDecl(std::ostream& out, aA_varDeclStmt vd);
void check_StructDef(std::ostream& out, aA_structDef sd);
void check_FnDecl(std::ostream& out, aA_fnDecl fd);
void check_FnDeclStmt(std::ostream& out, aA_fnDeclStmt fd);
void check_FnDef(std::ostream& out, aA_fnDef fd);
void check_CodeblockStmt(std::ostream& out, aA_codeBlockStmt cs);
void check_AssignStmt(std::ostream& out, aA_assignStmt as);
void check_ArrayExpr(std::ostream& out, aA_arrayExpr ae);
tc_type check_MemberExpr(std::ostream& out, aA_memberExpr me);
void check_IfStmt(std::ostream& out, aA_ifStmt is);
void check_BoolExpr(std::ostream& out, aA_boolExpr be);
void check_BoolUnit(std::ostream& out, aA_boolUnit bu);
tc_type check_ExprUnit(std::ostream& out, aA_exprUnit eu);
tc_type check_ArithExpr(std::ostream& out, aA_arithExpr ae);
void check_FuncCall(std::ostream& out, aA_fnCall fc);
void check_WhileStmt(std::ostream& out, aA_whileStmt ws);
void check_CallStmt(std::ostream& out, aA_callStmt cs);
void check_ReturnStmt(std::ostream& out, aA_returnStmt rs);

struct tc_type_{
    aA_type type;
    uint isVarArrFunc; // 0 for scalar, 1 for array, 2 for function
};


/* ============  Added  ============= */
bool check_Defined(std::ostream& out, string id);
tc_type val_of_rightVal(std::ostream& out, aA_rightVal right);
void add_var(aA_varDeclStmt vd, typeMap* tmp, intMap* len);
void add_var_def(aA_varDef vdef, typeMap* tmp, intMap* len);
void add_var_decl(aA_varDecl vdecl, typeMap* tmp, intMap* len);
int get_len(string s);
tc_type pinpoint(string s);
void initialize();
string get_return_type(tc_type tp);
/* ============   End   ============= */