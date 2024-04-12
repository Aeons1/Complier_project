#include "TypeCheck.h"

//global tabels
//typeMap func2retType; // function name to return type


// TODO: xuya


// global token ids to type
typeMap g_token2Type; // 记录全局变量及其return值

// local token ids to type, since func param can override global param
typeMap funcparam_token2Type;// 记录函数及 return 值
vector<typeMap*> local_token2Type;//各局部变量及 return 值


paramMemberMap func2Param; // 记录函数及其声明值
paramMemberMap struct2Members; // 记录对象


/* ============  Added  ============= */

intMap g_token2ArrLen;
intMap funcparam_token2Arrlen;
vector<intMap*> local_token2ArrLen;
vector<string> func_defined;

/* ============   End   ============= */



// private util functions
void error_print(std::ostream& out, A_pos p, string info)
{
    out << "Typecheck error in line " << p->line << ", col " << p->col << ": " << info << std::endl << std::endl;
    exit(0);
}


void print_token_map(typeMap* map){
    for(auto it = map->begin(); it != map->end(); it++){
        std::cout << it->first << " : ";
        switch (it->second->type->type)
        {
        case A_dataType::A_nativeTypeKind:
            switch (it->second->type->u.nativeType)
            {
            case A_nativeType::A_intTypeKind:
                std::cout << "int";
                break;
            default:
                break;
            }
            break;
        case A_dataType::A_structTypeKind:
            std::cout << *(it->second->type->u.structType);
            break;
        default:
            break;
        }
        switch(it->second->isVarArrFunc){
            case 0:
                std::cout << " scalar";
                break;
            case 1:
                std::cout << " array";
                break;
            case 2:
                std::cout << " function";
                break;
        }
        std::cout << std::endl;
    }
}


void print_token_maps(){
    std::cout << "global token2Type:" << std::endl;
    print_token_map(&g_token2Type);
    std::cout << "local token2Type:" << std::endl;
    print_token_map(&funcparam_token2Type);
}


bool comp_aA_type(aA_type target, aA_type t){
    //std::cout << target->type << " " << t->type <<std::endl;
    if(!target || !t)
        return false;
    if(target->type != t->type)
        return false;
    if(target->type == A_dataType::A_nativeTypeKind)
        if(target->u.nativeType != t->u.nativeType)
            return false;
    if(target->type == A_dataType::A_structTypeKind)
        if(target->u.structType != t->u.structType)
            return false;
    return true;
}


bool comp_tc_type(tc_type target, tc_type t){
    //std::cout << target->isVarArrFunc << " " << t->isVarArrFunc<<std::endl;
    //std::cout << target->type << " " << t->type <<std::endl;
    if(!target || !t)
        return false;
    
    // arr kind first
    if ((target->isVarArrFunc == 1) && (t->isVarArrFunc != 1))// || (target->isVarArrFunc == 1 && t->isVarArrFunc != 1))
        return false;
    
    // if target type is nullptr, alwayse ok
    return comp_aA_type(target->type, t->type);
}


tc_type tc_Type(aA_type t, uint isVarArrFunc){
    tc_type ret = new tc_type_;
    ret->type = t;
    ret->isVarArrFunc = isVarArrFunc;
    return ret;
}


tc_type tc_Type(aA_varDecl vd){
    if(vd->kind == A_varDeclType::A_varDeclScalarKind)
        return tc_Type(vd->u.declScalar->type, 0);
    else if(vd->kind == A_varDeclType::A_varDeclArrayKind)
        return tc_Type(vd->u.declArray->type, 1);
    return nullptr;
}

/* ============  Added  ============= */

void add_var_decl(aA_varDecl vdecl, typeMap* tmp, intMap* len){
    string name; tc_type type;
    if(vdecl->kind == A_varDeclType::A_varDeclScalarKind){
        name = *vdecl->u.declScalar->id;
        type = tc_Type(vdecl->u.declScalar->type, 0);
    }else if (vdecl->kind == A_varDeclType::A_varDeclArrayKind){
        name = *vdecl->u.declArray->id;
        type = tc_Type(vdecl->u.declArray->type, 1);
        (*len)[name] = vdecl->u.declArray->len;
    }
    // std::cout<< "name: " << name<<std::endl;
    (*tmp)[name] = type;
    return;
}

void add_var_def(aA_varDef vdef, typeMap* tmp, intMap* len){
    string name; tc_type type;
    if (vdef->kind == A_varDefType::A_varDefScalarKind){
        name = *vdef->u.defScalar->id;
        type = tc_Type(vdef->u.defScalar->type, 0);
    }else if (vdef->kind == A_varDefType::A_varDefArrayKind){
        name = *vdef->u.defArray->id;
        type = tc_Type(vdef->u.defArray->type, 1);
        (*len)[name] = vdef->u.defArray->len;
    }
    // std::cout<< "name: " << name<<std::endl;
    (*tmp)[name] = type;
    return;
}

void add_var(aA_varDeclStmt vd, typeMap* tmp, intMap* len){
    if (vd->kind == A_varDeclStmtType::A_varDeclKind)
        add_var_decl(vd->u.varDecl, tmp, len);
    else if (vd->kind == A_varDeclStmtType::A_varDefKind)
        add_var_def(vd->u.varDef, tmp, len);
    return;
}

void del_var(typeMap* tmp, intMap* len){
    tmp->clear();
    len->clear();
    return;
}

tc_type val_of_rightVal(std::ostream& out, aA_rightVal right){
    tc_type ret;
    switch(right->kind){
        case A_rightValType::A_arithExprValKind:{
            // std::cout<< "right value of arithexpr" << std::endl;
            ret = check_ArithExpr(out, right->u.arithExpr);
        }
            break;
        case A_rightValType::A_boolExprValKind:{
            // std::cout<< "right value of boolexpr" << std::endl;
            check_BoolExpr(out, right->u.boolExpr);
            aA_type numt = new aA_type_;
            numt->pos = right->pos;
            numt->type = A_dataType::A_nativeTypeKind;
            numt->u.nativeType = A_nativeType::A_intTypeKind;
            ret = tc_Type(numt, 0);
        }
            break;
        default:
            ret = nullptr;
            break;
    }
    return ret;
}

void initialize(){
    local_token2ArrLen.clear();
    local_token2Type.clear();
    g_token2ArrLen.clear();
    g_token2Type.clear();
    funcparam_token2Type.clear();
    funcparam_token2Arrlen.clear();
    func2Param.clear();
    struct2Members.clear();
    func_defined.clear();
}

string get_return_type(tc_type tp){
    string ret;
    if(tp->isVarArrFunc == 1){
        if(tp->type->type == A_dataType::A_nativeTypeKind)
            ret = "int Arr";
        else if(tp->type->type == A_dataType::A_structTypeKind)
            ret = *tp->type->u.structType + " Arr";        
    }else{
        if(tp->type->type == A_dataType::A_nativeTypeKind)
            ret = "int Scalar";
        else if(tp->type->type == A_dataType::A_structTypeKind)
            ret = *tp->type->u.structType + " Scalar";            
    }

    return ret;
}


/* ============   End   ============= */

// public functions
void check_Prog(std::ostream& out, aA_program p)
{
    initialize();
    for (auto ele : p->programElements)
    {
        if(ele->kind == A_programVarDeclStmtKind){
            check_VarDecl(out, ele->u.varDeclStmt);
            add_var(ele->u.varDeclStmt, &g_token2Type, &g_token2ArrLen);
        }else if (ele->kind == A_programStructDefKind){
            check_StructDef(out, ele->u.structDef);
        }
    }
    
    for (auto ele : p->programElements){
        if(ele->kind == A_programFnDeclStmtKind){
            check_FnDeclStmt(out, ele->u.fnDeclStmt);
        }
        else if (ele->kind == A_programFnDefKind){
            check_FnDecl(out, ele->u.fnDef->fnDecl);
        }
    }

    for (auto ele : p->programElements){
        if(ele->kind == A_programFnDefKind){
            check_FnDef(out, ele->u.fnDef);
        }
        else if (ele->kind == A_programNullStmtKind){
            // do nothing
        }
    }

    out << "Typecheck passed!" << std::endl;
    return;
}

//  作用域5：局部变量不能和全局变量重名，与C语言不同。

void check_VarDecl(std::ostream& out, aA_varDeclStmt vd)
{
    if (!vd)
        return;
    string name;
    if (vd->kind == A_varDeclStmtType::A_varDeclKind){
        // decl only
        aA_varDecl vdecl = vd->u.varDecl;
        if(vdecl->kind == A_varDeclType::A_varDeclScalarKind){
            name = *vdecl->u.declScalar->id;
            /* fill code here*/
            /* ============  Added  ============= */
            if(check_Defined(out, name))
                error_print(out, vdecl->pos, "Var '" + name + "' redeclared.");
            /* ============   End   ============= */
        }else if (vdecl->kind == A_varDeclType::A_varDeclArrayKind){
            name = *vdecl->u.declArray->id;
            /* fill code here*/
            /* ============  Added  ============= */
            if(check_Defined(out, name))
                error_print(out, vdecl->pos, "Arr '" + name + "' redeclared.");
            /* ============   End   ============= */
            }
    }
    else if (vd->kind == A_varDeclStmtType::A_varDefKind){
        // decl and def
        aA_varDef vdef = vd->u.varDef;
        if (vdef->kind == A_varDefType::A_varDefScalarKind){
            name = *vdef->u.defScalar->id;
            /* fill code here, allow omited type */
            /* ============  Added  ============= */
            if(check_Defined(out, name))
                error_print(out, vdef->pos, "Var '" + name + "' redeclared.");

            tc_type type     = tc_Type(vdef->u.defScalar->type, 0);
            aA_rightVal val  = vdef->u.defScalar->val;

            tc_type ret = val_of_rightVal(out, val);
            if(!comp_tc_type(type, ret))
                error_print(out, vdef->pos, "Right-value's type inconsistent with desired type.");
            /* ============   End   ============= */
        }else if (vdef->kind == A_varDefType::A_varDefArrayKind){
            name = *vdef->u.defArray->id;
            /* fill code here, allow omited type */
            /* ============  Added  ============= */
            if(check_Defined(out, name))
                error_print(out, vdef->pos, "Arr '" + name + "' redeclared.");

            vector<aA_rightVal> vals = vdef->u.defArray->vals;
            int length = vdef->u.defArray->len;
            if(length != vals.size())
                error_print(out, vdef->pos, "Size of arr '" + name + "' is inconsistent with param list.");

            tc_type type = tc_Type(vdef->u.defArray->type, 1);
            for(int i = 0;i < vals.size();i++){
                tc_type ret = val_of_rightVal(out, vals[i]);
                if(!comp_aA_type(type->type, ret->type))
                    error_print(out, vdef->pos, "Right-value's type inconsistent with desired type.");
            }
            /* ============   End   ============= */
        }
    }
    return;
}


void check_StructDef(std::ostream& out, aA_structDef sd)
{
    if (!sd)
        return;
    string name = *sd->id;
    if (struct2Members.find(name) != struct2Members.end())
        error_print(out, sd->pos, "This id is already defined!");
    struct2Members[name] = &(sd->varDecls);
    return;
}


void check_FnDecl(std::ostream& out, aA_fnDecl fd)
{
    // std::cout<<"Entered FnDecl" << std::endl;
    if (!fd)
        return;
    string name = *fd->id;
    for(int i = 0;i < func_defined.size();i++){
        // std::cout<<"Entered" << std::endl;
        if(func_defined[i] == name)
            error_print(out, fd->pos, "Function '"+ name + "' is already defined and cannot be overrided.");
    }
    tc_type type = tc_Type(fd->type, 2);
    // if already declared, should match
    if (func2Param.find(name) != func2Param.end()){
        /* ============  Added  ============= */
        /* fill code here */
        // is function ret val matches
        if(!comp_tc_type(g_token2Type[name], type)){
            error_print(out, fd->pos, "Function '" + name + "' expects return type '" + get_return_type(g_token2Type[name])
                     +  "', got '" + get_return_type(type) + "' instead.");
        }else{// is function params matches decl
            vector<aA_varDecl> new_vars = fd->paramDecl->varDecls;
            vector<aA_varDecl> vars     = *(func2Param[name]);

            tc_type cur, new_cur;
            if(new_vars.size() != vars.size())
                error_print(out, fd->pos, "Function '" + name + "' has been declared/defined and cannot override it.(Different number of params.)");
            for(int i = 0;i < vars.size();i++){
                if(vars[i]->kind != new_vars[i]->kind)
                    error_print(out, fd->pos, "Function '" + name + "' has been declared/defined and cannot override it.(Different return type of some params.)");
                switch(vars[i]->kind){
                    case A_varDeclType::A_varDeclScalarKind:{
                        cur = tc_Type(vars[i]->u.declScalar->type, 0);
                        new_cur = tc_Type(new_vars[i]->u.declScalar->type, 0); 
                    }
                        break;
                    case A_varDeclType::A_varDeclArrayKind:{
                        cur = tc_Type(vars[i]->u.declArray->type, 1);
                        new_cur = tc_Type(new_vars[i]->u.declArray->type, 1); 
                    }
                        break;
                    default:
                        break;
                }
                if(!comp_tc_type(cur, new_cur))
                    error_print(out, fd->pos, "Function '" + name + "' has been declared/defined and cannot override it.(Different return type of function.)");
                }
            }
        /* ============   End   ============= */
    }else{
        // if not defined
        /* fill code here */
        /* ============  Added  ============= */
        if(check_Defined(out, name) && pinpoint(name)->isVarArrFunc != 2)
            error_print(out, fd->pos, "Function name '" + name + "' can't be same with var.");
        else{
            func2Param[name] = &(fd->paramDecl->varDecls);
            g_token2Type[name] = type;
        }
        /* ============   End   ============= */
    }
    return;
}


void check_FnDeclStmt(std::ostream& out, aA_fnDeclStmt fd)
{
    if (!fd)
        return;
    check_FnDecl(out, fd->fnDecl);
    return;
}


void check_FnDef(std::ostream& out, aA_fnDef fd)
{
    if (!fd)
        return;
    // std::cout<<"Entered FnDef" << std::endl;
    // should match if declared
    check_FnDecl(out, fd->fnDecl);
    string name = *fd->fnDecl->id;
    tc_type ret = g_token2Type[name];
    func_defined.push_back(name);
    /*for(int i = 0;i < func_defined.size();i++){
        std::cout<<func_defined[i] << " ";
    }
    std::cout << std::endl;*/
    //print_token_maps();
    // add params to local tokenmap, func params override global ones
    
    for (aA_varDecl vd : fd->fnDecl->paramDecl->varDecls)
    {
        string tmp_id;
        /* ============  Added  ============= */
        if(vd->kind == A_varDeclType::A_varDeclScalarKind){
            tmp_id = *vd->u.declScalar->id;
            /* fill code here*/
            /* ============  Added  ============= */
            if(funcparam_token2Type.find(name) != funcparam_token2Type.end())
                error_print(out, vd->pos, "Var '" + name + "' redeclared.");
            /* ============   End   ============= */
        }else if (vd->kind == A_varDeclType::A_varDeclArrayKind){
            tmp_id = *vd->u.declArray->id;
            /* fill code here*/
            /* ============  Added  ============= */
            if(funcparam_token2Type.find(name) != funcparam_token2Type.end())
                error_print(out, vd->pos, "Arr '" + name + "' redeclared.");
            /* ============   End   ============= */
        }
        // 进行局部变量的保存。
        add_var_decl(vd, &funcparam_token2Type, &funcparam_token2Arrlen);
        /* ============   End   ============= */
    }
    //print_token_maps();

    typeMap* tmp = new typeMap; intMap* len = new intMap;
    local_token2Type.push_back(tmp);
    local_token2ArrLen.push_back(len);
    // print_token_map(local_token2Type.back());
    int i = 0;
    for (aA_codeBlockStmt stmt : fd->stmts)
    {
        check_CodeblockStmt(out, stmt);
        // std::cout<< name << " " << ++i << " round loc tokens" << std::endl;
        // print_token_map(local_token2Type.back());
        // return value type should match
        /* fill code here */
        /* ============  Added  ============= */  
        if(stmt->kind == A_codeBlockStmtType::A_returnStmtKind){
            // print_token_maps();
            tc_type type = val_of_rightVal(out, stmt->u.returnStmt->retVal);
            if(!comp_tc_type(ret, type)){
                error_print(out, stmt->pos, "Expected return type '" + get_return_type(ret)
                         +  "', got '" + get_return_type(type) + "' instead.");
            }
            // else std::cout<<"Finish" << std::endl;
        }
        /* ============   End   ============= */
    }
    
    /* fill code here */
    /* ============  Added  ============= */  
    if(!local_token2Type.empty())
        local_token2Type.pop_back();
    if(!local_token2ArrLen.empty())
        local_token2ArrLen.pop_back();
    delete tmp; delete len;
    tmp = nullptr, len = nullptr;
    funcparam_token2Type.clear();
    funcparam_token2Arrlen.clear();
    /* ============   End   ============= */
    return;
}


void check_CodeblockStmt(std::ostream& out, aA_codeBlockStmt cs){
    if(!cs)
        return;
    // variables declared in a code block should not duplicate with outer ones.
    switch (cs->kind)
    {
    case A_codeBlockStmtType::A_varDeclStmtKind:{
        check_VarDecl(out, cs->u.varDeclStmt);
        if(!local_token2Type.empty() && !local_token2ArrLen.empty())
            add_var(cs->u.varDeclStmt, local_token2Type.back(), local_token2ArrLen.back());
    }
        break;
    case A_codeBlockStmtType::A_assignStmtKind:
        check_AssignStmt(out, cs->u.assignStmt);
        break;
    case A_codeBlockStmtType::A_ifStmtKind:
        check_IfStmt(out, cs->u.ifStmt);
        break;
    case A_codeBlockStmtType::A_whileStmtKind:
        check_WhileStmt(out, cs->u.whileStmt);
        break;
    case A_codeBlockStmtType::A_callStmtKind:
        check_CallStmt(out, cs->u.callStmt);
        break;
    case A_codeBlockStmtType::A_returnStmtKind:
        check_ReturnStmt(out, cs->u.returnStmt);
        break;
    default:
        break;
    }
    return;
}

/* ============  Added  ============= */
bool check_Defined(std::ostream& out, string id){
    // std::cout << "Checking " << id << std::endl;
    if(funcparam_token2Type.find(id) != funcparam_token2Type.end())
        return true;
    if(g_token2Type.find(id) != g_token2Type.end())
        return true;
    for(int i = 0;i < local_token2Type.size();i++){
        if(local_token2Type[i]->find(id) != local_token2Type[i]->end())
            return true;
    }
    return false;
}
/* ============   End   ============= */

void check_AssignStmt(std::ostream& out, aA_assignStmt as){
    if(!as)
        return;
    string name;
    tc_type deduced_type; // deduced type if type is omitted at decl
    switch (as->leftVal->kind)
    {//左值是否有定义？左值右值是否类型相同？
        case A_leftValType::A_varValKind:{
            name = *as->leftVal->u.id;
            /* fill code here */
            /* ============  Added  ============= */  
            if(!check_Defined(out, name))
                error_print(out, as->leftVal->pos, "Var '" + name + "' is not defined!");
            deduced_type = pinpoint(name);
            if(deduced_type->isVarArrFunc == 2)
                error_print(out, as->leftVal->pos, "Cannot assign a value to function '" + name + "'.");
            tc_type type = val_of_rightVal(out, as->rightVal);
            if(!comp_tc_type(deduced_type, type))
                error_print(out, as->leftVal->pos, "Right-value's type inconsistent with desired type.");
            /* ============   End   ============= */
        }
            break;
        case A_leftValType::A_arrValKind:{
            name = *as->leftVal->u.arrExpr->arr->u.id;
            /* fill code here */
            /* ============  Added  ============= */
            check_ArrayExpr(out, as->leftVal->u.arrExpr);
            deduced_type = pinpoint(name);
            tc_type type = val_of_rightVal(out, as->rightVal);
            if(!comp_tc_type(deduced_type, type))
                error_print(out, as->leftVal->pos, "Right-value's type inconsistent with desired type.");
            /* ============   End   ============= */
        }
            break;
        case A_leftValType::A_memberValKind:{
            /* fill code here */
            /* ============  Added  ============= */
            deduced_type = check_MemberExpr(out, as->leftVal->u.memberExpr);
            tc_type type = val_of_rightVal(out, as->rightVal);
            if(!comp_tc_type(deduced_type, type))
                error_print(out, as->leftVal->pos, "Right-value's type inconsistent with desired type.");
            /* ============   End   ============= */
        }
            break;
    }
    return;
}


void check_ArrayExpr(std::ostream& out, aA_arrayExpr ae){
    if(!ae)
        return;
    string name = *ae->arr->u.id;
    /* ============  Added  ============= */
    /* fill code here */
    // check array name 是否已定义
    if(!check_Defined(out, name) || pinpoint(name)->isVarArrFunc != 1)
        error_print(out, ae->pos, name + "is not an array.");

    int length  = get_len(name);
    // check index
    switch(ae->idx->kind){
        case A_indexExprKind::A_idIndexKind:{
            string id = *ae->idx->u.id;
            if(!check_Defined(out, id))
                error_print(out, ae->pos, "Var '"+ id + "' is not defined!");
        }
            break;
        case A_indexExprKind::A_numIndexKind:{
            if(ae->idx->u.num < 0 || ae->idx->u.num >= length)
                error_print(out, ae->idx->pos, "Num in array must > 0 and < length of array.");
        }
            break;
        default:
            break;
    }

    /* ============   End   ============= */

    return;
}

 
tc_type check_MemberExpr(std::ostream& out, aA_memberExpr me){
    // check if the member exists and return the type of the member
    if(!me)
        return nullptr;
    string name   = *me->structId->u.id;
    /* fill code here */
    /* ============  Added  ============= */
    string member = *me->memberId;
    
    if(!check_Defined(out, name) || pinpoint(name)->type->type != A_dataType::A_structTypeKind)
        error_print(out, me->pos, "'" + name + "' is not a struct.");
    
    string structname = *pinpoint(name)->type->u.structType;
    
    // check struct name
    if(struct2Members.find(structname) == struct2Members.end())
        error_print(out, me->pos, "'" +  structname + "' is not defined.");
    else{
    // check member name
        
        vector<aA_varDecl> v = (*struct2Members[structname]);
        // print_token_maps();
        if(v.size() == 0)
            error_print(out, me->pos,  "Member '" +  member + "' is not defined!");
        
        string tmp_id;
        for(aA_varDecl vd : v){
            switch(vd->kind){
                case A_varDeclType::A_varDeclScalarKind:{
                    aA_varDeclScalar vds = vd->u.declScalar;
                    tmp_id = *vds->id;
                    if(tmp_id == member)  return tc_Type(vds->type, 0);
                }
                    break;
                case A_varDeclType::A_varDeclArrayKind:{
                    aA_varDeclArray vda = vd->u.declArray;
                    tmp_id = *vda->id;
                    if(tmp_id == member) return tc_Type(vda->type, 1);
                }
                    break;
                default:
                    break;
            }
        }
        error_print(out, me->pos, "Member '" +  member + "' is not defined!");
    }
    /* ============   End   ============= */
        
    return nullptr;
}


void check_IfStmt(std::ostream& out, aA_ifStmt is){
    if(!is)
        return;
    check_BoolExpr(out, is->boolExpr);
    /* fill code here, take care of variable scope */
    /* ============  Added  ============= */  
    typeMap* tmp = new typeMap;
    local_token2Type.push_back(tmp);

    for(aA_codeBlockStmt s : is->ifStmts){
        check_CodeblockStmt(out, s);
    }
    if(!local_token2Type.empty())
        local_token2Type.pop_back();

    delete tmp; tmp = nullptr;

    typeMap* tmp1 = new typeMap;
    /* fill code here */ 
    local_token2Type.push_back(tmp1);
    for(aA_codeBlockStmt s : is->elseStmts){
        check_CodeblockStmt(out, s);
    }
    if(!local_token2Type.empty())
        local_token2Type.pop_back();

    delete tmp1;tmp1 = nullptr;
    /* fill code here */
    /* ============   End   ============= */
    return;
}


void check_BoolExpr(std::ostream& out, aA_boolExpr be){
    if(!be)
        return;
    switch (be->kind)
    {
    case A_boolExprType::A_boolBiOpExprKind:
        check_BoolExpr(out, be->u.boolBiOpExpr->left);
        check_BoolExpr(out, be->u.boolBiOpExpr->right);
        break;
    case A_boolExprType::A_boolUnitKind:
        check_BoolUnit(out, be->u.boolUnit);
        break;
    default:
        break;
    }
    return;
}


void check_BoolUnit(std::ostream& out, aA_boolUnit bu){
    if(!bu)
        return;
    switch (bu->kind)
    {
        case A_boolUnitType::A_comOpExprKind:{
            /* fill code here */
            /* ============  Added  ============= */
            aA_exprUnit left = bu->u.comExpr->left;
            aA_exprUnit right = bu->u.comExpr->right;
            tc_type l = check_ExprUnit(out, left), r = check_ExprUnit(out, right);
            if(!comp_tc_type(l, r))
                error_print(out, bu->pos, "'" + get_return_type(l) + "' var '" +  *left->u.id + 
                    "' is not comparable with '" + get_return_type(r) + "' var '" + *right->u.id + "'.");
            /* ============   End   ============= */
        }
            break;
        case A_boolUnitType::A_boolExprKind:
            check_BoolExpr(out, bu->u.boolExpr);
            break;
        case A_boolUnitType::A_boolUOpExprKind:
            check_BoolUnit(out, bu->u.boolUOpExpr->cond);
            break;
        default:
            break;
    }
    return;
}
/* ============  Added  ============= */

tc_type pinpoint(string s){
    tc_type ret;
    if(funcparam_token2Type.find(s) != funcparam_token2Type.end())
        ret = funcparam_token2Type[s];
    else if(g_token2Type.find(s) != g_token2Type.end())
        ret = g_token2Type[s];
    else{
        for(int i = 0;i < local_token2Type.size();i++){
            if(local_token2Type[i]->find(s) != local_token2Type[i]->end())
                ret = local_token2Type[i]->at(s);
        }        
    }
    return ret;
}



int get_len(string s){
    int length;
    if(funcparam_token2Arrlen.find(s) != funcparam_token2Arrlen.end())
        length = funcparam_token2Arrlen[s];
    if(g_token2ArrLen.find(s) != g_token2ArrLen.end())
        length = g_token2ArrLen[s];
    for(int i = 0;i < local_token2ArrLen.size();i++){
        if(local_token2ArrLen[i]->find(s) != local_token2ArrLen[i]->end())
            length = local_token2ArrLen[i]->at(s);
    }
    return length;
}

/* ============   End   ============= */


tc_type check_ExprUnit(std::ostream& out, aA_exprUnit eu){
    // return the aA_type of expr eu
    if(!eu)
        return nullptr;
    tc_type ret;
    switch (eu->kind)
    {
        case A_exprUnitType::A_idExprKind:{
            /* fill code here */
            /* ============  Added  ============= */
            string id = *eu->u.id;
            if(!check_Defined(out, id))
                error_print(out, eu->pos, "Var '"+ id + "' is not defined!");
            ret = pinpoint(id);
            /* ============   End   ============= */
        }
            break;
        case A_exprUnitType::A_numExprKind:{
            aA_type numt = new aA_type_;
            numt->pos = eu->pos;
            numt->type = A_dataType::A_nativeTypeKind;
            numt->u.nativeType = A_nativeType::A_intTypeKind;
            ret = tc_Type(numt, 0);
        }
            break;
        case A_exprUnitType::A_fnCallKind:{
            check_FuncCall(out, eu->u.callExpr);
            // check_FuncCall will check if the function is defined
            ret = pinpoint(*eu->u.callExpr->fn);
            /* fill code here */
        }
            break;
        case A_exprUnitType::A_arrayExprKind:{
            check_ArrayExpr(out, eu->u.arrayExpr);
            /* fill code here */
            ret = pinpoint(*eu->u.arrayExpr->arr->u.id);
        }
            break;
        case A_exprUnitType::A_memberExprKind:{
            ret = check_MemberExpr(out, eu->u.memberExpr);
        }
            break;
        case A_exprUnitType::A_arithExprKind:{
            ret = check_ArithExpr(out, eu->u.arithExpr);
        }
            break;
        case A_exprUnitType::A_arithUExprKind:{
            ret = check_ExprUnit(out, eu->u.arithUExpr->expr);
        }
            break;
    }
    return ret;
}


tc_type check_ArithExpr(std::ostream& out, aA_arithExpr ae){
    if(!ae)
        return nullptr;
    tc_type ret;
    switch (ae->kind)
    {
        case A_arithExprType::A_arithBiOpExprKind:{
            // std::cout<< "arithexpr of arithBiOpExpr" << std::endl;
            ret = check_ArithExpr(out, ae->u.arithBiOpExpr->left);
            tc_type rightType = check_ArithExpr(out, ae->u.arithBiOpExpr->right);
            if(ret->type->type > 0 || ret->type->type != A_dataType::A_nativeTypeKind || ret->type->u.nativeType != A_nativeType::A_intTypeKind ||
            rightType->type->type > 0 || rightType->type->type != A_dataType::A_nativeTypeKind || rightType->type->u.nativeType != A_nativeType::A_intTypeKind)
                error_print(out, ae->pos, "Only int can be arithmetic expression operation values!");
        }
            break;
        case A_arithExprType::A_exprUnitKind:{
            // std::cout<< "arithexpr of exprunit" << std::endl;
            ret = check_ExprUnit(out, ae->u.exprUnit);
        }
            break;
    }
    return ret;
}


void check_FuncCall(std::ostream& out, aA_fnCall fc){
    if(!fc)
        return;
    // check if function defined
    string func_name = *fc->fn;
    /* fill code here */
    /* ============  Added  ============= */
    if(!check_Defined(out, func_name) || pinpoint(func_name)->isVarArrFunc != 2)
        error_print(out, fc->pos, "Function name '" + func_name + "' doesn't exist.");        

    // check if parameter list matches
    vector<aA_rightVal> vals = fc->vals;
    vector<aA_varDecl>  vars = *(func2Param[func_name]);

    tc_type tmp;
    if(vals.size() != vars.size())
        error_print(out, fc->pos, "Param number inconsistent with function '" + func_name + "'.");
    for(int i = 0;i < vals.size();i++){
        switch(vars[i]->kind){
            case A_varDeclType::A_varDeclScalarKind:{
                tmp = tc_Type(vars[i]->u.declScalar->type, 0);
            }
                break;
            case A_varDeclType::A_varDeclArrayKind:{
                tmp = tc_Type(vars[i]->u.declArray->type, 1);
            }
                break;
            default:
                break;
        }
        if(!comp_tc_type(tmp, val_of_rightVal(out, vals[i])))
            error_print(out, fc->pos, "Param mismatch between definition and call.");
    }
    /* ============   End   ============= */    

    return ;
}


void check_WhileStmt(std::ostream& out, aA_whileStmt ws){
    if(!ws)
        return;
    check_BoolExpr(out, ws->boolExpr);
    /* fill code here, take care of variable scope */
    typeMap* tmp = new typeMap;
    local_token2Type.push_back(tmp);
    for(aA_codeBlockStmt s : ws->whileStmts){
        check_CodeblockStmt(out, s);
    }
    /* fill code here */
    if(!local_token2Type.empty())
        local_token2Type.pop_back();

    delete tmp; tmp = nullptr;
    return;
}


void check_CallStmt(std::ostream& out, aA_callStmt cs){
    if(!cs)
        return;
    check_FuncCall(out, cs->fnCall);
    return;
}


void check_ReturnStmt(std::ostream& out, aA_returnStmt rs){
    if(!rs)
        return;
    return;
}

