#include <iostream>
#include <string>
#include <fstream>
#include "interpreter.h"

using namespace std;

int main()
{

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
        cout << input;
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