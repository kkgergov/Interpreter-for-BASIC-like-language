# Interpreter

Interpreter for programming language.

## Description

No desc so far, too lazy.

### Getting started

* Download the .h, .inl and .cpp files.
* Optionally download the test programs.
* Write program in txt file or use the REPL mode.
* The program you write must obey the following grammar:
```
<Program_Lines>   ::= 
                 (<Statement> NEWLINE)*
                | <Statement> END

<Statement>   ::= 
                | GOTO ID
                | LABEL ID 
                | IF <Expression> (<Statement> NEWLINE)* [ELSE (<Statement> NEWLINE)*] ENDIF
                | WHILE <Expression> (<Statement> NEWLINE)* DONE     
                | LET ID
                | LET ID '[' <Expression> ']'
                | ID '=' <Expression>
                | ID '[' <Expression> ']' '=' <Expression>
                | PRINT <Expression> 
                | READ ID
                | READ ID '[' <Expression> ']'
                | NO_OP

<Expression>  ::= <And Exp> '||' <Expression> 
                | <And Exp> 

<And Exp>     ::= <Not Exp> '&&' <And Exp> 
                | <Not Exp> 
 
<Not Exp>     ::= '!' <Compare Exp> 
                | <Compare Exp> 

<Compare Exp> ::= <Add Exp> '=='  <Compare Exp> 
                | <Add Exp> '!=' <Compare Exp> 
                | <Add Exp> '>'  <Compare Exp> 
                | <Add Exp> '>=' <Compare Exp> 
                | <Add Exp> '<'  <Compare Exp> 
                | <Add Exp> '<=' <Compare Exp> 
                | <Add Exp> 

<Add Exp>     ::= <Mult Exp> '+' <Add Exp> 
                | <Mult Exp> '-' <Add Exp> 
                | <Mult Exp> 

<Mult Exp>    ::= <Negate Exp> '*' <Mult Exp> 
                | <Negate Exp> '/' <Mult Exp> 
                | <Negate Exp> 

<Negate Exp>  ::= '-' <Value> 
                | <Value> 

<Value>       ::= '(' <Expression> ')'
                | ID 
                | ID '[' <Expression> ']'
                | INTEGER
```

## License

This project is licensed under the MIT License - see the LICENSE.md file for details

