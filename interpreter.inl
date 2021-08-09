#ifndef INTERPRETER_SOURCE
#define INTERPRETER_SOURCE

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
    std::string goto_label = extractor.type.text_data;

    std::unordered_set<std::string>::const_iterator got = labels.find(goto_label);
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
    std::string goto_label = extractor.type.text_data;

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
    std::string varname = extractor.type.text_data;

    nested_scopes.dec_var(varname);
}

inline void Interpreter::visit(ArrDecl *ast)
{
    if (skip_mode)
        return;

    ast->arr->accept(extractor);
    std::string arrname = extractor.type.text_data;

    extractor.helperNode->accept(*this);

    nested_scopes.dec_arr(arrname, value);
}

inline void Interpreter::visit(VarAssign *ast)
{
    if (skip_mode)
        return;

    ast->var->accept(extractor);
    std::string varname = extractor.type.text_data;

    ast->expr->accept(*this);

    nested_scopes.modify_var(varname, value);
}

inline void Interpreter::visit(ArrAssign *ast)
{
    if (skip_mode)
        return;

    ast->arr->accept(extractor);
    std::string arrname = extractor.type.text_data;

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

    std::string input;
    getline(std::cin, input);

    Lexer inputLex(input);
    Parser inputParse(inputLex);

    inputParse.Expression()->accept(*this);
    int inputValue = value;

    ast->var->accept(extractor);
    std::string varname = extractor.type.text_data;

    nested_scopes.modify_var(varname, inputValue);
}

inline void Interpreter::visit(ReadArr *ast)
{
    if (skip_mode)
        return;

    std::string input;
    getline(std::cin, input);

    Lexer inputLex(input);
    Parser inputParse(inputLex);

    inputParse.Expression()->accept(*this);
    int inputValue = value;
    //cout << "the value of entered expression is" << inputValue << endl;

    ast->arr->accept(extractor);
    std::string arrname = extractor.type.text_data;

    extractor.helperNode->accept(*this);
    int index = value;

    nested_scopes.modify_arr(arrname, index, inputValue);
}

inline void Interpreter::visit(Print *ast)
{
    if (skip_mode)
        return;

    ast->expr_to_print->accept(*this);

    std::cout << value << std::endl;
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
    std::string varname = ast->token.text_data;

    value = nested_scopes.lookup_var(varname);
}

inline void Interpreter::visit(Array *ast)
{
    std::string arr_name = ast->token.text_data;

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
    std::string input;
    std::cout << ">";
    getline(std::cin, input);

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