#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <stack>
#include <regex>
using namespace std;

// Class that will handle reading and running .sl files
class SimpleLang
{
private:
    string fileName;
    vector<string> linesOfCode;

    void readFile()
    {
        ifstream file(this->fileName);
        if (!file)
        {
            cout << "Can't open this file: " << this->fileName << endl;
            return;
        }

        string line;
        while (getline(file, line))
        {
            line += "\n"; // getline is removing '\n' when reading the line!
            linesOfCode.push_back(line);
        }

        return;
    }

    // Lexer class for making tokens out of written program
    class Lexer
    {
    private:
        vector<string> linesOfCode;
        vector<string> tokens;

        void tokenize()
        {
            int lineOfCode = 0;
            bool inString = false;
            for (string line : linesOfCode)
            {
                string current;
                bool inString = false;
                lineOfCode++;

                for (int i = 0; i < line.size(); i++)
                {
                    char c = line[i];

                    if (isspace(c))
                    {
                        if (inString)
                        {
                            current += c;
                        }
                        else if (!current.empty())
                        {
                            tokens.push_back(current + " " + to_string(lineOfCode));
                            current.clear();
                        }
                    }
                    else if (c == '"')
                    {
                        inString = !inString;
                        current += c;

                        // If closing quote, save token
                        if (!inString)
                        {
                            tokens.push_back(current + " " + to_string(lineOfCode));
                            current.clear();
                        }
                    }
                    else if (inString)
                    {
                        current += c;
                    }
                    else if (isalnum(c) || c == '_')
                    {
                        current += c;
                    }
                    else
                    {
                        if (!current.empty())
                        {
                            tokens.push_back(current + " " + to_string(lineOfCode));
                            current.clear();
                        }

                        // Check bounds before accessing line[i+1]
                        if (i + 1 < line.size())
                        {
                            char next = line[i + 1];

                            // Double character operators (e.g. ==, !=, <=, >=)
                            if ((c == '=' || c == '!' || c == '<' || c == '>') && next == '=')
                            {
                                tokens.push_back(string(1, c) + "= " + to_string(lineOfCode));
                                i++; // Skip next char
                                continue;
                            }

                            // Line comment
                            if (c == '/' && next == '/')
                            {
                                break; // Ignore rest of the line
                            }
                        }

                        // Single character symbol
                        tokens.push_back(string(1, c) + " " + to_string(lineOfCode));
                    }
                }

                // Final token at line end
                if (!current.empty())
                {
                    tokens.push_back(current + " " + to_string(lineOfCode));
                }
            }
        }

        void classify()
        {
            for (int i = 0; i < tokens.size(); i++)
            {
                string token = tokens[i];
                string temp = token.substr(0, token.find_last_of(' '));
                string line = token.substr(token.find_last_of(' ') + 1);
                string tokenClass = "";

                // Keywords
                if (temp == "print")
                {
                    tokenClass = "KW_PRINT";
                }
                else if (temp == "var")
                {
                    tokenClass = "KW_VAR";
                }
                else if (temp == "if")
                {
                    tokenClass = "KW_IF";
                }
                else if (temp == "else")
                {
                    tokenClass = "KW_ELSE";
                }
                else if (temp == "for")
                {
                    tokenClass = "KW_FOR";
                }

                // Special characters
                else if (temp == "(")
                {
                    tokenClass = "L_PARENT";
                }
                else if (temp == ")")
                {
                    tokenClass = "R_PARENT";
                }
                else if (temp == "{")
                {
                    tokenClass = "L_BRACE";
                }
                else if (temp == "}")
                {
                    tokenClass = "R_BRACE";
                }
                else if (temp == ";")
                {
                    tokenClass = "SC_SEMICOLON";
                }
                else if (temp == ":")
                {
                    tokenClass = "SC_COLON";
                }

                // Operators
                else if (temp == "=")
                {
                    tokenClass = "OP_ASSIGN";
                }
                else if (temp == "+")
                {
                    tokenClass = "OP_PLUS";
                }
                else if (temp == "-")
                {
                    tokenClass = "OP_MINUS";
                }
                else if (temp == "/")
                {
                    tokenClass = "OP_SLASH";
                }
                else if (temp == "%")
                {
                    tokenClass = "OP_MOD";
                }
                else if (temp == "*")
                {
                    tokenClass = "OP_STAR";
                }
                else if (temp == "==")
                {
                    tokenClass = "OP_EQ";
                }
                else if (temp == "!=")
                {
                    tokenClass = "OP_NOT_EQ";
                }
                else if (temp == ">=")
                {
                    tokenClass = "OP_GE";
                }
                else if (temp == "<=")
                {
                    tokenClass = "OP_LE";
                }
                else if (temp == ">")
                {
                    tokenClass = "OP_GT";
                }
                else if (temp == "<")
                {
                    tokenClass = "OP_LT";
                }
                // Constants
                else if (regex_match(temp, regex("[0-9]+")))
                {
                    tokenClass = "CONST_INT";
                }
                // Include spaces in STRING
                else if (regex_match(temp, regex("^\"[^\"]*\"$")))
                {
                    tokenClass = "CONST_STRING";
                }
                else if (temp == "true" || temp == "false")
                {
                    tokenClass = "CONST_BOOL";
                }

                // Identifiers
                else if (regex_match(temp, regex("[a-zA-Z_][a-zA-Z0-9_]*")))
                {
                    tokenClass = "IDN";
                }

                else if (regex_match(temp, regex("[0-9]+[a-zA-Z0-9_]*")))
                {
                    cerr << "Expected an identifier! Line: " << line << " | code: " << temp << endl;
                    exit(1);
                }

                tokens[i] = tokenClass + " " + token;
            }
        }

    public:
        Lexer(vector<string> linesOfCode)
        {
            this->linesOfCode = linesOfCode;
        }

        vector<string> getClassifiedTokens()
        {
            return this->tokens;
        }

        void start()
        {
            this->tokenize();
            this->classify();
        }
    };

    class ASTNode
    {
    private:
        string node;
        int depth;

    public:
        ASTNode() {}
        ASTNode(string node, int depth)
        {
            this->node = node;
            this->depth = depth;
        }

        int getDepth()
        {
            return this->depth;
        }

        string getNode()
        {
            return this->node;
        }

        string getNodeWithDepth()
        {
            string hierarchy = "";
            for (int i = 0; i < this->depth; i++)
            {
                hierarchy += " ";
            }
            return hierarchy + this->node;
        }

        string getTokenType()
        {
            return this->node.substr(0, this->node.find_first_of(' '));
        }

        string getTokenValue()
        {
            string tokenValue = this->node.substr(this->node.find_first_of(' ') + 1);
            return tokenValue.substr(0, tokenValue.find_last_of(' '));
        }

        int getLineInCode()
        {
            return stoi(this->node.substr(this->node.find_last_of(' ') + 1));
        }

        bool isTokenType(string tokenType)
        {
            return this->getTokenType() == tokenType;
        }

        bool isOperator()
        {
            return this->getTokenType().substr(0, 2).compare("OP") == 0;
        }

        bool empty()
        {
            return this->node.empty();
        }
    };

    class Parser
    {
    private:
        vector<string> classifiedTokens;
        vector<ASTNode> AST;
        int depth = 0;
        int currentTokenIndex;
        string currentToken;

        string nextToken()
        {
            if (currentTokenIndex < classifiedTokens.size())
            {
                return classifiedTokens[currentTokenIndex++];
            }
            return "";
        }

        string peekNextToken()
        {
            if (currentTokenIndex < classifiedTokens.size())
            {
                string temp = classifiedTokens[currentTokenIndex];
                return temp.substr(0, temp.find(' '));
            }

            return "";
        }

        string getCurrentToken()
        {
            return currentToken.substr(0, currentToken.find(' '));
        }

        string getLineOfCode()
        {
            return currentToken.substr(currentToken.find_last_of(' ') + 1);
        }

        void addNode(string name)
        {
            AST.push_back(createASTNode(name));
        }

        ASTNode createASTNode(string name)
        {
            ASTNode node(name, depth);
            return node;
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
            string token = getCurrentToken();
            if (token.empty() || token == "R_BRACE")
            {
                return;
            }
            addNode("<statement_list>");
            depth++;
            statement();
            statement_list();
            depth--;
        }

        void statement()
        {
            addNode("<statement>");
            depth++;
            string token = getCurrentToken();
            if (token == "KW_VAR")
            {
                declaration();
            }
            else if (token == "IDN")
            {
                assignment();
            }
            else if (token == "KW_IF")
            {
                if_stmt();
            }
            else if (token == "KW_PRINT")
            {
                print_stmt();
            }
            else if (token == "KW_FOR")
            {
                for_stmt();
            }
            else if (token == "L_BRACE")
            {
                block();
            }
            else
            {
                syntaxError("Unexpected token " + token, this->getLineOfCode());
            }
            depth--;
        }

        void declaration()
        {
            addNode("<declaration>");
            depth++;
            consume("KW_VAR");
            consume("IDN");
            assign_opt();
            semicolon_opt();
            depth--;
        }

        void assignment()
        {
            addNode("<assignment>");
            depth++;
            consume("IDN");
            assign_opt();
            semicolon_opt();
            depth--;
        }

        void assign_opt()
        {
            string token = getCurrentToken();
            if (token != "OP_ASSIGN")
            {
                return;
            }

            addNode("<assign_opt>");
            depth++;
            consume("OP_ASSIGN");
            expr();
            depth--;
        }

        void semicolon_opt()
        {
            string token = getCurrentToken();
            if (token == "SC_SEMICOLON")
            {
                addNode("<semicolon_opt>");
                depth++;
                consume("SC_SEMICOLON");
                depth--;
            }
        }

        void if_stmt()
        {
            addNode("<if_stmt>");
            depth++;
            consume("KW_IF");
            consume("L_PARENT");
            expr();
            consume("R_PARENT");
            block();
            else_if_stmt_opt();
            else_stmt_opt();
            depth--;
        }

        void else_if_stmt_opt()
        {
            string token = getCurrentToken();
            if (token != "KW_ELSE" || peekNextToken() != "KW_IF")
            {
                return;
            }
            addNode("<else_if_stmt_opt>");
            depth++;
            consume("KW_ELSE");
            consume("KW_IF");
            consume("L_PARENT");
            expr();
            consume("R_PARENT");
            block();
            else_if_stmt_opt();
            depth--;
        }

        void else_stmt_opt()
        {
            string token = getCurrentToken();
            if (token != "KW_ELSE")
            {
                return;
            }
            addNode("<else_stmt_opt>");
            depth++;
            consume("KW_ELSE");
            block();
            depth--;
        }

        void print_stmt()
        {
            addNode("<print_stmt>");
            depth++;
            consume("KW_PRINT");
            consume("L_PARENT");
            expr();
            consume("R_PARENT");
            semicolon_opt();
            depth--;
        }

        void for_stmt()
        {
            addNode("<for_stmt>");
            depth++;
            consume("KW_FOR");
            consume("L_PARENT");
            iterator_declaration_opt();
            consume("IDN");
            consume("SC_SEMICOLON");
            range_expr();
            consume("SC_SEMICOLON");
            step_expr();
            consume("R_PARENT");
            block();
            depth--;
        }

        void iterator_declaration_opt()
        {
            string token = getCurrentToken();
            if (token != "KW_VAR")
            {
                return;
            }
            addNode("<iterator_declaration_opt>");
            depth++;
            consume("KW_VAR");
            depth--;
        }

        void range_expr()
        {
            addNode("<range_expr>");
            depth++;
            expr();
            consume("SC_COLON");
            expr();
            depth--;
        }

        void step_expr()
        {
            addNode("<step_expr>");
            depth++;
            expr();
            depth--;
        }

        void block()
        {
            addNode("<block>");
            depth++;
            consume("L_BRACE");
            statement_list();
            consume("R_BRACE");
            depth--;
        }

        void expr()
        {
            addNode("<expr>");
            depth++;
            rel_expr();
            depth--;
        }

        void rel_expr()
        {
            addNode("<rel_expr>");
            depth++;
            add_expr();
            rel_expr_tail();
            depth--;
        }

        void rel_expr_tail()
        {
            string token = getCurrentToken();
            if (token != "OP_EQ" && token != "OP_NOT_EQ" && token != "OP_LT" && token != "OP_GT" && token != "OP_LE" && token != "OP_GE")
            {
                return;
            }
            addNode("<rel_expr_tail>");
            depth++;
            if (token == "OP_EQ" || token == "OP_NOT_EQ" || token == "OP_LT" || token == "OP_GT" || token == "OP_LE" || token == "OP_GE")
            {
                rel_op();
                add_expr();
            }
            depth--;
        }

        void add_expr()
        {
            addNode("<add_expr>");
            depth++;
            term();
            add_expr_tail();
            depth--;
        }

        void add_expr_tail()
        {
            string token = getCurrentToken();
            if (token != "OP_PLUS" && token != "OP_MINUS")
            {
                return;
            }
            addNode("<add_expr_tail>");
            depth++;
            if (token == "OP_PLUS")
            {
                consume("OP_PLUS");
            }
            else if (token == "OP_MINUS")
            {
                consume("OP_MINUS");
            }
            term();
            add_expr_tail();
            depth--;
        }

        void term()
        {
            addNode("<term>");
            depth++;
            string token = getCurrentToken();
            if (token == "IDN" || (token == "OP_MINUS" && peekNextToken() == "CONST_INT") || token == "CONST_INT" || token == "CONST_STRING" || token == "CONST_BOOL" || token == "CONST_BOOL" || token == "L_PARENT")
            {
                factor();
                term_tail();
            }
            else
            {
                syntaxError("Unexpected token " + token, this->getLineOfCode());
            }
            depth--;
        }

        void term_tail()
        {
            string token = getCurrentToken();
            if (token != "OP_STAR" && token != "OP_SLASH" && token != "OP_MOD")
            {
                return;
            }
            addNode("<term_tail>");
            depth++;
            if (token == "OP_STAR")
            {
                consume("OP_STAR");
            }
            else if (token == "OP_SLASH")
            {
                consume("OP_SLASH");
            }
            else if (token == "OP_MOD")
            {
                consume("OP_MOD");
            }
            factor();
            term_tail();
            depth--;
        }

        void factor()
        {
            addNode("<factor>");
            depth++;
            string token = getCurrentToken();
            if (token == "IDN" || (token == "OP_MINUS" && peekNextToken() == "CONST_INT") || token == "CONST_INT" || token == "CONST_STRING" || token == "CONST_BOOL" || token == "L_PARENT")
            {
                consume(token);
                if (token == "L_PARENT")
                {
                    expr();
                    consume("R_PARENT");
                }
                else if (token == "OP_MINUS")
                {
                    consume("CONST_INT");
                }
            }
            else
            {
                syntaxError("Unexpected token " + token, this->getLineOfCode());
            }
            depth--;
        }

        void rel_op()
        {
            addNode("<rel_op>");
            depth++;
            string token = getCurrentToken();
            if (token == "OP_EQ" || token == "OP_NOT_EQ" || token == "OP_LT" || token == "OP_GT" || token == "OP_LE" || token == "OP_GE")
            {
                consume(token);
            }
            else
            {
                syntaxError("Unexpected token " + token, this->getLineOfCode());
            }
            depth--;
        }

        void syntaxError(string message, string line)
        {
            cerr << "Syntax error at line (" << line << "):\n\t" << message << endl;
            exit(1);
        }

        void consume(string expectedToken)
        {
            string token = getCurrentToken();
            if (token == expectedToken)
            {
                addNode(currentToken);
                currentToken = this->nextToken();
            }
            else
            {
                syntaxError("Expected " + expectedToken + ", but found " + token + "!", this->getLineOfCode());
            }
        }

    public:
        Parser(vector<string> classifiedTokens)
        {
            this->classifiedTokens = classifiedTokens;
            this->currentTokenIndex = 0;
            this->currentToken = this->nextToken();
        }

        void printAST(string fileName)
        {
            ofstream file(fileName);
            for (ASTNode node : AST)
            {
                file << node.getNodeWithDepth() << endl;
            }
        }

        void start()
        {
            this->parse();
        }

        vector<ASTNode> getAST()
        {
            return this->AST;
        }
    };

    struct VariableInfo
    {
        enum Type
        {
            INT,
            STRING,
            BOOL,
            UNDEFINED
        } type;
        string value;
        int declaredAtLine;
    };

    class SemanticAnalyzer
    {
    private:
        vector<ASTNode> AST;
        stack<map<string, VariableInfo>> scopeSymbolTables;
        vector<string> semanticErrors;

        void addError(string error)
        {
            semanticErrors.push_back(error);
        }

        bool isDeclared(string variableName)
        {
            auto tempStack = scopeSymbolTables;
            while (!tempStack.empty())
            {
                auto table = tempStack.top();
                tempStack.pop();

                if (table.find(variableName) != table.end())
                {
                    return true;
                }
            }
            return false;
        }

        int getLineOfDeclaration(string variableName)
        {
            auto tempStack = scopeSymbolTables;
            while (!tempStack.empty())
            {
                auto table = tempStack.top();
                tempStack.pop();

                if (table.find(variableName) != table.end())
                {
                    return table[variableName].declaredAtLine;
                }
            }
            return -1;
        }

        void analyze()
        {
            string state = "";
            scopeSymbolTables.push({});

            bool enteredForStatement = false;
            VariableInfo iteratorVariableInfo{VariableInfo::UNDEFINED, "", 0};
            string iteratorVariable = "";
            for (int i = 0; i < AST.size(); i++)
            {
                string node = AST[i].getNode();
                if (AST[i].isTokenType("<for_stmt>"))
                {
                    enteredForStatement = true;
                    state = "assign";
                }
                else if (AST[i].isTokenType("<block>"))
                {
                    scopeSymbolTables.push({});
                    if (enteredForStatement)
                    {
                        if (!iteratorVariable.empty())
                        {
                            scopeSymbolTables.top()[iteratorVariable] = iteratorVariableInfo;
                        }
                        enteredForStatement = false;
                    }
                }
                else if (AST[i].isTokenType("R_BRACE"))
                {
                    scopeSymbolTables.pop();
                }

                else if (AST[i].isTokenType("<assignment>"))
                {
                    state = "assign";
                }
                else if (AST[i].isTokenType("<declaration>") || AST[i].isTokenType("<iterator_declaration_opt>"))
                {
                    state = "declare";
                }
                else if (AST[i].isTokenType("<factor>"))
                {
                    state = "use";
                }

                else if (AST[i].isTokenType("IDN"))
                {
                    if (state == "declare")
                    {
                        string variableName = AST[i].getTokenValue();
                        int lineOfCode = AST[i].getLineInCode();
                        // If variable is already declared -> Redeclaration error!
                        if (isDeclared(variableName))
                        {
                            addError("Error (line " + to_string(lineOfCode) + "): Variable '" + variableName + "' already declared at line " + to_string(getLineOfDeclaration(variableName)) + ".");
                        }
                        else
                        {
                            if (enteredForStatement)
                            {
                                iteratorVariable = variableName;
                                iteratorVariableInfo.declaredAtLine = lineOfCode;
                            }
                            else
                            {
                                scopeSymbolTables.top()[variableName].type = VariableInfo::UNDEFINED;
                                scopeSymbolTables.top()[variableName].declaredAtLine = lineOfCode;
                            }
                        }
                    }
                    else if (state == "assign")
                    {
                        string variableName = AST[i].getTokenValue();
                        int lineOfCode = AST[i].getLineInCode();
                        // If variable is not declared -> Declaration error!
                        if (!isDeclared(variableName))
                        {
                            addError("Error (line " + to_string(lineOfCode) + "): Cannot assign value to variable '" + variableName + "' because it hasn't been declared yet.");
                        }
                    }
                    else if (state == "use")
                    {
                        string variableName = AST[i].getTokenValue();
                        int lineOfCode = AST[i].getLineInCode();
                        // If variable is not declared -> Declaration error!
                        if (!isDeclared(variableName))
                        {
                            addError("Error (line " + to_string(lineOfCode) + "): Cannot use variable '" + variableName + "' because it hasn't been declared yet.");
                        }
                    }
                }
            }
        }

        // For checking the scope
        void printScopeSymbolTables()
        {
            int scopeLevel = scopeSymbolTables.size();
            auto tempTables = scopeSymbolTables;

            while (!tempTables.empty())
            {
                auto table = tempTables.top();
                cout << "Scope level " << scopeLevel-- << ":\n";

                for (auto variable : table)
                {
                    cout << "  " << variable.first << " = " << variable.second.value << " ";

                    switch (variable.second.type)
                    {
                    case VariableInfo::INT:
                        cout << "(int)";
                        break;
                    case VariableInfo::STRING:
                        cout << "(string)";
                        break;
                    case VariableInfo::BOOL:
                        cout << "(bool)";
                        break;
                    case VariableInfo::UNDEFINED:
                        cout << "(undefined)";
                        break;
                    default:
                        break;
                    }

                    cout << " (declared at line " << variable.second.declaredAtLine << ")\n";
                }

                cout << endl;
                tempTables.pop();
            }
        }

    public:
        SemanticAnalyzer(vector<ASTNode> AST)
        {
            this->AST = AST;
        }

        void start()
        {
            this->analyze();
            if (!semanticErrors.empty())
            {
                for (string error : semanticErrors)
                {
                    cerr << error << endl;
                }
                exit(1);
            }
        }
    };

    class Interpreter
    {
    private:
        vector<ASTNode> AST;
        vector<map<string, VariableInfo>> scopeSymbolTables;
        int currentIndex;
        const string iteratorTail = "_it_SimpleLang_Compiler_";

        VariableInfo *findVariableRef(string variableName)
        {
            for (int i = scopeSymbolTables.size() - 1; i >= 0; --i)
            {
                auto &currentScope = scopeSymbolTables[i];

                auto it = currentScope.find(variableName);
                if (it != currentScope.end())
                {
                    return &(it->second); // return reference on VariableInfo
                }
            }

            return nullptr;
        }

        VariableInfo evaluateFactor()
        {
            currentIndex++;
            if (AST[currentIndex].isTokenType("IDN"))
            {
                return *findVariableRef(AST[currentIndex++].getTokenValue()); // dereference the VariableInfo ptr
            }
            // For negative numbers!
            else if (AST[currentIndex].isTokenType("OP_MINUS"))
            {
                goToNextSibling("CONST_INT");
                return VariableInfo{VariableInfo::INT, to_string(-1 * stoi(AST[currentIndex++].getTokenValue()))};
            }
            else if (AST[currentIndex].isTokenType("CONST_INT"))
            {
                return VariableInfo{VariableInfo::INT, AST[currentIndex++].getTokenValue()};
            }
            else if (AST[currentIndex].isTokenType("CONST_STRING"))
            {
                return VariableInfo{VariableInfo::STRING, AST[currentIndex++].getTokenValue()};
            }
            else if (AST[currentIndex].isTokenType("CONST_BOOL"))
            {
                return VariableInfo{VariableInfo::BOOL, AST[currentIndex++].getTokenValue()};
            }
            else if (AST[currentIndex].isTokenType("L_PARENT"))
            {
                currentIndex++; // skip L_PARENT
                VariableInfo result = evaluateExpression();
                currentIndex++; // skip R_PARENT

                return result;
            }
            else
            {
                cerr << "ERROR: Cannot evaluate expression at line: " << AST[currentIndex].getLineInCode() << endl;
                exit(1);
            }
        }

        VariableInfo evaluateTerm()
        {
            currentIndex++;
            VariableInfo left = evaluateFactor();

            while (currentIndex < AST.size() && AST[currentIndex].isTokenType("<term_tail>"))
            {
                string op = AST[++currentIndex].getTokenType(); // First: currentIndex++ --> OP_STAR or OP_SLASH or OP_MOD
                currentIndex++;
                VariableInfo right = evaluateFactor();

                if (left.type == VariableInfo::INT && right.type == VariableInfo::INT)
                {
                    if (op == "OP_STAR")
                    {
                        left.value = to_string(stoi(left.value) * stoi(right.value));
                    }
                    else if (op == "OP_SLASH")
                    {
                        left.value = to_string(stoi(left.value) / stoi(right.value));
                    }
                    else if (op == "OP_MOD")
                    {
                        left.value = to_string(stoi(left.value) % stoi(right.value));
                    }
                    else
                    {
                        cerr << "ERROR: Cannot use arithmetic operator here!" << endl;
                        exit(1);
                    }
                }
                else if ((left.type == VariableInfo::STRING && right.type == VariableInfo::INT || right.type == VariableInfo::STRING && left.type == VariableInfo::INT) && op == "OP_STAR")
                {
                    string result = "";
                    int times;
                    string str;
                    if (left.type == VariableInfo::INT)
                    {
                        times = stoi(left.value);
                        str = right.value;
                        str = str.substr(1, str.find_last_of('"') - 1);
                    }
                    else if (right.type == VariableInfo::INT)
                    {
                        times = stoi(right.value);
                        str = left.value;
                        str = str.substr(1, str.find_last_of('"') - 1);
                    }
                    for (int i = 0; i < times; i++)
                    {
                        result += str;
                    }

                    result = "\"" + result + "\"";
                    left.value = result;
                }
                else
                {
                    cerr << "ERROR: Unknown operation!" << endl;
                    exit(1);
                }
            }

            return left;
        }

        VariableInfo evaluateAddExpr()
        {
            currentIndex++;
            VariableInfo left = evaluateTerm();

            while (currentIndex < AST.size() && AST[currentIndex].isTokenType("<add_expr_tail>"))
            {
                string op = AST[++currentIndex].getTokenType(); // First: currentIndex++ --> OP_PLUS or OP_MINUS
                currentIndex++;
                VariableInfo right = evaluateTerm();

                if (left.type == VariableInfo::INT && right.type == VariableInfo::INT)
                {
                    if (op == "OP_PLUS")
                    {
                        left.value = to_string(stoi(left.value) + stoi(right.value));
                    }
                    else if (op == "OP_MINUS")
                    {
                        left.value = to_string(stoi(left.value) - stoi(right.value));
                    }
                    else
                    {
                        cerr << "ERROR: Cannot use arithmetic operator here!" << endl;
                        exit(1);
                    }
                }
                else if ((left.type == VariableInfo::STRING || right.type == VariableInfo::STRING) && op == "OP_PLUS")
                {
                    string str1 = left.value;
                    str1 = str1.substr(0, str1.find_last_of('"'));
                    string str2 = right.value;
                    str2 = str2.substr(1);
                    left.value = str1 + str2;
                }
                else
                {
                    cerr << "ERROR: Unknown operation!" << endl;
                    exit(1);
                }
            }

            return left;
        }

        VariableInfo evaluateRelExpr()
        {
            currentIndex++; // -> <add_expr>
            VariableInfo left = evaluateAddExpr();

            if (currentIndex < AST.size() && AST[currentIndex].isTokenType("<rel_expr_tail>"))
            {
                currentIndex++; // -> <rel_op>
                string op = AST[++currentIndex].getTokenType();
                currentIndex++;
                VariableInfo right = evaluateAddExpr();

                VariableInfo result{VariableInfo::UNDEFINED, ""};

                if (op == "OP_EQ")
                {
                    result.value = stoi(left.value) == stoi(right.value) ? "true" : "false";
                    return result;
                }
                else if (op == "OP_NOT_EQ")
                {
                    result.value = stoi(left.value) != stoi(right.value) ? "true" : "false";
                    return result;
                }
                else if (op == "OP_LT")
                {
                    result.value = stoi(left.value) < stoi(right.value) ? "true" : "false";
                    return result;
                }
                else if (op == "OP_GT")
                {
                    result.value = stoi(left.value) > stoi(right.value) ? "true" : "false";
                    return result;
                }
                else if (op == "OP_LE")
                {
                    result.value = stoi(left.value) <= stoi(right.value) ? "true" : "false";
                    return result;
                }
                else if (op == "OP_GE")
                {
                    result.value = stoi(left.value) >= stoi(right.value) ? "true" : "false";
                    return result;
                }
                else
                {
                    cerr << "ERROR: Unknown operation!" << endl;
                    exit(1);
                }
            }

            return left;
        }

        VariableInfo evaluateExpression()
        {
            currentIndex++; // -> <rel_expr>
            return evaluateRelExpr();
        }

        void interpretPrintStatement()
        {
            int currentDepth = AST[currentIndex].getDepth();
            currentIndex++;
            VariableInfo evaluatedExpr{VariableInfo::UNDEFINED, "", 0};
            while (currentIndex < AST.size() && AST[currentIndex].getDepth() > currentDepth)
            {
                if (AST[currentIndex].isTokenType("<expr>"))
                {
                    evaluatedExpr = evaluateExpression();
                    break;
                }
                currentIndex++;
            }
            cout << "\t> " << evaluatedExpr.value << endl;
        }

        void goToNextSibling(string tokenType)
        {
            int currentDepth = AST[currentIndex].getDepth();
            while (currentIndex < AST.size() && !AST[currentIndex].isTokenType(tokenType) && AST[currentIndex].getDepth() >= currentDepth)
            {
                currentIndex++;
            }
        }

        void skipBlock()
        {
            int currentDepth = AST[currentIndex].getDepth();
            currentIndex++;
            while (AST[currentIndex].getDepth() > currentDepth)
            {
                currentIndex++;
            }
        }

        void interpretIfStatement()
        {
            int currentDepth = AST[currentIndex].getDepth();
            currentIndex++;
            bool executeIfStatement = false;
            bool blockExecuted = false;
            while (currentIndex < AST.size() && AST[currentIndex].getDepth() > currentDepth)
            {
                if (AST[currentIndex].isTokenType("<expr>"))
                {
                    executeIfStatement = evaluateExpression().value == "true";
                    currentIndex++;
                    break;
                }
                currentIndex++;
            }

            // If executeIfStatement = true
            if (executeIfStatement)
            {
                // Execute that block
                interpretBlock();
                blockExecuted = true;
            }
            else
            {
                // Skip that block
                skipBlock();
            }

            currentDepth = AST[currentIndex].getDepth();
            // ELSE IF part
            while (currentIndex < AST.size() && AST[currentIndex].isTokenType("<else_if_stmt_opt>") && AST[currentIndex].getDepth() >= currentDepth)
            {
                currentIndex += 4;
                if (!blockExecuted && evaluateExpression().value == "true")
                {
                    currentIndex++;
                    interpretBlock();
                    blockExecuted = true;
                }
                else
                {
                    // Go to <block> => because we are at <expr> now!
                    goToNextSibling("<block>");
                    skipBlock();
                }
            }

            // ELSE part
            currentDepth = AST[currentIndex].getDepth();
            currentIndex++;
            // Find <block> part
            while (currentIndex < AST.size() && AST[currentIndex].getDepth() > currentDepth)
            {
                if (AST[currentIndex].isTokenType("<block>"))
                {
                    break;
                }
                currentIndex++;
            }

            if (!blockExecuted)
            {
                interpretBlock();
            }
            else
            {
                skipBlock();
            }
        }

        void interpretForStatementBody()
        {

            int currentDepth = AST[currentIndex].getDepth();
            currentIndex++;
            while (currentIndex < AST.size() && AST[currentIndex].getDepth() > currentDepth)
            {
                interpretStatements();
            }
        }

        void interpretForStatement()
        {
            int currentDepth = AST[currentIndex].getDepth();
            currentIndex++;
            bool declareIterator = false;

            while (currentIndex < AST.size() && AST[currentIndex].getDepth() > currentDepth)
            {
                if (AST[currentIndex].isTokenType("<iterator_declaration_opt>"))
                {
                    declareIterator = true;
                    goToNextSibling("IDN");
                    break;
                }
                else if (AST[currentIndex].isTokenType("IDN"))
                {
                    break;
                }
                currentIndex++;
            }

            // Get iterator
            string iteratorVariable = AST[currentIndex].getTokenValue();
            VariableInfo iteratorVariableInfo{VariableInfo::UNDEFINED, ""};

            goToNextSibling("<range_expr>");
            currentIndex++;
            VariableInfo beginInfo = evaluateExpression();
            string begin = beginInfo.value;
            iteratorVariableInfo.value = begin;
            iteratorVariableInfo.type = beginInfo.type;

            currentIndex++; // skip SC_COLON
            VariableInfo endInfo = evaluateExpression();
            string end = endInfo.value;
            goToNextSibling("<step_expr>");
            currentIndex++;
            VariableInfo stepInfo = evaluateExpression();
            string step = stepInfo.value;

            map<string, VariableInfo> bodyScope;
            if (declareIterator)
            {
                bodyScope.insert({iteratorVariable, iteratorVariableInfo});
            }
            else
            {
                VariableInfo *temp = findVariableRef(iteratorVariable);
                temp->value = begin;
            }
            scopeSymbolTables.push_back(bodyScope);

            goToNextSibling("<block>");
            int bodyIndex = currentIndex;

            for (int i = stoi(begin); i < stoi(end); i += stoi(step))
            {
                currentIndex = bodyIndex;
                interpretForStatementBody();
                VariableInfo *temp = findVariableRef(iteratorVariable);
                temp->value = to_string(stoi(temp->value) + stoi(step));
            }
            scopeSymbolTables.pop_back();
        }

        void interpretAssignment()
        {
            int currentDepth = AST[currentIndex].getDepth();
            currentIndex++;
            ASTNode variableToAssign;
            VariableInfo variableToAssignInfo{VariableInfo::UNDEFINED, "", 0};
            VariableInfo evaluatedExpr{VariableInfo::UNDEFINED, "", 0};
            while (currentIndex < AST.size() && AST[currentIndex].getDepth() > currentDepth)
            {
                if (AST[currentIndex].isTokenType("IDN"))
                {
                    // Save which to variable the value must be assigned
                    if (variableToAssign.empty())
                    {
                        variableToAssign = AST[currentIndex];
                    }
                }
                else if (AST[currentIndex].isTokenType("<expr>"))
                {
                    // Get evaluated expression and assign all values
                    evaluatedExpr = evaluateExpression();
                    variableToAssignInfo.type = evaluatedExpr.type;
                    variableToAssignInfo.value = evaluatedExpr.value;
                    // declaredAtLine is already written in scopeSymbolTables -> just get the data
                    variableToAssignInfo.declaredAtLine = scopeSymbolTables.back()[variableToAssign.getTokenValue()].declaredAtLine;

                    // Skip <semicolon_opt> & SC_SEMICOLON ;
                    currentIndex += 2;
                    break;
                }
                currentIndex++;
            }

            scopeSymbolTables.back()[variableToAssign.getTokenValue()] = variableToAssignInfo;
        }

        void interpretDeclaration()
        {
            int currentDepth = AST[currentIndex].getDepth();
            currentIndex++;
            string variableName = "";
            ASTNode variableToDeclare;
            VariableInfo variableToDeclareInfo{VariableInfo::UNDEFINED, "", 0};
            VariableInfo evaluatedExpr{VariableInfo::UNDEFINED, "", 0};
            while (currentIndex < AST.size() && AST[currentIndex].getDepth() > currentDepth)
            {
                if (AST[currentIndex].isTokenType("IDN"))
                {
                    // Save which variable has to be declared
                    if (variableToDeclare.empty())
                    {
                        variableToDeclare = AST[currentIndex];
                        variableToDeclareInfo.declaredAtLine = variableToDeclare.getLineInCode();
                    }
                }
                else if (AST[currentIndex].isTokenType("<expr>"))
                {
                    evaluatedExpr = evaluateExpression();
                    variableToDeclareInfo.type = evaluatedExpr.type;
                    variableToDeclareInfo.value = evaluatedExpr.value;

                    // Skip <semicolon_opt> & SC_SEMICOLON ;
                    currentIndex += 2;
                    break;
                }
                currentIndex++;
            }
            scopeSymbolTables.back()[variableToDeclare.getTokenValue()] = variableToDeclareInfo;
        }

        void enterScope()
        {
            map<string, VariableInfo> newScope;
            scopeSymbolTables.push_back(newScope);
        }

        void exitScope()
        {
            scopeSymbolTables.pop_back();
        }

        void interpretBlock()
        {
            enterScope();
            int currentDepth = AST[currentIndex].getDepth();
            currentIndex++;
            while (currentIndex < AST.size() && AST[currentIndex].getDepth() > currentDepth)
            {
                interpretStatements();
            }

            exitScope();
        }

        void interpretStatements()
        {
            int currentDepth = AST[currentIndex].getDepth();
            currentIndex++;
            while (currentIndex < AST.size() && AST[currentIndex].getDepth() > currentDepth)
            {
                if (AST[currentIndex].isTokenType("<print_stmt>"))
                {
                    interpretPrintStatement();
                    currentIndex--;
                }
                else if (AST[currentIndex].isTokenType("<declaration>"))
                {
                    interpretDeclaration();
                    currentIndex--;
                }
                else if (AST[currentIndex].isTokenType("<assignment>"))
                {
                    interpretAssignment();
                    currentIndex--;
                }
                else if (AST[currentIndex].isTokenType("<if_stmt>"))
                {
                    interpretIfStatement();
                    currentIndex--;
                }
                else if (AST[currentIndex].isTokenType("<for_stmt>"))
                {
                    interpretForStatement();
                    currentIndex--;
                }
                else if (AST[currentIndex].isTokenType("<block>"))
                {
                    interpretBlock();
                    currentIndex--;
                }
                currentIndex++;
            }
        }

        void interpretProgram()
        {
            enterScope();
            int currentDepth = AST[currentIndex].getDepth();
            currentIndex++;
            while (currentIndex < AST.size() && AST[currentIndex].getDepth() > currentDepth)
            {
                if (AST[currentIndex].isTokenType("<statement_list>"))
                {
                    interpretStatements();
                    currentIndex--;
                }
                currentIndex++;
            }
            exitScope();
        }

        void printScopeSymbolTables()
        {
            int scopeLevel = scopeSymbolTables.size();
            auto tempTables = scopeSymbolTables;

            while (!tempTables.empty())
            {
                auto table = tempTables.back();
                cout << "Scope level " << scopeLevel-- << ":\n";

                for (auto variable : table)
                {
                    cout << "  " << variable.first << " = " << variable.second.value << " ";

                    switch (variable.second.type)
                    {
                    case VariableInfo::INT:
                        cout << "(int)";
                        break;
                    case VariableInfo::STRING:
                        cout << "(string)";
                        break;
                    case VariableInfo::BOOL:
                        cout << "(bool)";
                        break;
                    case VariableInfo::UNDEFINED:
                        cout << "(undefined)";
                        break;
                    default:
                        break;
                    }

                    cout << " (declared at line " << variable.second.declaredAtLine << ")\n";
                }

                cout << endl;
                tempTables.pop_back();
            }
        }

    public:
        Interpreter(vector<ASTNode> AST)
        {
            this->AST = AST;
            this->currentIndex = 0;
        }

        void start()
        {
            this->interpretProgram();
        }
    };

public:
    SimpleLang(string fileName)
    {
        this->fileName = fileName;
    }

    void run()
    {
        // Read program - line by line
        this->readFile();
        // Lexer - making tokens from program
        Lexer lexer(this->linesOfCode);
        lexer.start();
        vector<string> classifiedTokens = lexer.getClassifiedTokens();

        // Parser - building AST
        Parser parser(classifiedTokens);
        parser.start();
        parser.printAST("ast.txt"); // print AST in file
        vector<ASTNode> AST = parser.getAST();

        // SemanticAnalyzer
        SemanticAnalyzer analyzer(AST);
        analyzer.start();

        // Interpreter
        Interpreter interpreter(AST);
        interpreter.start();
    }
};

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cout << "USE: " << argv[0] << " <file name>" << endl;
    }

    // Read program written in .sl and run it!
    SimpleLang myProgram(argv[1]);
    myProgram.run();
    return 0;
}