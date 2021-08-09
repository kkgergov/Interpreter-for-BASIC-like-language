#include <iostream>
#include <string>
#include <fstream>
#include "interpreter.h"

int main()
{
    //SCOPE TEST
    //Lexer lexer("LET a \n a = 10 \n LET b \n LET c \n READ a \n READ b \n READ c \n PRINT a \n PRINT b \n PRINT c");

    int c;
    std::cout << "0-import program from text file / 1-enter REPL MODE \n";
    std::cin >> c;

    if (c == 0)
    {
        std::string filename, input;
        std::cout << "Enter full file name:";
        std::cin >> filename;

        std::fstream file;
        file.open(filename, std::fstream::in);
        std::getline(file, input, '\0');
        file.close();

        Lexer lexer(input);
        Parser parser(lexer);
        AST_Node *tree = parser.parse();

        Interpreter interpreter(tree);
        interpreter.interpret_fullprogram();
    }
    else
    {
        std::cout << "Type 'stop' to break out of repl mode\n";

        Interpreter interpreter;
        while (true)
        {
            try
            {
                interpreter.interpret_REPL();
            }
            catch (...)
            {
                std::cout << "exiting REPL \n";
                break;
            }
        }
    }
}