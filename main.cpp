#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include "interpreter.h"

using namespace std;

int main()
{
    //SCOPE TEST
    //Lexer lexer("LET a \n a = 10 \n LET b \n LET c \n READ a \n READ b \n READ c \n PRINT a \n PRINT b \n PRINT c");

    int c;
    cout << "0-import program from text file / 1-enter REPL MODE \n";
    cin >> c;

    if (c == 0)
    {
        string filename, input;
        cout << "Enter full file name:";
        cin >> filename;

        fstream file;
        file.open(filename, fstream::in);
        getline(file, input, '\0');
        file.close();

        Lexer lexer(input);
        Parser parser(lexer);
        AST_Node *tree = parser.parse();

        Interpreter interpreter(tree);
        interpreter.interpret_fullprogram();
    }
    else
    {
        cout << "Type 'stop' to break out of repl mode\n";

        Interpreter interpreter;
        while (true)
        {
            try
            {
                interpreter.interpret_REPL();
            }
            catch (...)
            {
                cout << "exiting REPL \n";
                break;
            }
        }
    }
}