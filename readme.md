# Self-hosting C compiler

CC3 is a work-in-progress compiler from C99 (ISO/IEC 9899:1999) to AMD64.
It can compile itself using an external pre-processor.

It's current state is a result of ~3 weeks of work, with the main goal of
compiling itself. There are many TODOs:
- Integrated pre-processor
- Any support for floating point types
- Type checking other then where strictly necessary for semantics
- Passing structure and union parameters by value
- Returning structures and unions by value
- Stack arguments, functions are limited to 6 parameters
- Designated initializers
- Compound literals
- Tentative definitions of statics/externs
- Memory management, free is never called
- Typedef name ambiguity handled incorrectly in parameter lists
- Missing block scopes for selection and iteration statements (except for)
- K&R function declarations and definitions (omitted on purpose, not a TODO)
- Variable length arrays (omitted on purpose, not a TODO)

For compatibility with glibc's headers and general ergonomics, the following
GNU extensions are supported on top of standard:
- Statement expression (used by glibc)
- Alternative spelling of inline as __inline (used by glibc)
- Anonymous structs/unions (used by glibc and the compiler itself)
- Case ranges in switches (used by the compiler itself)

# Design

I tried to keep the separation of concerns and the interface between the
modules relatively clean. There are a few hacky interactions, and data in
inappropriate places, but these are clearly labeled and fixing them is a
future goal.

The compiler itself has 4 main modules:
- lex.c: Lexical analyzer (hand written)
- parse.c: Recursive-descent parser (hand written)
- sema.c: Semantic analyzer
- gen.c: Code generator

The lexical analyzer reads input using a C FILE I/O handle and does it's own
buffering to allow cheap forward peeking. It's interface provides a view of the
next two tokens of output, that can be advanced forward one token at a time.

The parser is a relatively clean, textbook, predictive recursive descent parser.
It uses the interface above to choose productions based on the two tokens of
look-ahead. When the appropriate production is determined it calls the function
corresponding to the chosen production. The parsing of expressions and
statements roughly matches the standard grammar. Declarations and declarators
less so, the functions parsing these are quite hacky and intertwined with
semantic analysis, this is required as C is everything but a clean context free
language in this regard. The parser's output is two-fold, for scoping and
declarations it hands off to the semantic analyzer. For expressions and
statements it constructs an AST with the help of the semantic analyzer for
type checking.

The semantic analyzer handles, scoping, declarations, and type checking.
Scoping and declarations are handled through callbacks from the parser.
Type checking is done through providing constructors for expressions,
statements, and initializers. These type check their arguments, then return the
constructed object containing these arguments. Since scoping is done by the
semantic analyzer, the parser flattens block scopes and these are not
represented in the AST of statements.

Finally the code generator takes the AST constructed by the parser, and walks
it using a set of recursive procedures. Different procedures are used based on
what context certain things appear, e.g. lvalue expression, boolean expression,
and rvalue expressions need different code. It generates output treating AMD64
as a simplistic stack machine, and tree nodes are matched with instruction
templates pretty much 1-to-1. The generated code is quite awful and a lot slower
then an optimizing compiler.

# Future plans

This is a personal project with the purpose of entertainment, and/or to serve
as platform for language experiments.

Right now the the main goals are:
- Clear the list of TODOs above
- Reduce hackiness of module interactions
- Use more efficient data structures
- Improve the quality of generated code

# Copying

Everything in this repository is Copyright (C) Mate Kukri, 2022 and is licensed
under version 2 (only) of the GNU GPL.
