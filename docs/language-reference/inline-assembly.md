# Inline Assembly in the Luva Language

## Limitations

- Currently the Luvascript compiler does only support inline assembly for x86-64 platforms. Additional information about x86-64 assembly is available in the [x86 Assembly Guide](https://www.cs.virginia.edu/~evans/cs216/guides/x86.html).
*It is planned to support other platforms in the future.*
- The inline assembly has to be in the Intel syntax.
- It is currently not possible to reference luvascript variables in the inline assembly.

## How to enable

Inline Assembly is enabled by default and can not be disabled (yet).

## How to use

Use the inline assembly syntax to write assembly code.
```luvascript
asm { 
	<assembly> 
}
```

The following example shows how to use inline assembly:

```luvascript
function main(): int {
	asm {
		mov eax, 6
		add eax, 9
		mov rdi, rax
		mov rax, 60
		syscall
	}
}
```
This example adds 6 and 9 and uses the result as exit code.