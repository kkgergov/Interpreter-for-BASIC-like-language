#pragma once

#ifndef INTERPRETER_HEADER
#define INTERPRETER_HEADER

#include <stdexcept>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include "lexer.h"
#include "AST_Nodes.h"
#include "parser.h"
#include "ScopedTable.h"

////ABSTRACT SYNTAX TREE////

//DATA EXTRACT FROM NODE
class DataExtractor : public Visitor
{
public:
    Token type;
    AST_Node *helperNode;

    void visit(Bin_OP *ast);
    void visit(Un_OP *ast);
    void visit(Var *ast);
    void visit(Array *ast);
    void visit(GoTo *ast);
    void visit(Label *ast);
    void visit(Num *ast);
    void visit(BlockCode *ast);
    void visit(IfElse *ast);
    void visit(While *ast);
    void visit(VarAssign *ast);
    void visit(ArrAssign *ast);
    void visit(VarDecl *ast);
    void visit(ArrDecl *ast);
    void visit(Print *ast);
    void visit(ReadArr *ast);
    void visit(ReadVar *ast);
    void visit(NO_OP *ast);
};

class BeforeInterpret : public Visitor
{
public:
    std::unordered_set<std::string> labels;

    void visit(Var *ast);
    void visit(Array *ast);
    void visit(GoTo *ast);
    void visit(Label *ast);
    void visit(BlockCode *ast);
    void visit(IfElse *ast);
    void visit(While *ast);
    void visit(Bin_OP *ast);
    void visit(Un_OP *ast);
    void visit(Num *ast);
    void visit(VarAssign *ast);
    void visit(ArrAssign *ast);
    void visit(VarDecl *ast);
    void visit(ArrDecl *ast);
    void visit(Print *ast);
    void visit(ReadArr *ast);
    void visit(ReadVar *ast);
    void visit(NO_OP *ast);
};

class Interpreter : public Visitor
{
private:
    Deleter deleter;
    DataExtractor extractor;

    AST_Node *tree;

    bool skip_mode;

    int value; //used to evaluate expressions

    std::unordered_set<std::string> labels;
    std::string looking_for_label;

    ScopedTable nested_scopes;

public:
    Interpreter();

    Interpreter(AST_Node *t);

    void visit(GoTo *ast);
    void visit(Label *ast);
    void visit(BlockCode *ast);
    void visit(IfElse *ast);
    void visit(While *ast);
    void visit(VarDecl *ast);
    void visit(ArrDecl *ast);
    void visit(VarAssign *ast);
    void visit(ArrAssign *ast);
    void visit(ReadVar *ast);
    void visit(ReadArr *ast);
    void visit(Print *ast);
    void visit(Bin_OP *ast);
    void visit(Num *ast);
    void visit(Var *ast);
    void visit(Array *ast);
    void visit(Un_OP *ast);
    void visit(NO_OP *ast);

    void interpret_fullprogram();
    void interpret_REPL();
};

#include "interpreter.inl"

#endif 
