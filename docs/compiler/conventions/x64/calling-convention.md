# x86-64 Calling Convention

## Parameter passing

| Parameter Type | arg0 | arg1 | arg2 | arg3 | arg4 | arg5 | arg6  | arg7  | argN  |
| -------------- | ---- | ---- | ---- | ---- | ---- | ---- | ----- | ----- | ----- |
| Integer        | rdi  | rsi  | rdx  | rcx  | r8   | r9   | stack | stack | stack |
| Float          | xmm0 | xmm1 | xmm2 | xmm3 | xmm4 | xmm5 | xmm6  | xmm7  | stack |
| Pointer        | rdi  | rsi  | rdx  | rcx  | r8   | r9   | stack | stack | stack |
| BigInteger (`i128`, `u128`), as a pointer | rdi  | rsi  | rdx  | rcx  | r8   | r9   | stack | stack | stack |

### Examples

!!! example "Only integer parameters"

    ```luvascript
    function func(a: i32, b: i32, c: i32, d: i32, e: i32, f: i32, g: i32): void {}
    // a -> rdi
    // b -> rsi
    // c -> rdx
    // d -> rcx
    // e -> r8
    // f -> r9
    // g -> stack
    ```

!!! example "Only float parameters"

    ```luvascript
    function func(a: f32, b: f32, c: f32, d: f32, e: f32, f: f32, g: f32): void {}
    // a -> xmm0
    // b -> xmm1
    // c -> xmm2
    // d -> xmm3
    // e -> xmm4
    // f -> xmm5
    // g -> stack
    ```

!!! example "Mixed parameters"

    ```luvascript
    function func(a: i32, b: f32, c: i32, d: f32, e: i32, f: f32, g: i32): void {}
    // a -> rdi
    // b -> xmm1
    // c -> rdx
    // d -> xmm3
    // e -> r8
    // f -> xmm5
    // g -> stack
    ```

## Return values

| Return Type | arg0 | arg1 | arg2 | arg3 | arg4 | arg5  | arg6  | arg7  | argN  |
| ----------- | ---- | ---- | ---- | ---- | ---- | ----- | ----- | ----- | ----- |
| Integer     | rax  | r8   | r9   | r10  | r11  | stack | stack | stack | stack |
| Float       | xmm0 | xmm1 | xmm2 | xmm3 | xmm4 | xmm5  | xmm6  | xmm7  | stack |
| Pointer     | rax  | r8   | r9   | r10  | r11  | stack | stack | stack | stack |
| BigInteger (`i128`, `u128`), as a pointer | rax  | r8   | r9   | r10  | r11  | stack | stack | stack | stack |

### Examples

!!! example "Only integer return values"

    ```luvascript
    function func(): i32, i32, i32, i32, i32, i32, i32;
    // 1 -> rax
    // 2 -> rcx
    // 3 -> rdx
    // 4 -> rsi
    // 5 -> rdi
    // 6 -> stack
    // 7 -> stack
    ```

!!! example "Only float return values"

    ```luvascript
    function func(): f32, f32, f32, f32, f32, f32, f32;
    // 1 -> xmm0
    // 2 -> xmm1
    // 3 -> xmm2
    // 4 -> xmm3
    // 5 -> xmm4
    // 6 -> xmm5
    // 7 -> stack
    ```

!!! example "Mixed return types"

    ```luvascript
    function func(): i32, f32, i32, f32, i32, f32, i32;
    // 1 -> rax
    // 2 -> xmm1
    // 3 -> rdx
    // 4 -> xmm3
    // 5 -> rdi
    // 6 -> stack
    // 7 -> stack
    ```
