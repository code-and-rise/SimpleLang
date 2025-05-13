#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
using namespace std;

class SimpleLang
{
private:
    string fileName;

    class Lexer
    {
    private:
        string fileName;

        vector<string> removeWhitespace(string line)
        {
            vector<string> tokens;
            string current;
            bool isStringConstant = false;

            for (int i = 0; i < line.size(); i++)
            {
                char c = line[i];

                if (isspace(c))
                {
                    if (!current.empty())
                    {
                        tokens.push_back(current);
                        current.clear();
                    }
                }
                // Letters and digits for IDN and CONST
                else if (isalnum(c) || c == '_')
                {
                    current += c;
                }
                else if (c == '"')
                {
                    if (!isStringConstant)
                    {
                        isStringConstant = true;
                        if (!current.empty())
                        {
                            tokens.push_back(current);
                            current.clear();
                        }
                        current += c;
                    }
                    else
                    {
                        current += c;
                        if (!current.empty())
                        {
                            tokens.push_back(current);
                            current.clear();
                        }
                    }
                }
                else
                {
                    if (!current.empty())
                    {
                        tokens.push_back(current);
                        current.clear();
                    }

                    if ((c == '=' || c == '!' || c == '<' || c == '>' || c == '+') && i + 1 < line.size() && line[i + 1] == '=')
                    {
                        tokens.push_back(string(1, c) + "=");
                        ++i;
                    }
                    else
                    {
                        tokens.push_back(string(1, c));
                    }
                }
            }

            return tokens;
        }

        string classify(string line, int numberOfLine)
        {
            string result = "";
            // Keywords
            if (line == "print")
            {
                result = "KW_PRINT " + line + " " + to_string(numberOfLine);
            }
            else if (line == "if")
            {
                result = "KW_IF " + line + " " + to_string(numberOfLine);
            }
            else if (line == "else")
            {
                result = "KW_ELSE " + line + " " + to_string(numberOfLine);
            }
            else if (line == "while")
            {
                result = "KW_WHILE " + line + " " + to_string(numberOfLine);
            }
            else if (line == "for")
            {
                result = "KW_FOR " + line + " " + to_string(numberOfLine);
            }
            else if (line == "from")
            {
                result = "KW_FROM " + line + " " + to_string(numberOfLine);
            }
            else if (line == "to")
            {
                result = "KW_TO " + line + " " + to_string(numberOfLine);
            }
            else if (line == "function")
            {
                result = "KW_FUNCTION " + line + " " + to_string(numberOfLine);
            }

            // Special characters
            else if (line == "(")
            {
                result = "L_PARENT " + line + " " + to_string(numberOfLine);
            }
            else if (line == ")")
            {
                result = "R_PARENT " + line + " " + to_string(numberOfLine);
            }
            else if (line == "{")
            {
                result = "L_BRACET " + line + " " + to_string(numberOfLine);
            }
            else if (line == "}")
            {
                result = "R_BRACET " + line + " " + to_string(numberOfLine);
            }
            else if (line == ",")
            {
                result = "COMMA " + line + " " + to_string(numberOfLine);
            }
            else if (line == ":")
            {
                result = "D_DOT " + line + " " + to_string(numberOfLine);
            }

            // Operators
            else if (line == "=")
            {
                result = "OP_ASSIGN " + line + " " + to_string(numberOfLine);
            }
            else if (line == "==")
            {
                result = "OP_EQUAL " + line + " " + to_string(numberOfLine);
            }
            else if (line == "!=")
            {
                result = "OP_NOT_EQUAL " + line + " " + to_string(numberOfLine);
            }
            else if (line == "<")
            {
                result = "OP_LESS " + line + " " + to_string(numberOfLine);
            }
            else if (line == ">")
            {
                result = "OP_GREATER " + line + " " + to_string(numberOfLine);
            }
            else if (line == "<=")
            {
                result = "OP_LESS_EQUAL " + line + " " + to_string(numberOfLine);
            }
            else if (line == ">=")
            {
                result = "OP_GREATER_EQUAL " + line + " " + to_string(numberOfLine);
            }
            else if (line == "+=")
            {
                result = "OP_PLUS_EQUAL " + line + " " + to_string(numberOfLine);
            }
            else if (line == "+")
            {
                result = "OP_PLUS " + line + " " + to_string(numberOfLine);
            }
            else if (line == "-")
            {
                result = "OP_MINUS " + line + " " + to_string(numberOfLine);
            }
            else if (line == "/")
            {
                result = "OP_SLASH " + line + " " + to_string(numberOfLine);
            }
            else if (line == "*")
            {
                result = "OP_STAR " + line + " " + to_string(numberOfLine);
            }

            // Constants
            else if (regex_match(line, regex("[0-9]+")))
            {
                result = "CONST_NUM " + line + " " + to_string(numberOfLine);
            }
            else if (regex_match(line, regex("\".*\"")))
            {
                result = "CONST_STRING " + line + " " + to_string(numberOfLine);
            }

            // Identifiers
            else if (regex_match(line, regex("[a-zA-Z_][a-zA-Z0-9_]*")))
            {
                result = "IDN " + line + " " + to_string(numberOfLine);
            }

            // Unknown
            else
            {
                result = "UNKNOWN";
            }
            return result;
        }

    public:
        Lexer(string fileName)
        {
            this->fileName = fileName;
        }

        vector<string> tokenize()
        {
            ifstream file(this->fileName);

            if (!file)
            {
                cout << "Can't open file: " << this->fileName << endl;
            }
            else
            {
                string line;
                int numberOfLine = 1;

                vector<string> tokens;
                while (getline(file, line))
                {
                    if (line != "")
                    {
                        line += "\n"; // getline() is removing \n when reading the line from file!!!
                    }
                    vector<string> formattedLine = this->removeWhitespace(line);

                    for (string line : formattedLine)
                    {
                        // cout << lexer(line, numberOfLine) << endl;
                        tokens.push_back(this->classify(line, numberOfLine));
                    }
                    numberOfLine++;
                }

                for (string token : tokens)
                {
                    cout << token << endl;
                }

                return tokens;
            }

            file.close();
        }
    };

    class Parser
    {
    private:
        Lexer lexer;
    };

public:
    SimpleLang(string fileName)
    {
        this->fileName = fileName;
    }

    void run()
    {
        Lexer lexer(this->fileName);
        lexer.tokenize();
    }
};

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cout << "USE: " << argv[0] << " <file name>" << endl;
        return 1;
    }

    SimpleLang myProgram(argv[1]);
    myProgram.run();

    return 0;
}
