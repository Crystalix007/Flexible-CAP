# Flexible-CAP

Flexible-CAP is a flexible Commandline Argument Parser.

Using a custom argument specification language, it will generate a corresponding lexer and parser in Flex and Bison respectively.

This can then be compiled with the other source files to generate a program using the commandline interface specified.

# Requirements

* `flex`
* `bison`
* [`mstch`](https://github.com/no1msd/mstch)
* `g++`

# Compiling

First build the generator.

If you have `tup` installed:

```sh
tup init
tup
```

If you do not, on Linux:

```sh
./build.sh
```

# Usage

Once the generator has been built, write an `ArgSpec` file specifying the interface of your program.

Then, produce the interface files with:

```sh
/path/to/cloned/repo/generator/fcap /path/to/argument-specification.argspec
```

This will produce the required files in the directory the command is run from.

There should be 6 files produced:

```sh
${argspec}ArgGrammarDriver.cpp
${argspec}ArgGrammarDriver.hpp
${argspec}ArgGrammarParser.yy
${argspec}ArgGrammarScanner.cpp
${argspec}ArgGrammarScanner.hpp
${argspec}ArgGrammarScanner.ll
```

Here, the `${argspec}` denotes the name of the `.argspec` file without the extension.

Next, run `bison` on the `${argspec}ArgGrammarParser.yy` file, and run `flex` on the `${argspec}ArgGrammarParser.yy`.

Then, compile and link all the `.cc` and `.cpp` files together with your application.
