#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
using namespace std;

class Lexer
{
private:
    vector<string> tokens;
    int currentTokenIndex;

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
                // ignore line comment
                else if (c == '/' && i + 1 < line.size() && line[i + 1] == '/')
                {
                    break;
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
        else if (line == "var")
        {
            result = "KW_VAR " + line + " " + to_string(numberOfLine);
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
            result = "UNKNOWN " + line + " " + to_string(numberOfLine);
        }
        return result;
    }

    vector<string> tokenize(string fileName)
    {
        ifstream file(fileName);

        if (!file)
        {
            cout << "Can't open file: " << fileName << endl;
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
                    tokens.push_back(this->classify(line, numberOfLine));
                }
                numberOfLine++;
            }

            // for (string s : tokens)
            // {
            //     cout << s << endl;
            // }

            return tokens;
        }

        file.close();
    }

public:
    Lexer() {}
    Lexer(string fileName)
    {
        this->tokens = this->tokenize(fileName);
        this->currentTokenIndex = 0;
    }

    string nextToken()
    {
        if (currentTokenIndex < tokens.size())
        {
            string token = tokens[this->currentTokenIndex];
            this->currentTokenIndex++;
            return token;
        }
        return "";
    }

    void reset()
    {
        currentTokenIndex = 0;
    }
};

class Parser
{
private:
    Lexer &lexer; // We need reference not a copy of object!
    string currentToken;
    int depth = 0;
    vector<string> ASTNodes;

    string printDepth()
    {
        string result = "";
        for (int i = 0; i < depth; i++)
        {
            result += " ";
        }
        return result;
    }

    void addNode(string name)
    {
        // cout << printDepth() + name << endl;
        ASTNodes.push_back(printDepth() + name);
    }

    void parse()
    {
        program();
    }

    void program()
    {
        addNode("<program>");
        depth++;
        statement_list();
        depth--;
    }

    void statement_list()
    {
        addNode("<statement_list>");
        depth++;
        while (!currentToken.empty() && currentToken.substr(0, currentToken.find(' ')) != "R_BRACET")
        {
            statement();
        }
        depth--;
    }

    void statement()
    {
        addNode("<statement>");
        depth++;
        if (currentToken.substr(0, currentToken.find(' ')) == "KW_VAR")
        {
            assignment();
        }
        else if (currentToken.substr(0, currentToken.find(' ')) == "KW_IF")
        {
            if_stmt();
        }
        else if (currentToken.substr(0, currentToken.find(' ')) == "KW_FOR")
        {
            for_stmt();
        }
        else if (currentToken.substr(0, currentToken.find(' ')) == "KW_WHILE")
        {
            while_stmt();
        }
        else if (currentToken.substr(0, currentToken.find(' ')) == "KW_PRINT")
        {
            print_stmt();
        }
        else
        {
            syntaxError("Unexpected token");
        }
        depth--;
    }

    void assignment()
    {
        addNode("<assignment>");
        depth++;
        consume("KW_VAR");
        consume("IDN");
        consume("OP_ASSIGN");
        expr();
        depth--;
    }

    void if_stmt()
    {
        addNode("<if_stmt>");
        depth++;
        consume("KW_IF");
        expr();
        block();
        if_stmt_tail();
        depth--;
    }

    void if_stmt_tail()
    {
        if (currentToken.substr(0, currentToken.find(' ')) != "KW_ELSE")
        {
            return;
        }
        addNode("<if_stmt_tail>");
        depth++;
        consume("KW_ELSE");
        if (currentToken.substr(0, currentToken.find(' ')) == "KW_IF")
        {
            if_stmt();
        }
        else
        {
            block();
        }
        depth--;
    }

    void for_stmt()
    {
        addNode("<for_stmt>");
        depth++;
        consume("KW_FOR");
        consume("IDN");
        consume("KW_FROM");
        expr();
        consume("KW_TO");
        expr();
        block();
        depth--;
    }

    void while_stmt()
    {
        addNode("<while_stmt>");
        depth++;
        consume("KW_WHILE");
        expr();
        block();
        depth--;
    }

    void print_stmt()
    {
        addNode("<print_stmt>");
        depth++;
        consume("KW_PRINT");
        expr();
        depth--;
    }

    void expr()
    {
        addNode("<expr>");
        depth++;
        comparison();
        depth--;
    }

    void comparison()
    {
        addNode("<comparison>");
        depth++;
        operand();
        while (isRelOp(currentToken.substr(0, currentToken.find(' '))))
        {
            // cout << "ovdje" << endl;
            consume(currentToken.substr(0, currentToken.find(' ')));
            operand();
        }
        depth--;
    }

    void operand()
    {
        addNode("<operand>");
        depth++;
        term();
        while (currentToken.substr(0, currentToken.find(' ')) == "OP_PLUS" || currentToken.substr(0, currentToken.find(' ')) == "OP_MINUS")
        {
            consume(currentToken.substr(0, currentToken.find(' ')));
            term();
        }
        depth--;
    }

    bool isRelOp(string token)
    {
        // cout << token << endl;
        return token == "OP_EQUAL" || token == "OP_NOT_EQUAL" || token == "OP_LESS" || token == "OP_GREATER" || token == "OP_LESS_EQUAL" || token == "OP_GREATER_EQUAL";
    }

    void block()
    {
        addNode("<block>");
        depth++;
        consume("L_BRACET");
        statement_list();
        consume("R_BRACET");
        depth--;
    }

    void term()
    {
        addNode("<term>");
        depth++;
        factor();
        while (currentToken.substr(0, currentToken.find(' ')) == "OP_STAR" || currentToken.substr(0, currentToken.find(' ')) == "OP_SLASH")
        {
            consume(currentToken.substr(0, currentToken.find(' ')));
            factor();
        }
        depth--;
    }

    void factor()
    {
        addNode("<factor>");
        depth++;
        if (currentToken.substr(0, currentToken.find(' ')) == "IDN")
        {
            consume("IDN");
        }
        else if (currentToken.substr(0, currentToken.find(' ')) == "CONST_NUM")
        {
            consume("CONST_NUM");
        }
        else if (currentToken.substr(0, currentToken.find(' ')) == "CONST_STRING")
        {
            consume("CONST_STRING");
        }
        else if (currentToken.substr(0, currentToken.find(' ')) == "L_PARENT")
        {
            consume("L_PARENT");
            expr();
            consume("R_PARENT");
        }
        else
        {
            syntaxError("Unexpected token in factor");
        }
        depth--;
    }

    void consume(string expectedToken)
    {
        if (currentToken.substr(0, currentToken.find(' ')) == expectedToken)
        {
            addNode(currentToken);
            currentToken = lexer.nextToken();
            // cout << currentToken << endl;
        }
        else
        {
            syntaxError("Expected " + expectedToken);
        }
    }

    void syntaxError(string message)
    {
        if (message == "Unexpected token")
        {
            cout << "Syntax error: " << message << " '" << currentToken << "'" << endl;
        }
        else
        {
            cout << "Syntax error: " << message << ", but found '" << currentToken << "'" << endl;
        }
        exit(1);
    }

public:
    // Parser() {}
    Parser(Lexer &lexer) : lexer(lexer)
    {
        this->currentToken = lexer.nextToken();
        this->parse();
    }

    vector<string> getASTNodes()
    {
        return this->ASTNodes;
    }
};

class SemanticAnalyzer
{
    Parser &parser;

public:
    SemanticAnalyzer(Parser &parser) : parser(parser)
    {
        this->analyze();
    }

    void analyze()
    {
        for (string s : parser.getASTNodes())
        {
            cout << s << endl;
        }
    }
};

class SimpleLang
{
private:
    string fileName;

public:
    SimpleLang(string fileName)
    {
        this->fileName = fileName;
    }

    void run()
    {
        Lexer lexer(this->fileName);
        Parser parser(lexer);
        SemanticAnalyzer analyzer(parser);
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
