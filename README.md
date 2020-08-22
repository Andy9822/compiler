# flex-bison based Compiler

Proposed and developed in UFRGS' Compilers course.
You can clone or download this repository freely.


### Prerequisites

TLC is a flex-bison based compiler. So, you will need to install them.
You can use
```
apt-get install flex
```
and
```
apt-get install bison
```
It also uses programming language C, so you will need GCC.

### Installing

To get started, open the main folder on a Unix based system and type ```make```.
This command should generate ```flex``` and ```bison``` necessary files.
Also, with ```make```, you automatically generates the AST (Abstract Syntax Tree) for the language.

## Running the tests

To test, you can use ```make test```.
Each part has a different set of tests.
You can just get into the part specific folder and run the command.

## Parts

WIP
