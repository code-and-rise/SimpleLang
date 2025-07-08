# SimpleLang
**SimpleLang** is a minimal educational programming language built to explore and understand the internals of programming languages — including lexing, parsing, semantic analysis, and interpretation. It uses a **block-based structure** to represent the Abstract Syntax Tree (AST), where each node and sub-node is explicitly defined using tags.

## SimpleLang Syntax
**Program structure**
- A program consists of zero or more statements.
- Statements can be:
    - Variable declarations
    - Assignments
    - ```if``` statements (with optional ```else if``` / ```else```)
    


## Installation
Clone SimpleLang directory:
```
git clone https://github.com/code-and-rise/SimpleLang.git
```
Go to SimpleLang directory:
```
cd SimpleLang
```
Run command
```
g++ -o simplelang main.cpp
```
Create ```.sl``` file and write your own code.
Once you are finished with writing code in SimpleLang, you can execute it with command:
```
./simplelang <file_name>.sl
```