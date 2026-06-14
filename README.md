# SimpleLang

**SimpleLang** is a minimal educational programming language built to explore and understand the internals of programming languages — including lexing, parsing, semantic analysis, and interpretation. It uses a **block-based structure** to represent the Abstract Syntax Tree (AST), where each node and sub-node is explicitly defined using tags.

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

Create `.sl` file and write your own code.
Once you are finished with writing code in SimpleLang, you can execute it with command:

```
./simplelang <file_name>.sl
```

## SimpleLang Syntax

### Program structure

- A program consists of zero or more statements.
- Statements can be:
  - Variable declarations
  - Assignments
  - `if` statements (with optional `else if` / `else`)
  - `for` loops
  - `print` statements
  - code blocks (`{}`)

### Variable declaration

```
var x;
var y = 10;
```

- Use `var` to declare a variable.
- An assigment (`=`) is **optional** during declaration.
- A semicolon (`;`) is **optional** at the end.

### Assignment

```
x = 5;
y;
```

- An assignment sets a value to a variable using `=`.
- An dentifier without assignment (e.g., `y;`) is also valid.
- A semicolon (`;`) is **optional** at the end.

### Print statement

```
print("Hello, world");
print(x + 1);
```

- Use `print(...)` to output values.
- Accepts any valid expression inside parentheses.
- A semicolon (`;`) is **optional**.

### If / Else if / Else statement

```
if (x > 0) {
    print("Positive");
} else if (x < 0) {
    print("Negative");
} else {
    print("Zero");
}
```

- `if` requires a condition inside parentheses.
- The body must be enclosed in `{}`.
- Multiple `else if` branches are allowed.
- `else` is **optional** and must also use `{}`.

### For loop

```
for (var i; 0:10; i + 1) {
    print(i);
}

for (j; 5:0; j - 1) {
    print(j);
}
```

- `var` is optional — you can declare the iterator or use an existing one.
- The range is written as `start : end`.
- Step is a regular expression that modifies the loop variable.

### Block

```
{
    print("Inside a block");
    x = x + 1;
}
```

- Blocks group multiple statements using `{ ... }`.
- Used in `if`, `else`, `for`, or as standalone.

### Expression

- Expressions can contain:
  - Identifiers (`x`)
  - Constants (integers, strings, booleans)
  - Arithmetic: `+`, `-`, `*`, `/`, `%`
  - Relational: `==`, `!=`, `<`, `>`, `<=`, `>=`
  - Parentheses can be used for grouping: `(x + 2)`

- Example: `(x + 5) * 2 >= y - 3`

### Constants

```
123        // Integer
"hello"    // String
true       // Boolean
false      // Boolean
-42        // Negative integer
```

- You can use optional minus sign `-` before integer constants.

Happy coding! ;)
