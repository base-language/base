=========================================
BASE LANGUAGE SYNTAX SPECIFICATION
=========================================

Author: randseas  
Version: 1.0  
Date: August 2025

-----------------------------------------
1. Comments
-----------------------------------------
Single-line comment:
  // This is a comment

Multi-line comment:
  /*
    This is a multi-line
    comment block
  */

-----------------------------------------
2. Variable Declarations
-----------------------------------------
Syntax:
  <variable_decl> ::= ("let" | "const") <type> <identifier> "=" <expression> ";"

Rules:
- `const`: read-only, static, cannot be reassigned.
- `let`: read-write, dynamic, can be reassigned.
- If a type is specified, the value must match it.

Examples:
  const string name = "Mark";
  let number age = 18;

-----------------------------------------
3. Function Declarations
-----------------------------------------
Syntax:
  <function_decl> ::= "function" <type> <identifier> "(" <parameters>? ")" <block>

<type> ::= "number" | "string" | "void"
<parameters> ::= <parameter> ("," <parameter>)*  
<parameter> ::= <type> <identifier>  
<block> ::= "{" <statement>* "}"

Rules:
- `function`: declares a new function.
- The return type must be specified.
- `void`: means the function does **not return a value**.
- Parameters follow the format: `<type> <name>`, separated by commas.

Examples:
  function number add(number a, number b) {
    return a + b;
  }

  function void logMessage(string message) {
    print(message);
  }

-----------------------------------------
4. Built-in print Function
-----------------------------------------
Usage:
  print(<string>);

String Types:
- **Double quotes** (`"..."`)  
  - Does NOT support `${}` interpolation  
  - Does NOT support multi-line text  
- **Backticks** (`` `...` ``)  
  - Supports `${}` variable interpolation  
  - Supports multi-line strings

Valid:
  print("Hello!");
  print(`Hello ${name}`);

Invalid:
  print("
    Multiline
  ");      // ❌ Invalid multi-line with double quotes
  print("${name}");  // ❌ Invalid interpolation with double quotes

-----------------------------------------
5. Expressions
-----------------------------------------
<expression> ::= <literal>
               | <identifier>
               | <binary_operation>
               | <function_call>

<literal> ::= <number> | <string>
<function_call> ::= <identifier> "(" <arguments>? ")"
<arguments> ::= <expression> ("," <expression>)*

<binary_operation> ::= <expression> ("+" | "-" | "*" | "/") <expression>

Examples:
  5 + 3
  name + " " + surname
  greet(name, surname, age)

-----------------------------------------
6. Types
-----------------------------------------
Supported types:

- `number`: numeric values (e.g. `14`, `3.14`)
- `string`: `"text"` or `` `text` ``

Planned (not yet implemented):
- `boolean`
- `array`
- `object`

-----------------------------------------
7. Identifiers
-----------------------------------------
Identifiers must start with a letter or underscore, and may contain letters, numbers, or underscores.

Syntax:
  <identifier> ::= /[a-zA-Z_][a-zA-Z0-9_]*/

Valid:
  name, surname, _age, user1

-----------------------------------------
8. Sample Code
-----------------------------------------
const string name = "Mark";
const string surname = "Rober";
let number age = 14;

function number greet(string name, string surname, number age) {
  print(`Hello ${name} ${surname}, you are ${age} years old!`);
}

greet(name, surname, age);

-----------------------------------------
End of Specification
-----------------------------------------
