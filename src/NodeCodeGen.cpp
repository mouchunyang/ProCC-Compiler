#include "Node.h"
#include "parser.hpp"

void NBlock::CodeGen(CodeContext& context)
{
    for (NStatement* stmt : mStatements){
        stmt->CodeGen(context);
    }
}

void NData::CodeGen(CodeContext& context)
{
    for (NDecl* decl : mDecls){
        decl->CodeGen(context);
    }
}

void NProgram::CodeGen(CodeContext& context)
{
    mData->CodeGen(context);
    mMain->CodeGen(context);
    context.AddOp("exit", {});
}

void NNumeric::CodeGen(CodeContext& context)
{
}

void NVarDecl::CodeGen(CodeContext& context)
{
    context.AddOp("push", {"r0"});
    context.AddStack(mName, 1);
    
}

void NArrayDecl::CodeGen(CodeContext& context)
{
    for (int i = 0; i < mSize->GetValue(); ++i) {
        context.AddOp("push", {"r0"});
    }
    context.AddStack(mName, mSize->GetValue());
}

void NNumericExpr::CodeGen(CodeContext& context)
{
    mResultRegister = context.GetReg();
    context.AddOp("movi", {GetResultRegister(), std::to_string(mNumeric->GetValue())});
}

void NVarExpr::CodeGen(CodeContext& context)
{
    mResultRegister = context.GetReg();
    context.AddOp("loadi", {GetResultRegister(), std::to_string(context.stackIndexMap[mName])});
}

void NBinaryExpr::CodeGen(CodeContext& context)
{
    mLhs->CodeGen(context);
    mRhs->CodeGen(context);
    
    mResultRegister = context.GetReg();
    std::string opName;
    switch (mType) {
        case TADD:
            opName = "add";
            break;
        case TSUB:
            opName = "sub";
            break;
        case TMUL:
            opName = "mul";
            break;
        case TDIV:
            opName = "div";
            break;
        default:
            break;
    }
    context.AddOp(opName, {GetResultRegister(), mLhs->GetResultRegister(), mRhs->GetResultRegister()});
}

void NArrayExpr::CodeGen(CodeContext& context)
{
    mSubscript->CodeGen(context);
    std::string reg1 = context.GetReg();
    context.AddOp("movi", {reg1, std::to_string(context.stackIndexMap[mName])});
    std::string reg2 = context.GetReg();
    context.AddOp("add", {reg2, reg1, mSubscript->GetResultRegister()});
    mResultRegister = context.GetReg();
    context.AddOp("load", {GetResultRegister(), reg2});
}

void NAssignVarStmt::CodeGen(CodeContext& context)
{
    // x = expr
    // loadi (int, reg) stack[int] = reg1
    mRhs->CodeGen(context);
    context.AddOp("storei", {std::to_string(context.stackIndexMap[mName]), mRhs->GetResultRegister()});
}

void NAssignArrayStmt::CodeGen(CodeContext& context)
{
    mRhs->CodeGen(context);
    mSubscript->CodeGen(context);
    
    std::string reg1 = context.GetReg();
    context.AddOp("movi", {reg1, std::to_string(context.stackIndexMap[mName])});
    std::string reg2 = context.GetReg();
    context.AddOp("add", {reg2, reg1, mSubscript->GetResultRegister()});
    context.AddOp("store", {reg2, mRhs->GetResultRegister()});
}

void NIncStmt::CodeGen(CodeContext& context)
{
    std::string reg1 = context.GetReg();
    context.AddOp("loadi", {reg1, std::to_string(context.stackIndexMap[mName])});
    context.AddOp("inc", {reg1});
    context.AddOp("storei", {std::to_string(context.stackIndexMap[mName]), reg1});
}

void NDecStmt::CodeGen(CodeContext& context)
{
    std::string reg1 = context.GetReg();
    context.AddOp("loadi", {reg1, std::to_string(context.stackIndexMap[mName])});
    context.AddOp("dec", {reg1});
    context.AddOp("storei", {std::to_string(context.stackIndexMap[mName]), reg1});
}

void NComparison::CodeGen(CodeContext& context)
{
    mLhs->CodeGen(context);
    mRhs->CodeGen(context);
    std::string opName;
    switch (mType) {
        case TISEQUAL:
            opName = "cmpeq";
            break;
        case TLESS:
            opName = "cmplt";
            break;
        default:
            break;
    }
    context.AddOp(opName, {mLhs->GetResultRegister(), mRhs->GetResultRegister()});
}

void NIfStmt::CodeGen(CodeContext& context)
{
    mComp->CodeGen(context);
    if (mElseBlock == nullptr){
        //if without else statement
        int moveIdx = context.params.size();
        std::string reg1 = context.GetReg();
        context.AddOp("movi", {reg1});
        context.AddOp("jnt", {reg1});
        mIfBlock->CodeGen(context);
        std::string ifEndIdx = std::to_string(context.params.size());
        context.params[moveIdx].emplace_back(ifEndIdx);
    }
    else{
        int moveIdx = context.params.size();
        std::string reg1 = context.GetReg();
        context.AddOp("movi", {reg1});
        context.AddOp("jnt", {reg1});
        mIfBlock->CodeGen(context);
        int ifEndIdx = context.params.size();
        std::string reg2 = context.GetReg();
        context.AddOp("movi", {reg2});
        context.AddOp("jmp", {reg2});
        std::string elseStartIdx = std::to_string(context.params.size());
        context.params[moveIdx].emplace_back(elseStartIdx);
        mElseBlock->CodeGen(context);
        std::string elseEndIdx = std::to_string(context.params.size());
        context.params[ifEndIdx].emplace_back(elseEndIdx);
    }
}

void NWhileStmt::CodeGen(CodeContext& context)
{
    int beginIdx = context.params.size();
    mComp->CodeGen(context);
    int whileIdx = context.params.size(); //address2
    std::string reg1 = context.GetReg();
    context.AddOp("movi", {reg1});
    context.AddOp("jnt", {reg1});
    mBlock->CodeGen(context);
    std::string reg2 = context.GetReg();
    context.AddOp("movi", {reg2, std::to_string(beginIdx)});
    context.AddOp("jmp", {reg2});
    std::string outBlockPos = std::to_string(context.params.size());
    context.params[whileIdx].emplace_back(outBlockPos);
}

void NPenUpStmt::CodeGen(CodeContext& context)
{
    context.AddOp("penup", {});
}

void NPenDownStmt::CodeGen(CodeContext& context)
{
    context.AddOp("pendown", {});
}

void NSetPosStmt::CodeGen(CodeContext& context)
{
    mXExpr->CodeGen(context);
    mYExpr->CodeGen(context);
    
    context.AddOp("mov", {"tx", mXExpr->GetResultRegister()});
    context.AddOp("mov", {"ty", mYExpr->GetResultRegister()});
}

void NSetColorStmt::CodeGen(CodeContext& context)
{
    mColor->CodeGen(context);
    context.AddOp("mov", {"tc", mColor->GetResultRegister()});
}

void NFwdStmt::CodeGen(CodeContext& context)
{
    mParam->CodeGen(context);
    context.AddOp("fwd", {mParam->GetResultRegister()});
}

void NBackStmt::CodeGen(CodeContext& context)
{
    mParam->CodeGen(context);
    context.AddOp("back", {mParam->GetResultRegister()});
}

void NRotStmt::CodeGen(CodeContext& context)
{
    mParam->CodeGen(context);
    context.AddOp("add", {"tr", "tr", mParam->GetResultRegister()});
}
