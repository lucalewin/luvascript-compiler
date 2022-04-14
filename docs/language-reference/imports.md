# Import Statements

(for Luvascript Version 0.1)

### Importing a package

    import <package-name>;

### Only importing some functions or variables

    import <package-name>::<identifier>;

or

    import <package-name>::{ 
        <identifier>, 
        <other identifier>,
        ...
    };

!!! example

        import std.io::write;
        import std.io::read;
        import std.io::print;
        import std.io::println;
        import std.io::readln;
        import std.io::flush;

    or

        import std.io::{
            write,
            read,
            print,
            println,
            readln,
            flush
        };

### Importing all functions and variables from a package

    import <package-name>::*;

!!! example

        import std.io::*;

### Importing a package and renaming it

    import <package-name> as <new-package-name>;

### Importing functions and variables from a package and renaming them

    import <package-name>::{
        <type/variable-identifer> as <new-type/variable-identifer>, 
        <other-type/variable-identifer> as <other-new-type/variable-identifer>,
        ...
    };