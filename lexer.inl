#ifndef LEXER_SOURCE
#define LEXER_SOURCE

inline Lexer::Lexer(const std::string &input) : text(input), pos(0)
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
    std::string result;

    while (current_char && isalpha(current_char))
    {
        result.push_back(current_char);
        advance();
    }

    std::unordered_map<std::string, Token>::const_iterator got = RESERVED_KEYWORDS.find(result);
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

#endif