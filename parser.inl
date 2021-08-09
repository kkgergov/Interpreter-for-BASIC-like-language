#ifndef PARSER_SOURCE
#define PARSER_SOURCE

inline Parser::Parser(Lexer &_lexer) : lexer(_lexer)
{
    current_token = lexer.get_next_token();
};

inline void Parser::error()
{
    //cout << "parser error";
    throw std::invalid_argument("There is something wrong with the structure of the code");
}

inline void Parser::eat(Token::type input_type)
{
    if (current_token.t == input_type)
    {
        current_token = lexer.get_next_token();
    }
    else
        error();
}

inline AST_Node *Parser::Program_Lines()
{
    AST_Node *node;

    try
    {
        std::vector<AST_Node *> statements;
        statements.push_back(Statement());

        Token token = current_token;
        while (token.t != Token::END)
        {
            eat(Token::NEWLINE);
            statements.push_back(Statement());
            token = current_token;
        }

        node = new BlockCode(statements);
    }
    catch (...)
    {
        node->accept(deleter);
        throw std::invalid_argument("something went wrong");
    }

    return node;
}

inline AST_Node *Parser::Statement()
{

    AST_Node *node;

    try
    {

        Token token = current_token;

        if (token.t == Token::LET)
        {
            eat(Token::LET);
            token = current_token;

            if (token.t == Token::ID)
            {
                Token id = token;
                eat(Token::ID);

                token = current_token;

                if (token.t == Token::SQ_LPAREN)
                {

                    eat(Token::SQ_LPAREN);

                    node = new ArrDecl(new Array(id, Expression()));

                    eat(Token::SQ_RPAREN);
                }
                else
                {
                    node = new VarDecl(new Var(id));
                }
            }
        }
        else if (token.t == Token::ID)
        {
            eat(Token::ID);

            Token id = token;

            token = current_token;
            if (token.t == Token::ASSIGN)
            {
                eat(Token::ASSIGN);

                node = new VarAssign(new Var(id), Expression());
            }
            else if (token.t == Token::SQ_LPAREN)
            {
                eat(Token::SQ_LPAREN);
                AST_Node *_index = Expression();
                eat(Token::SQ_RPAREN);

                eat(Token::ASSIGN);

                node = new ArrAssign(new Array(id, _index), Expression());
            }
        }
        else if (token.t == Token::PRINT)
        {
            eat(Token::PRINT);

            node = new Print(Expression());
        }
        else if (token.t == Token::READ)
        {
            eat(Token::READ);
            token = current_token;
            Token id = token;
            eat(Token::ID);

            token = current_token;

            if (token.t == Token::SQ_LPAREN)
            {

                eat(Token::SQ_LPAREN);

                node = new ReadArr(new Array(id, Expression()));

                eat(Token::SQ_RPAREN);
            }
            else
            {
                node = new ReadVar(new Var(id));
            }
        }
        else if (token.t == Token::IF)
        {
            eat(Token::IF);

            AST_Node *expr = Expression();

            std::vector<AST_Node *> if_statements;
            std::vector<AST_Node *> else_statements;

            if_statements.push_back(Statement());
            token = current_token;
            while (token.t != Token::ELSE && token.t != Token::ENDIF)
            {
                eat(Token::NEWLINE);
                if_statements.push_back(Statement());
                token = current_token;
            }

            AST_Node *ifBlock = new BlockCode(if_statements);

            if (token.t == Token::ELSE)
            {
                eat(Token::ELSE);

                else_statements.push_back(Statement());
                token = current_token;
                while (token.t != Token::ENDIF)
                {
                    eat(Token::NEWLINE);
                    else_statements.push_back(Statement());
                    token = current_token;
                }
            }

            AST_Node *elseBlock = new BlockCode(else_statements);

            eat(Token::ENDIF);

            node = new IfElse(expr, ifBlock, elseBlock);
        }
        else if (token.t == Token::WHILE)
        {
            eat(Token::WHILE);

            AST_Node *expr = Expression();
            std::vector<AST_Node *> block_statements;

            block_statements.push_back(Statement());
            token = current_token;
            while (token.t != Token::DONE)
            {
                eat(Token::NEWLINE);
                block_statements.push_back(Statement());
                token = current_token;
            }

            eat(Token::DONE);

            node = new While(expr, new BlockCode(block_statements));
        }
        else if (token.t == Token::GOTO)
        {
            eat(Token::GOTO);

            token = current_token;
            eat(Token::ID);

            node = new GoTo(token);

            //cout << "created goto node with value" << token.text_data << "--parser\n";
        }
        else if (token.t == Token::LABEL)
        {

            eat(Token::LABEL);

            token = current_token;
            eat(Token::ID);

            node = new Label(token);
        }
        else
        {
            node = new NO_OP();
        }
    }
    catch (...)
    {
        node->accept(deleter);
        throw std::invalid_argument("something went wrong");
    }

    return node;
}

inline AST_Node *Parser::Expression()
{
    AST_Node *node;

    try
    {
        node = AND_Exp();
        Token token = current_token;

        if (token.t == Token::OR)
        {
            eat(Token::OR);
            node = new Bin_OP(token, node, Expression());
        }
    }
    catch (...)
    {
        node->accept(deleter);
        throw std::invalid_argument("something went wrong");
    }

    return node;
};

inline AST_Node *Parser::AND_Exp()
{
    AST_Node *node;

    try
    {
        node = NOT_Exp();
        Token token = current_token;

        if (token.t == Token::AND)
        {
            eat(Token::AND);
            node = new Bin_OP(token, node, AND_Exp());
        }
    }
    catch (...)
    {
        node->accept(deleter);
        throw std::invalid_argument("something went wrong");
    }

    return node;
};

inline AST_Node *Parser::NOT_Exp()
{
    AST_Node *node;

    try
    {
        Token token = current_token;
        if (token.t == Token::NOT)
        {
            eat(Token::NOT);
            node = new Un_OP(token, COMPARE_Exp());
        }
        else
            node = COMPARE_Exp();
    }
    catch (...)
    {
        node->accept(deleter);
        throw std::invalid_argument("something went wrong");
    }

    return node;
};

inline AST_Node *Parser::COMPARE_Exp()
{
    AST_Node *node;

    try
    {
        node = ADD_Exp();
        Token token = current_token;

        switch (token.t)
        {
        case Token::EQ:
            eat(Token::EQ);
            node = new Bin_OP(token, node, COMPARE_Exp());
            break;
        case Token::NEQ:
            eat(Token::NEQ);
            node = new Bin_OP(token, node, COMPARE_Exp());
            break;
        case Token::LESS:
            eat(Token::LESS);
            node = new Bin_OP(token, node, COMPARE_Exp());
            break;
        case Token::LESSEQ:
            eat(Token::LESSEQ);
            node = new Bin_OP(token, node, COMPARE_Exp());
            break;
        case Token::MORE:
            eat(Token::MORE);
            node = new Bin_OP(token, node, COMPARE_Exp());
            break;
        case Token::MOREEQ:
            eat(Token::MOREEQ);
            node = new Bin_OP(token, node, COMPARE_Exp());
            break;
        default:
            break;
        }
    }
    catch (...)
    {
        node->accept(deleter);
        throw std::invalid_argument("something went wrong");
    }

    return node;
};

inline AST_Node *Parser::ADD_Exp()
{
    AST_Node *node;

    try
    {
        node = MULT_Exp();
        Token token = current_token;

        switch (token.t)
        {
        case Token::PLUS:
            eat(Token::PLUS);
            node = new Bin_OP(token, node, ADD_Exp());
            break;
        case Token::MINUS:
            eat(Token::MINUS);
            node = new Bin_OP(token, node, ADD_Exp());
            break;
        default:
            break;
        }
    }
    catch (...)
    {
        node->accept(deleter);
        throw std::invalid_argument("something went wrong");
    }

    return node;
};

inline AST_Node *Parser::MULT_Exp()
{
    AST_Node *node;

    try
    {
        node = NEGATE_Exp();
        Token token = current_token;

        switch (token.t)
        {
        case Token::MUL:
            eat(Token::MUL);
            node = new Bin_OP(token, node, MULT_Exp());
            break;
        case Token::DIV:
            eat(Token::DIV);
            node = new Bin_OP(token, node, MULT_Exp());
            break;
        case Token::MOD:
            eat(Token::MOD);
            node = new Bin_OP(token, node, MULT_Exp());
            break;
        default:
            break;
        }
    }
    catch (...)
    {
        node->accept(deleter);
        throw std::invalid_argument("something went wrong");
    }

    return node;
};

inline AST_Node *Parser::NEGATE_Exp()
{
    AST_Node *node;

    try
    {
        Token token = current_token;

        if (token.t == Token::MINUS)
        {
            eat(Token::MINUS);
            node = new Un_OP(token, Value());
        }
        else
            node = Value();
    }
    catch (...)
    {
        node->accept(deleter);
        throw std::invalid_argument("something went wrong");
    }

    return node;
};

inline AST_Node *Parser::Value()
{
    AST_Node *node;

    try
    {
        Token token = current_token;

        //cout << "got here \n";
        switch (token.t)
        {

        case Token::LPAREN:
            eat(Token::LPAREN);
            node = Expression();
            eat(Token::RPAREN);
            break;
        case Token::ID:
        {
            Token name = token;

            eat(Token::ID);

            token = current_token;
            if (token.t == Token::SQ_LPAREN)
            {
                //cout << "got here";
                eat(Token::SQ_LPAREN);
                node = new Array(name, Expression());
                eat(Token::SQ_RPAREN);
            }
            else
                node = new Var(name);
            break;
        }
        case Token::INTEGER:
            eat(Token::INTEGER);
            node = new Num(token);
            break;
        default:
            //cout << "oops";
            break;
        }
    }
    catch (...)
    {
        node->accept(deleter);
        throw std::invalid_argument("something went wrong");
    }

    return node;
};

inline AST_Node *Parser::parse()
{
    return Program_Lines();
}

/////DELETER
inline void Deleter::visit(Bin_OP *ast)
{
    if (ast->left)
        ast->left->accept(*this);
    if (ast->right)
        ast->right->accept(*this);

    delete ast;
}

inline void Deleter::visit(Un_OP *ast)
{
    if (ast->expr)
        ast->expr->accept(*this);

    delete ast;
}

inline void Deleter::visit(Var *ast)
{
    delete ast;
}

inline void Deleter::visit(Array *ast)
{
    if (ast->index)
        ast->index->accept(*this);
    delete ast;
}

inline void Deleter::visit(Print *ast)
{
    if (ast->expr_to_print)
        ast->expr_to_print->accept(*this);
    delete ast;
}

inline void Deleter::visit(ReadVar *ast)
{
    if (ast->var)
        ast->var->accept(*this);
    delete ast;
}

inline void Deleter::visit(ReadArr *ast)
{
    if (ast->arr)
        ast->arr->accept(*this);
    delete ast;
}

inline void Deleter::visit(Num *ast)
{
    delete ast;
}

inline void Deleter::visit(BlockCode *ast)
{

    for (int i = 0; i < ast->statements.size(); ++i)
    {
        if (ast->statements[i])
        {
            ast->statements[i]->accept(*this);
        }
    }

    delete ast;
}

inline void Deleter::visit(IfElse *ast)
{
    if (ast->expr)
        ast->expr->accept(*this);
    if (ast->bCode1)
        ast->bCode1->accept(*this);
    if (ast->bCode2)
        ast->bCode2->accept(*this);

    delete ast;
}

inline void Deleter::visit(While *ast)
{
    if (ast->expr)
        ast->expr->accept(*this);
    if (ast->bCode)
        ast->bCode->accept(*this);

    delete ast;
}

inline void Deleter::visit(GoTo *ast)
{
    delete ast;
}

inline void Deleter::visit(Label *ast)
{
    delete ast;
}

inline void Deleter::visit(VarAssign *ast)
{
    if (ast->var)
        ast->var->accept(*this);
    if (ast->expr)
        ast->expr->accept(*this);

    delete ast;
}

inline void Deleter::visit(ArrAssign *ast)
{
    if (ast->arr)
        ast->arr->accept(*this);
    if (ast->expr)
        ast->expr->accept(*this);

    delete ast;
}

inline void Deleter::visit(VarDecl *ast)
{
    if (ast->var)
        ast->var->accept(*this);

    delete ast;
}

inline void Deleter::visit(ArrDecl *ast)
{
    if (ast->arr)
        ast->arr->accept(*this);

    delete ast;
}

inline void Deleter::visit(NO_OP *ast)
{
    delete ast;
}

#endif