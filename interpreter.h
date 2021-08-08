#pragma once

#ifndef INTERPRETER_HEADER
#define INTERPRETER_HEADER

#include <stdexcept>
using namespace std;

////LEXER PART////

struct Token
{
    //value is diffrent than -1 only when the token is for integer
    int value;

    enum type
    {
        //reserved keywords
        LABEL,
        GOTO,
        LET,
        READ,
        PRINT,
        IF,
        ENDIF,
        ELSE,
        WHILE,
        DONE,

        //extra types that are needed
        SQ_LPAREN,
        SQ_RPAREN,

        ID,
        ASSIGN,
        NEWLINE,

        //bool types
        EQ,
        NEQ,
        LESS,
        LESSEQ,
        MORE,
        MOREEQ,
        OR,
        AND,
        NOT,

        //arithmetic types
        INTEGER,
        PLUS,
        MINUS,
        MUL,
        DIV,
        MOD,
        LPAREN,
        RPAREN,
        END
    } t;

    //text_data contains text only when the token is ID
    string text_data;
};

unordered_map<string, Token> RESERVED_KEYWORDS =
    {{"LABEL", Token{-1, Token::LABEL}},
     {"GOTO", Token{-1, Token::GOTO}},
     {"LET", Token{-1, Token::LET}},
     {"READ", Token{-1, Token::READ}},
     {"PRINT", Token{-1, Token::PRINT}},
     {"IF", Token{-1, Token::IF}},
     {"ENDIF", Token{-1, Token::ENDIF}},
     {"ELSE", Token{-1, Token::ELSE}},
     {"WHILE", Token{-1, Token::WHILE}},
     {"DONE", Token{-1, Token::DONE}}};

class Lexer
{
private:
    string text;

    int pos;
    char current_char;

    void advance();
    char peek();

    //when the lexer finds a word it can be identifier or reserved keyword in the language so we check in a table:
    //pre-constructed token for that reserved keyword is returned, otherwise we make an identifier token
    Token _id();
    Token integer();

    void error();
    void skip_whitespace();

public:
    Lexer(const string &input);
    Token get_next_token();
};

////ABSTRACT SYNTAX TREE////
class Visitor;

class AST_Node
{
public:
    virtual void accept(Visitor &v) = 0;
};

class BlockCode : public AST_Node
{
public:
    vector<AST_Node *> statements;

    BlockCode(vector<AST_Node *> lines) : statements(lines){};

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

//BASE CLASS VISITOR
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
    unordered_set<string> labels;

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

class SymbolTable
{
public:
    void dec_var(string identifier);
    void dec_arr(string identifier, int arr_size);

    void modify_var(string identifier, int newvalue);
    void modify_arr(string identifier, int index, int newvalue);

    const int &lookup_var(string varname) const;
    const int &lookup_arr(string arr_name, int index) const;

    bool var_exists(string varname) const;
    bool arr_exists(string arrname) const;

private:
    unordered_map<string, int> vars;
    unordered_map<string, vector<int>> arrays;
};

class ScopedTable
{
public:
    void dec_var(string identifier);
    void dec_arr(string identifier, int arr_size);
    void modify_var(string identifier, int newvalue);
    void modify_arr(string identifier, int index, int newvalue);
    const int &lookup_var(string varname) const;
    const int &lookup_arr(string arr_name, int index) const;
    void addScope();
    void removeScope();
    void back_to_global();
    ScopedTable();

private:
    int top;
    vector<SymbolTable> scopes;
};

class Interpreter : public Visitor
{
private:
    Deleter deleter;
    DataExtractor extractor;

    AST_Node *tree;

    bool skip_mode;

    int value; //used to evaluate expressions

    unordered_set<string> labels;
    string looking_for_label;

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
