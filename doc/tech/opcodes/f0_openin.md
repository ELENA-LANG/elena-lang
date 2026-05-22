# 0xF0 -- open

- **Category:** TripleOp
- **Enum:** `ByteCode::OpenIN`
- **Operand(s):** `open i, n` -- `i` managed slots, `n` unmanaged bytes

## Semantics
Procedure prologue used inside ELENA code (not external entry points). Saves the previous frame, allocates `n` raw bytes for unmanaged data, sets up a new frame header `[fp_prev, 0]`, then allocates `i * ptr_size` managed slots (zero-filled).

The JIT specializes on the (`i`, `n`) shape -- there are ~12 distinct variants tuned for small `i` (0..4) and `n == 0`.

## Compiler behaviour
`compileOpen` (`jitcompiler.cpp:2929-2935`) computes the unmanaged area via `calcFrameOffset(arg2, false)` (the `false` = non-extended), then resets `scope->stackOffset = 0` so subsequent stack-relative operands are measured from the new frame top. The variant slot is **not** chosen via `retrieveCode`; instead the asm dispatch table is indexed directly by the `(i, n)` pair through a switch in the emitter. As a result two textually identical `open 2, 0` calls in different functions can produce different inline bodies depending on which assembler slot the JIT decides applies -- small `(i, n)` lands in `%2F0h..%5F0h` / `%8F0h..%0BF0h`, the generic path in `%0F0h`.

## JIT (amd64)
**Selection rule:** Variant is picked by direct switch on the `(i, n)` pair, *not* via `retrieveCode`. `%1F0h` covers `i=0, n>0`; `%7F0h` covers `i=0, n=0`; `%2F0h..%5F0h` cover `i  in  {1..4}, n>0`; `%6F0h` is `i>0, n=0`; `%8F0h..%0BF0h` cover `i  in  {1..4}, n=0`. Generic `%0F0h` is the fall-through.

**Template:** `asm/amd64/core60.asm` `inline %0F0h`
```asm
; generic: any i, any n
push rbp
xor  rax, rax
mov  rbp, rsp
sub  rsp, __n_2          ; unmanaged
push rbp
push rax
mov  rbp, rsp
mov  rcx, __n_1          ; i (slot count)
sub  rsp, __arg32_1      ; managed bytes
mov  rdi, rsp
rep  stos                ; zero managed slots
```

### Variants
| Prefix | (i, n) | Body |
|---|---|---|
| `%0F0h` | generic | full prologue with `rep stos` |
| `%1F0h` | (0, n) | save frame, alloc n, header -- no managed |
| `%2F0h` | (1, n) | + `push rax; push rax` (1 slot, 16B align) |
| `%3F0h` | (2, n) | + 2x `push rax` |
| `%4F0h` | (3, n) | + 4x `push rax` |
| `%5F0h` | (4, n) | + 4x `push rax` |
| `%6F0h` | (i, 0) | no raw alloc; `rep stos` for slots |
| `%7F0h` | (0, 0) | `push rbp; mov rbp,rsp` only |
| `%8F0h` | (1, 0) | + `push 0; push 0` |
| `%9F0h` | (2, 0) | + 2x `push rax` |
| `%0AF0h` | (3, 0) | + 4x `push rax` |
| `%0BF0h` | (4, 0) | + 4x `push rax` |

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0F0h`
```asm
; generic
push ebp
mov  ebp, esp
xor  eax, eax
sub  esp, __n_2
push ebp
push eax
mov  ebp, esp
mov  ecx, __n_1
sub  esp, __arg32_1
mov  edi, esp
rep  stos
```

### Variants
| Prefix | (i, n) | Body |
|---|---|---|
| `%0F0h` | generic | `rep stos` |
| `%1F0h` | (0, n) | no managed |
| `%2F0h` | (1, n) | + 1 `push ecx` |
| `%3F0h` | (2, n) | + 2 pushes |
| `%4F0h` | (3, n) | + 3 pushes |
| `%5F0h` | (4, n) | + SSE `movq` for 16 zero bytes |
| `%6F0h` | (i, 0) | `rep stos`, no raw alloc |
| `%7F0h` | (0, 0) | `push ebp; mov ebp,esp` |
| `%8F0h` | (1, 0) | + `push 0` |
| `%9F0h` | (2, 0) | + 2 pushes |
| `%0AF0h` | (3, 0) | + 3 pushes |
| `%0BF0h` | (4, 0) | SSE zero-fill 16 bytes |

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0F0h`
```asm
; generic
stp     x29, x30, [sp, #-16]!
mov     x29, sp

sub     sp, sp, __n12_2          ; allocate raw stack

mov     x11, #0
stp     x11, x29, [sp, #-16]!

mov     x29, sp                   ; set frame pointer

sub     sp, sp, __arg12_1         ; allocate stack
mov     x11, __arg16_1
mov     x12, #0
mov     x13, sp

labLoop:
  cmp     x11, #0
  beq     labEnd
  sub     x11, x11, #8
  str     x12, [x13], #8
  b       labLoop

labEnd:
```

### Variants
| Prefix | (i, n) |
|---|---|
| `%0F0h` | generic (loop-zero managed) |
| `%1F0h` | (0, n) |
| `%6F0h` | (i, 0) |
| `%7F0h` | (0, 0) -- minimal `stp x29,x30,[sp,#-16]!` |
| `%8F0h` | (1, 0) |
| `%9F0h` | (2, 0) |
| `%0AF0h` | (3, 0) |
| `%0BF0h` | (4, 0) |

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0F0h`
```asm
; generic
mflr    r0

std     r31, -10h(r1)            ; save frame pointer
std     r0,  -08h(r1)            ; save return address
addi    r1, r1, -16               ; allocate raw stack

mr      r31, r1                   ; set frame pointer

addi    r1, r1, -__n16_2

li      r16, 0
std     r31, -08h(r1)
std     r16, -10h(r1)
addi    r1, r1, -10h
mr      r31, r1

addi    r1, r1, -__arg16_1        ; managed
li      r16, __arg16_1
li      r17, 0
mr      r18, r1

labLoop:
  cmpwi   r16,0
  beq     labEnd
  addi    r16, r16, -8
  std     r17, 0(r18)
  addi    r18, r18, 8
  b       labLoop

labEnd:
```

### Variants
| Prefix | (i, n) |
|---|---|
| `%0F0h` | generic |
| `%1F0h` | (0, n) |
| `%2F0h` | (1, n) |
| `%3F0h` | (2, n) |
| `%4F0h` | (3, n) |
| `%5F0h` | (4, n) |
| `%6F0h` | (i, 0) |
| `%7F0h` | (0, 0) -- minimal |
| `%8F0h` | (1, 0) |
| `%9F0h` | (2, 0) |
| `%0AF0h` | (3, 0) |
| `%0BF0h` | (4, 0) |

## Notes
- Pairs with `close n` (0x91) -- every `open` must be closed before the method returns.
- Many variants for `(i, n)  in  {0,1,2,3,4} x {0, >0}` -- small frames hit faster, hand-rolled prologues.
- Pre-zeros the `i` managed slots so the GC's stack scan never sees stale values.
- The variant slot is picked by direct (i, n) switch -- `retrieveCode` is bypassed.
- Used for in-language ELENA functions; external entry points use `extopen` (0xF2) instead.
