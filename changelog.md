# Luvascript Changelog

## Luvascript v0.2.0
### New Language Features
- added primitive datatypes `char` and `string`
- added support for one-dimensional arrays
- added support for packages
- added `import ___;` and `from ___ import ___;` statements

### New Compiler Features
- multiple input source files
- compiler option `-S` to generate assembly code
- compiler option `-c` to disable linking of generated object file

### Other Changes
- Updated the LuvaScript Documentation to reflect the changes made in this release.

## Luvascript v0.1.1
### Bug Fixes
- fixes multiple memory leaks

## Luvascript v0.1.0
### New Language Features

- primitive datatypes (`byte`, `short`, `int`, `long`, `bool`)
- functions
- global variables / constants
- local variables / constants
- basic mathematical expressions
- condtional expressions
- external functions (importing / exporting functions to c/c++ or other)
- conditional statements (`if` / `else if` / `else`)
- loop statements (`while`)
- jump statememts (`return`)