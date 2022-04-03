# Luvascript Naming Convention (LNC)

Because of its implementation the luvascript language uses a different naming convention for functions and variables.

## Why not use the standard `C naming convention`?

In the Luvascript language you can use a function identifier multiple times, so you can use the same function name for different functions. This is not possible in the C language and therefore it is not in the `C naming convention`.

The use of the LNC also prevents some unexpected behavior. For example, if you declare a function with the name `_start`, the Assembler will throw an error, because the Luvascript compiler automatically adds the `_start` function to the start of the program and therefore exists twice in the assembly code.
This is prevented by adding a `_func_` prefix to the function name.

!!! warning

    You can use the same function name for different functions, but the parameter types of the functions must be different.

## Examples

!!! success
    ```luvascript
    function test(value: i32): bool {
        return value == 0;
    }
    function test(value: i64): bool {
        return value == 0;
    }
    ```
    In this case you can use the same function name for different functions, because the parameter types of `value` are different.
!!! failure
    ```luvascript
    function test(value: i32): bool {
        return value == 0;
    }
    function test(value: i32): bool {
        return value == 0;
    }
    ```
    In this case you can not use the same function name for different functions, because the parameter types of `value` are the same.

## How to use the standard `C calling convention`

If you want to use the standard `C calling convention`, you can use the `extern` keyword.

```luvascript
extern function <identifier> ( <parameters> ): <return_type> {
    <body>
}
```

See also: [External Functions](../../language-reference/external-functions.md)

## Format

General format:

`_func_<function_name>__<parameters>`, where `<function_name>` is the name of the function and `<parameters>` is the parameter types separated by `_`.

!!! example

    The name of this function
    ```luvascript
    function test(text: string, count: i64): bool {
        return text.length == count;
    }
    ```
    will be converted to `_func_test__string_i64`
