# Luvascript Naming Convention (LNC)

Because of its implementation the luvascript language uses a different naming convention for functions and variables.

## Why not use the standard `C` naming convention?

In the Luvascript language you can use a function identifier multiple times, so you can use the same function name for different functions. This is not possible in the C language and therefore it is not in the `C` naming convention.

## Limitations

You can use the same function name for different functions, but the parameter types of the functions must be different.

### Example
```luvascript
function test(value: i32): bool {
    return value == 0;
}

function test(value: i64): bool {
    return value == 0;
}
```
In this case you can use the same function name for different functions, because the parameter types of `value` are different.

```luvascript
function test(value: bool): bool {
    return value == false;
}

function test(value: bool): bool {
    return value == true;
}
```
In this case you cannot use the same function name for different functions, because the parameter types of `value` are the same.

## How to use the standard `C` naming convention

If you want to use the standard `C` naming convention, you can use the `extern` keyword.

```luvascript
extern function <identifier> ( <parameters> ): <return_type> {
    <body>
}
```

## Format

General format:

`_func_<function_name>_<parameters>_<return_type>`

### Example

The name of this function
```luvascript
function test(text: string, count: i64): bool {
    return text.length == count;
}
```
will be converted to `_func_test_string_i64_bool`
