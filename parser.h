#pragma once

#ifndef PARSER_HEADER
#define PARSER_HEADER

//DELETE TREE
class Deleter : public Visitor
{
    void visit(Bin_OP *ast);
    void visit(Un_OP *ast);
    void visit(Var *ast);
    void visit(Array *ast);
    void visit(Print *ast);
    void visit(ReadVar *ast);
    void visit(ReadArr *ast);
    void visit(Num *ast);
    void visit(BlockCode *ast);
    void visit(IfElse *ast);
    void visit(While *ast);
    void visit(GoTo *ast);
    void visit(Label *ast);
    void visit(VarAssign *ast);
    void visit(ArrAssign *ast);
    void visit(VarDecl *ast);
    void visit(ArrDecl *ast);
    void visit(NO_OP *ast);
};

////PARSER////
class Parser
{
private:
    Deleter deleter; //when error occurs we delete the ast with this

    Lexer lexer;
    Token current_token;
    void error();
    void eat(Token::type input_type);

public:
    Parser(Lexer &_lexer);

    AST_Node *Program_Lines();
    AST_Node *Statement();
    AST_Node *Expression();
    AST_Node *AND_Exp();
    AST_Node *NOT_Exp();
    AST_Node *COMPARE_Exp();
    AST_Node *ADD_Exp();
    AST_Node *MULT_Exp();
    AST_Node *NEGATE_Exp();
    AST_Node *Value();
    AST_Node *parse();
};

#include "parser.inl"

#endif