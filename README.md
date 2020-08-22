# flex based Compiler [![Build Status](https://travis-ci.org/Andy9822/compiler.svg?branch=master)](https://travis-ci.org/Andy9822/compiler)

Proposed and developed in UFRGS' Compilers course.
You can clone or download this repository freely.


### Prerequisites

TLC is a flex based compiler. So, you will need to install it.
You can use
```
sudo apt-get install flex
```
It also uses programming language C, so you will need GCC and make.

```
sudo apt-get install build-essential
```

### Installing

To get started, open the main folder on a Unix based system and type ```make```.
This command should generate ```flex``` necessary files.
Also, with ```make```, you automatically generates the AST (Abstract Syntax Tree) for the language.

## Running the tests

The project is divided in parts, so each part folder contains a specific stage of the compiler.
Each part has a different set of tests.
To test, you can use ```make test```.

## Parts

WIP
