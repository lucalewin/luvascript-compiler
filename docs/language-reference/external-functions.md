# External Functions

## Why should I use external functions

The luvascript language does not follow the standard [`C naming convention`](https://en.wikipedia.org/wiki/X86_calling_conventions) when naming functions and variables. It instead uses a different naming convention called the [`Luvascript Naming Convention (LNC)`](../compiler/conventions/luvascript-naming-convention.md). Therefore you have to specify the `extern`-keyword when you want to call the function from a different programming language. The the compiler knows that it should use the `C naming convention` instead of the LNC.

## Define a external function

```luvascript
extern function <identifier> ( <parameters> ) : <return-type>;
```
or
```luvascript
extern function <identifier> ( <parameters> ) : <return-type> {
    <body>
}
```

!!! warning "Important"

    You need to disable linking by the luvascript compiler when using external functions by using the `-c` [compiler flag](../compiler/options.md). Otherwise the compiler will try to link the object file to the executable, but the linker will not find a reference to the external function.