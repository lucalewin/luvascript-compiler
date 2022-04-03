# External Functions

## Why should I use external functions

The luvascript language does not follow the standard [`C calling convention`](https://en.wikipedia.org/wiki/X86_calling_conventions) when naming functions and variables. It instead uses a different calling convention called the [`Luvascript Calling Convention (LCC)`](../compiler/conventions/luvascript-calling-convention.md). Therefore you have to specify the `extern`-keyword when you want to call the function from a different programming language. The the compiler knows that it should use the `C calling convention` instead of the LCC.

## Define a external function

```luvascript
extern function <identifier> ( <parameters> ) : <return-type>
```

!!! warning "Important"

    You need to disable linking by the luvascript compiler when using external functions. Otherwise the compiler will try to link the object file to the executable, but the linker will not find a reference to the external function.