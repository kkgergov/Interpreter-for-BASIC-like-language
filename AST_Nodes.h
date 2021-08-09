#pragma once

#ifndef AST_HEADER
#define AST_HEADER

class Visitor;

class AST_Node
{
public:
    virtual void accept(Visitor &v) = 0;
};

class BlockCode : public AST_Node
{
public:
    std::vector<AST_Node *> statements;

    BlockCode(std::vector<AST_Node *> lines) : statements(lines){};

    void accept(Visitor &v);
};

class IfElse : public AST_Node
{
public:
    AST_Node *expr;

    AST_Node *bCode1; //in the interpreter we are going to visit this if expr is true
    AST_Node *bCode2;

    IfElse(AST_Node *a, AST_Node *b, AST_Node *c) : expr(a), bCode1(b), bCode2(c){};

    void accept(Visitor &v);
};

class While : public AST_Node
{
public:
    AST_Node *expr;

    AST_Node *bCode;

    While(AST_Node *a, AST_Node *b) : expr(a), bCode(b){};

    void accept(Visitor &v);
};

class GoTo : public AST_Node
{
public:
    Token token;

    GoTo(Token t) : token(t){};

    void accept(Visitor &v);
};

class Label : public AST_Node
{
public:
    Token token;

    Label(Token t) : token(t){};

    void accept(Visitor &v);
};

class VarAssign : public AST_Node
{
public:
    AST_Node *var;
    AST_Node *expr;

    VarAssign(AST_Node *l, AST_Node *r) : var(l), expr(r){};

    void accept(Visitor &v);
};

class ArrAssign : public AST_Node
{
public:
    AST_Node *arr;
    AST_Node *expr;

    ArrAssign(AST_Node *l, AST_Node *r) : arr(l), expr(r){};

    void accept(Visitor &v);
};

class VarDecl : public AST_Node
{
public:
    AST_Node *var;

    VarDecl(AST_Node *m) : var(m){};

    void accept(Visitor &v);
};

class ArrDecl : public AST_Node
{
public:
    AST_Node *arr;

    ArrDecl(AST_Node *l) : arr(l){};

    void accept(Visitor &v);
};

class Print : public AST_Node
{
public:
    AST_Node *expr_to_print;
    Print(AST_Node *ast) : expr_to_print(ast){};

    void accept(Visitor &v);
};

class ReadVar : public AST_Node
{
public:
    AST_Node *var;
    ReadVar(AST_Node *ast) : var(ast){};

    void accept(Visitor &v);
};

class ReadArr : public AST_Node
{
public:
    AST_Node *arr;
    ReadArr(AST_Node *ast) : arr(ast){};

    void accept(Visitor &v);
};

//VALUES
class Num : public AST_Node
{
public:
    Token token;
    Num(Token t) : token(t){};

    void accept(Visitor &v);
};

class Var : public AST_Node
{
public:
    Token token;
    Var(Token t) : token(t){};

    void accept(Visitor &v);
};

class Array : public AST_Node
{
public:
    Token token; //same as var, the array is characterized by id
    AST_Node *index;
    Array(Token t, AST_Node *ast) : token(t), index(ast){};

    void accept(Visitor &v);
};

//EXPRESSIONS
class Bin_OP : public AST_Node
{
public:
    AST_Node *left, *right;
    Token op;
    Bin_OP(Token t, AST_Node *l, AST_Node *r) : left(l), right(r), op(t){};

    void accept(Visitor &v);
};

class Un_OP : public AST_Node
{
public:
    AST_Node *expr;
    Token op;
    Un_OP(Token t, AST_Node *e) : expr(e), op(t){};

    void accept(Visitor &v);
};

class NO_OP : public AST_Node
{
public:
    void accept(Visitor &v);
};

class Visitor
{
public:
    virtual void visit(Bin_OP *ast) = 0;
    virtual void visit(Num *ast) = 0;
    virtual void visit(Un_OP *ast) = 0;
    virtual void visit(Var *ast) = 0;
    virtual void visit(NO_OP *ast) = 0;
    virtual void visit(Array *ast) = 0;

    virtual void visit(Print *ast) = 0;
    virtual void visit(ReadVar *ast) = 0;
    virtual void visit(ReadArr *ast) = 0;

    virtual void visit(BlockCode *ast) = 0;
    virtual void visit(IfElse *ast) = 0;
    virtual void visit(While *ast) = 0;
    virtual void visit(GoTo *ast) = 0;
    virtual void visit(Label *ast) = 0;

    virtual void visit(VarAssign *ast) = 0;
    virtual void visit(ArrAssign *ast) = 0;
    virtual void visit(VarDecl *ast) = 0;
    virtual void visit(ArrDecl *ast) = 0;
};

#include "AST_Nodes.inl"

#endif