# Inline Assembly in the Luva Language

## How to enable

When compiling use the `-asm` flag. ([See more](../compiler_flags.md))

## How to use

```
function main() {
    var x: int = 10;
    var y: int = 12;
    var z: int;
    asm {
        mov eax, x
        mov ebx, y
        add eax, ebx
        mov z, eax
    }
    println("Number: " + z);
}
```