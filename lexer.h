#pragma once

#ifndef LEXER_HEADER
#define LEXER_HEADER

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
    std::string text_data;
};

std::unordered_map<std::string, Token> RESERVED_KEYWORDS =
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
    std::string text;

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
    Lexer(const std::string &input);
    Token get_next_token();
};

#include "lexer.inl"

#endif