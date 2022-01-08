# External Functions

## Define a external function

```luvascript
extern function <identifier> ( <parameters> ) : <return-type>
```

## Important

You need to disable linking by the luvascript compiler when using external functions. Otherwise the compiler will try to link the object file to the executable, but the linker will not find a reference to the external function.