# Self-hosting C compiler

CC3 is a work-in-progress compiler from C99 (ISO/IEC 9899:1999) to AMD64.
It can compile itself using an external pre-processor.

Its current state is a result of ~3 weeks of work, the main goal in this period
was compiling itself, as a result there are many TODOs:
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
- Statement expressions (used by glibc)
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

    The lexical analyzer reads input from a file descriptor and does its own
    buffering to allow cheap forward peeking. Its interface is a single step
    iterator, it produces tokens using an output parameter.

- parse.c: Recursive-descent parser (hand written)

    The parser is a relatively clean, textbook, predictive recursive descent
    parser. It uses an internal FIFO to allow for two tokens of lookahead.
    If we ignore its entanglement with context information required to resolve
    typedef-name ambiguities, it is essentially an LL(2) parser.

    Expressions and statements produce an AST. However scoping is done by the
    semantic analyzer, thus nested block statements are not represented in it.

    Declarations need semantic information to disambiguate typedef names from
    idenitifiers. Additionally the syntax of C declarators is quite challanges
    to parse in one pass, thus a temporary linked-list representing a declarator
    is produced. Then it's walked in a second pass to construct the final type
    of a declaration.

- sema.c: Semantic analyzer

    The semantic analyzer handles, scoping, declarations, and type checking.

    Scoping and declarations are handled through callbacks from the parser.

    Type checking is done through providing constructors for expressions,
    statements, and initializers. These type check their arguments, then return
    the constructed object containing these arguments.

- gen.c: Code generator

    Finally the code generator takes the AST constructed by the parser,
    and walks it using a set of recursive procedures. Different procedures
    are used based on what context expressions appear in, lvalue-, rvalue-,
    and boolean expressions all generate different code.

    It generates output treating AMD64 as a simplistic stack machine, tree
    nodes are matched one-to-one with instruction templates. This makes the
    code generator quite straightforward, however the generated code can be
    orders of a magnitude slower then a modern optimizing compiler.

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
