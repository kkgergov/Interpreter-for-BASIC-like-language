#ifndef INTERPRETER_SOURCE
#define INTERPRETER_SOURCE

////////LEXER//////////
inline Lexer::Lexer(const string &input) : text(input), pos(0)
{
    current_char = text[pos];
}

inline void Lexer::error()
{
    throw std::invalid_argument("This cannot be tokenized! \n");
}

inline void Lexer::advance()
{
    pos += 1;
    if (pos < text.length())
        current_char = text[pos];
    else
        current_char = '\0';
}

inline void Lexer::skip_whitespace()
{
    while (current_char && isspace(current_char) && current_char != '\n')
        advance();
}

inline char Lexer::peek()
{
    int peek_pos = pos + 1;

    if (peek_pos < text.length())
        return text[peek_pos];
    else
        return '\0';
}

inline Token Lexer::integer()
{
    int result = 0;

    while (current_char && isdigit(current_char))
    {
        result = result * 10 + (int)(current_char - '0');
        advance();
    }

    return Token{result, Token::INTEGER};
}

inline Token Lexer::_id()
{
    string result;

    while (current_char && isalpha(current_char))
    {
        result.push_back(current_char);
        advance();
    }

    unordered_map<string, Token>::const_iterator got = RESERVED_KEYWORDS.find(result);
    if (got != RESERVED_KEYWORDS.end())
        return got->second;

    else
        return Token{-1, Token::ID, result};
}

inline Token Lexer::get_next_token()
{
    while (current_char)
    {

        if (isspace(current_char))
        {
            skip_whitespace();
            if (!current_char)
                continue;
        }

        if (isalpha(current_char))
            return _id();

        if (isdigit(current_char))
            return integer();

        if (current_char == '=' && peek() == '=')
        {
            advance();
            advance();
            return Token{-1, Token::EQ};
        }

        if (current_char == '!' && peek() == '=')
        {
            advance();
            advance();
            return Token{-1, Token::NEQ};
        }

        if (current_char == '<' && peek() == '=')
        {
            advance();
            advance();
            return Token{-1, Token::LESSEQ};
        }

        if (current_char == '>' && peek() == '=')
        {
            advance();
            advance();
            return Token{-1, Token::MOREEQ};
        }

        if (current_char == '&' && peek() == '&')
        {
            advance();
            advance();
            return Token{-1, Token::AND};
        }

        if (current_char == '|' && peek() == '|')
        {
            advance();
            advance();
            return Token{-1, Token::OR};
        }

        if (current_char == '!')
        {
            advance();
            return Token{-1, Token::NOT};
        }

        if (current_char == '<')
        {
            advance();
            return Token{-1, Token::LESS};
        }

        if (current_char == '>')
        {
            advance();
            return Token{-1, Token::MORE};
        }

        if (current_char == '=')
        {
            advance();
            return Token{-1, Token::ASSIGN};
        }
        if (current_char == '\n')
        {
            advance();
            return Token{-1, Token::NEWLINE};
        }

        if (current_char == '+')
        {
            advance();
            return Token{-1, Token::PLUS};
        }

        if (current_char == '-')
        {
            advance();
            return Token{-1, Token::MINUS};
        }

        if (current_char == '*')
        {
            advance();
            return Token{-1, Token::MUL};
        }

        if (current_char == '/')
        {
            advance();
            return Token{-1, Token::DIV};
        }

        if (current_char == '%')
        {
            advance();
            return Token{-1, Token::MOD};
        }

        if (current_char == '(')
        {
            advance();
            return Token{-1, Token::LPAREN};
        }

        if (current_char == ')')
        {
            advance();
            return Token{-1, Token::RPAREN};
        }
        if (current_char == '[')
        {
            advance();
            return Token{-1, Token::SQ_LPAREN};
        }

        if (current_char == ']')
        {
            advance();
            return Token{-1, Token::SQ_RPAREN};
        }
        error();
    }

    return Token{-1, Token::END};
}

/////////////PARSER///////////////

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
        vector<AST_Node *> statements;
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

            vector<AST_Node *> if_statements;
            vector<AST_Node *> else_statements;

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
            vector<AST_Node *> block_statements;

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

inline void Bin_OP::accept(Visitor &v)
{
    v.visit(this);
}

inline void Num::accept(Visitor &v)
{
    v.visit(this);
}

inline void Un_OP::accept(Visitor &v)
{
    v.visit(this);
}

inline void Var::accept(Visitor &v)
{
    v.visit(this);
}

inline void BlockCode::accept(Visitor &v)
{
    v.visit(this);
}

inline void VarAssign::accept(Visitor &v)
{
    v.visit(this);
}

inline void ArrAssign::accept(Visitor &v)
{
    v.visit(this);
}

inline void VarDecl::accept(Visitor &v)
{
    v.visit(this);
}

inline void ArrDecl::accept(Visitor &v)
{
    v.visit(this);
}

inline void IfElse::accept(Visitor &v)
{
    v.visit(this);
}

inline void While::accept(Visitor &v)
{
    v.visit(this);
}

inline void GoTo::accept(Visitor &v)
{
    v.visit(this);
}

inline void Label::accept(Visitor &v)
{
    v.visit(this);
}

inline void Print::accept(Visitor &v)
{
    v.visit(this);
}

inline void ReadVar::accept(Visitor &v)
{
    v.visit(this);
}

inline void ReadArr::accept(Visitor &v)
{
    v.visit(this);
}

inline void NO_OP::accept(Visitor &v)
{
    v.visit(this);
}

inline void Array::accept(Visitor &v)
{
    v.visit(this);
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

//DATA EXTRACTOR

inline void DataExtractor::visit(Var *ast)
{
    type = ast->token;
}

inline void DataExtractor::visit(Array *ast)
{
    type = ast->token;
    helperNode = ast->index;
}

inline void DataExtractor::visit(GoTo *ast)
{
    type = ast->token;
}

inline void DataExtractor::visit(Label *ast)
{
    type = ast->token;
}

inline void DataExtractor::visit(Bin_OP *ast){};
inline void DataExtractor::visit(Un_OP *ast){};
inline void DataExtractor::visit(Num *ast){};
inline void DataExtractor::visit(BlockCode *ast){};
inline void DataExtractor::visit(IfElse *ast){};
inline void DataExtractor::visit(While *ast){};
inline void DataExtractor::visit(VarAssign *ast){};
inline void DataExtractor::visit(ArrAssign *ast){};
inline void DataExtractor::visit(VarDecl *ast){};
inline void DataExtractor::visit(ArrDecl *ast){};
inline void DataExtractor::visit(Print *ast){};
inline void DataExtractor::visit(ReadArr *ast){};
inline void DataExtractor::visit(ReadVar *ast){};
inline void DataExtractor::visit(NO_OP *ast){};

//PRE INTERPRETER

inline void BeforeInterpret::visit(Var *ast){};

inline void BeforeInterpret::visit(Array *ast){};

inline void BeforeInterpret::visit(GoTo *ast){};

inline void BeforeInterpret::visit(Label *ast)
{

    DataExtractor extractor;
    ast->accept(extractor);

    labels.insert(extractor.type.text_data);
}

inline void BeforeInterpret::visit(BlockCode *ast)
{
    for (int i = 0; i < ast->statements.size(); ++i)
    {
        ast->statements[i]->accept(*this);
    }
}

inline void BeforeInterpret::visit(IfElse *ast)
{
    ast->bCode1->accept(*this);
    ast->bCode2->accept(*this);
}

inline void BeforeInterpret::visit(While *ast)
{
    ast->bCode->accept(*this);
}

inline void BeforeInterpret::visit(Bin_OP *ast){};
inline void BeforeInterpret::visit(Un_OP *ast){};
inline void BeforeInterpret::visit(Num *ast){};
inline void BeforeInterpret::visit(VarAssign *ast){};
inline void BeforeInterpret::visit(ArrAssign *ast){};
inline void BeforeInterpret::visit(VarDecl *ast){};
inline void BeforeInterpret::visit(ArrDecl *ast){};
inline void BeforeInterpret::visit(Print *ast){};
inline void BeforeInterpret::visit(ReadArr *ast){};
inline void BeforeInterpret::visit(ReadVar *ast){};
inline void BeforeInterpret::visit(NO_OP *ast){};

//SYMBOL TABLE
inline void SymbolTable::dec_var(string identifier)
{
    vars.insert({identifier, 0});
}
inline void SymbolTable::dec_arr(string identifier, int arr_size)
{
    arrays.insert({identifier, vector<int>(arr_size, 0)});
}

inline void SymbolTable::modify_var(string identifier, int newvalue)
{
    unordered_map<string, int>::iterator got = vars.find(identifier);

    if (got != vars.end())
    {
        got->second = newvalue;
    }
    else
        throw std::invalid_argument("variable cannot be found");
}
inline void SymbolTable::modify_arr(string identifier, int index, int newvalue)
{
    unordered_map<string, vector<int>>::iterator got = arrays.find(identifier);

    if (got != arrays.end() && index < got->second.size())
    {
        got->second[index] = newvalue;
    }
    else
        throw std::invalid_argument("cannot find the value at given index or array is not declared");
}

inline const int &SymbolTable::lookup_var(string varname) const
{
    unordered_map<string, int>::const_iterator got = vars.find(varname);

    if (got != vars.end())
    {
        return got->second;
    }
    else
        throw std::invalid_argument("variable cannot be found");
}
inline const int &SymbolTable::lookup_arr(string arr_name, int index) const
{
    unordered_map<string, vector<int>>::const_iterator got = arrays.find(arr_name);

    if (got != arrays.end() && index < got->second.size())
    {
        return got->second[index];
    }
    else
        throw std::invalid_argument("cannot find the value at given index or array is not declared");
}

inline bool SymbolTable::var_exists(string varname) const
{
    unordered_map<string, int>::const_iterator got = vars.find(varname);

    if (got != vars.end())
        return true;
    else
        return false;
}

inline bool SymbolTable::arr_exists(string arrname) const
{
    unordered_map<string, vector<int>>::const_iterator got = arrays.find(arrname);

    if (got != arrays.end())
        return true;
    else
        return false;
}
//SCOPED TABLE

inline void ScopedTable::dec_var(string identifier)
{
    scopes[top].dec_var(identifier);
}
inline void ScopedTable::dec_arr(string identifier, int arr_size)
{
    scopes[top].dec_arr(identifier, arr_size);
}

inline void ScopedTable::modify_var(string identifier, int newvalue)
{
    for (int i = 0; i <= top; ++i)
    {
        if (scopes[i].var_exists(identifier))
        {
            scopes[i].modify_var(identifier, newvalue);
            return;
        }
    }

    throw std::invalid_argument("something went wrong");
}
inline void ScopedTable::modify_arr(string identifier, int index, int newvalue)
{
    for (int i = 0; i <= top; ++i)
    {
        if (scopes[i].arr_exists(identifier))
        {
            scopes[i].modify_arr(identifier, index, newvalue);
            return;
        }
    }

    throw std::invalid_argument("something went wrong");
}

inline const int &ScopedTable::lookup_var(string varname) const
{
    for (int i = 0; i <= top; ++i)
    {
        if (scopes[i].var_exists(varname))
        {
            return scopes[i].lookup_var(varname);
        }
    }

    throw std::invalid_argument("something went wrong");
}

inline const int &ScopedTable::lookup_arr(string arr_name, int index) const
{
    for (int i = 0; i <= top; ++i)
    {
        if (scopes[i].arr_exists(arr_name))
        {
            return scopes[i].lookup_arr(arr_name, index);
        }
    }

    throw std::invalid_argument("something went wrong");
}

inline void ScopedTable::addScope()
{
    SymbolTable _new;
    scopes.push_back(_new);

    ++top;
}

inline void ScopedTable::removeScope()
{
    --top;
}

inline void ScopedTable::back_to_global()
{
    top = 0;
}

inline ScopedTable::ScopedTable() : top(0)
{
    SymbolTable global_scope;
    scopes.push_back(global_scope);
}

//INTERPRETER

inline Interpreter::Interpreter()
{
    skip_mode = false;
}

inline Interpreter::Interpreter(AST_Node *t) : tree(t)
{
    skip_mode = false;

    BeforeInterpret b;
    tree->accept(b);
    labels = b.labels;
}

inline void Interpreter::visit(GoTo *ast)
{

    if (skip_mode)
        return;

    ast->accept(extractor);
    string goto_label = extractor.type.text_data;

    unordered_set<string>::const_iterator got = labels.find(goto_label);
    if (got != labels.end())
    {

        looking_for_label = goto_label;
        skip_mode = true;
        nested_scopes.back_to_global();
    }
    else
        throw std::invalid_argument("no such label in program!");
}

inline void Interpreter::visit(Label *ast)
{

    ast->accept(extractor);
    string goto_label = extractor.type.text_data;

    if (goto_label == looking_for_label)
    {
        looking_for_label = "";
        skip_mode = false;
    }
}

inline void Interpreter::visit(BlockCode *ast)
{
    for (int i = 0; i < ast->statements.size(); ++i)
    {
        ast->statements[i]->accept(*this);
    }
}

inline void Interpreter::visit(IfElse *ast)
{
    if (skip_mode)
        return;

    ast->expr->accept(*this);
    int expr = value;

    nested_scopes.addScope();
    if (expr)
    {
        ast->bCode1->accept(*this);
    }
    else
    {
        ast->bCode2->accept(*this);
    }
    nested_scopes.removeScope();
}

inline void Interpreter::visit(While *ast)
{
    if (skip_mode)
        return;

    ast->expr->accept(*this);
    int expr = value;

    while (expr)
    {
        nested_scopes.addScope();

        ast->bCode->accept(*this);

        ast->expr->accept(*this);
        expr = value;

        nested_scopes.removeScope();
    }
}

inline void Interpreter::visit(VarDecl *ast)
{
    if (skip_mode)
        return;

    ast->var->accept(extractor);
    string varname = extractor.type.text_data;

    nested_scopes.dec_var(varname);
}

inline void Interpreter::visit(ArrDecl *ast)
{
    if (skip_mode)
        return;

    ast->arr->accept(extractor);
    string arrname = extractor.type.text_data;

    extractor.helperNode->accept(*this);

    nested_scopes.dec_arr(arrname, value);
}

inline void Interpreter::visit(VarAssign *ast)
{
    if (skip_mode)
        return;

    ast->var->accept(extractor);
    string varname = extractor.type.text_data;

    ast->expr->accept(*this);

    nested_scopes.modify_var(varname, value);
}

inline void Interpreter::visit(ArrAssign *ast)
{
    if (skip_mode)
        return;

    ast->arr->accept(extractor);
    string arrname = extractor.type.text_data;

    ast->expr->accept(*this);
    int data = value;

    extractor.helperNode->accept(*this);
    int index = value;

    nested_scopes.modify_arr(arrname, index, data);
}

inline void Interpreter::visit(ReadVar *ast)
{
    if (skip_mode)
        return;

    string input;
    getline(cin, input);

    Lexer inputLex(input);
    Parser inputParse(inputLex);

    inputParse.Expression()->accept(*this);
    int inputValue = value;

    ast->var->accept(extractor);
    string varname = extractor.type.text_data;

    nested_scopes.modify_var(varname, inputValue);
}

inline void Interpreter::visit(ReadArr *ast)
{
    if (skip_mode)
        return;

    string input;
    getline(cin, input);

    Lexer inputLex(input);
    Parser inputParse(inputLex);

    inputParse.Expression()->accept(*this);
    int inputValue = value;
    //cout << "the value of entered expression is" << inputValue << endl;

    ast->arr->accept(extractor);
    string arrname = extractor.type.text_data;

    extractor.helperNode->accept(*this);
    int index = value;

    nested_scopes.modify_arr(arrname, index, inputValue);
}

inline void Interpreter::visit(Print *ast)
{
    if (skip_mode)
        return;

    ast->expr_to_print->accept(*this);

    cout << value << endl;
}

inline void Interpreter::visit(Bin_OP *ast)
{
    int v1, v2;
    ast->left->accept(*this); //update value
    v1 = value;
    ast->right->accept(*this);
    v2 = value;

    switch (ast->op.t)
    {
    case Token::PLUS:
        value = v1 + v2;
        break;
    case Token::MINUS:
        value = v1 - v2;
        break;
    case Token::MUL:
        value = v1 * v2;
        break;
    case Token::DIV:
        if (v2 == 0)
            throw std::invalid_argument("cant divide by zero!");
        value = v1 / v2;
        break;
    case Token::MOD:
        value = v1 % v2;
        break;
    case Token::EQ:
        value = v1 == v2;
        break;
    case Token::NEQ:
        value = v1 != v2;
        break;
    case Token::LESS:
        value = v1 < v2;
        break;
    case Token::LESSEQ:
        value = v1 <= v2;
        break;
    case Token::MORE:
        value = v1 > v2;
        break;
    case Token::MOREEQ:
        value = v1 >= v2;
        break;
    case Token::OR:
        value = v1 || v2;
        break;
    case Token::AND:
        value = v1 && v2;
        break;
    }
}

inline void Interpreter::visit(Num *ast)
{
    value = ast->token.value;
}

inline void Interpreter::visit(Var *ast)
{
    string varname = ast->token.text_data;

    value = nested_scopes.lookup_var(varname);
}

inline void Interpreter::visit(Array *ast)
{
    string arr_name = ast->token.text_data;

    ast->index->accept(*this);
    int index = value;

    value = nested_scopes.lookup_arr(arr_name, index);
}

inline void Interpreter::visit(Un_OP *ast)
{
    ast->expr->accept(*this);

    switch (ast->op.t)
    {
    case Token::NOT:
        value = !value;
        break;
    case Token::MINUS:
        value = -value;
        break;
    }
}

inline void Interpreter::visit(NO_OP *ast)
{
    return;
}

inline void Interpreter::interpret_fullprogram()
{
    tree->accept(*this);
    tree->accept(deleter);
}
inline void Interpreter::interpret_REPL()
{
    string input;
    cout << ">";
    getline(cin, input);

    if (input != "stop")
    {
        Lexer lex(input);
        Parser par(lex);
        AST_Node *tempTree = par.parse();
        tempTree->accept(*this);
        tempTree->accept(deleter);
    }
    else
        throw std::invalid_argument("end");
}

#endif 