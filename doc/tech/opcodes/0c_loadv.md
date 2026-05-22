# 0x0C -- loadv

- **Category:** SingleOp
- **Enum:** `ByteCode::LoadV`
- **Operand(s):** (none)
- **Reads:** `index` (arg bits only), `[acc]` (action bits only)
- **Writes:** `index`

## Semantics
Splices the *action* bits read from `[acc]` into `index` while preserving the *argument* bits already held there: `index := (index & ARG_MASK) | ([acc] & ~ARG_MASK)`. Used when patching a message id with a new selector while keeping the caller's argument count.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline % 0Ch`

```asm
  and  edx, ARG_MASK
  mov  ecx, dword ptr [rbx]
  and  ecx, ~ARG_MASK
  or   edx, ecx
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline % 0Ch`

```asm
  and  edx, ARG_MASK
  mov  ecx, [ebx]
  and  ecx, ~ARG_MASK
  or   edx, ecx
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline % 0Ch`

```asm
  ;and     x9, x9, ARG_MASK
  mov     x16, ARG_MASK
  and     x9, x9, x16

  movz    x16,  ~ARG_MASK
  movk    x16,  #0FFFFh, lsl #16

  ldrsw   x14, [x10]
  and     x14, x14, x16

  orr     x9, x9, x14
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline % 0Ch`

```asm
  andi.   r14, r14, ARG_MASK

  li      r19, ~ARG_MASK
  andi.   r19, r19, 0FFFFh
  addis   r19, r19, 0FFFFh ; note: to adjust hi word

  lwz     r18, 0(r15)
  and     r18, r18, r19

  or      r14, r14, r18
```

## Notes
- Reads the action+flags bits from `[acc]` and merges them into `index` while preserving the low ARG_MASK bits (argument count) already in `index`. Used to splice a new VMT/selector into an existing message word.
- aarch64 has to materialise the inverted mask via `movz`/`movk` because ARM immediates cannot encode an arbitrary 32-bit constant; amd64/x32 use the literal directly.
- Preserves `acc`, `sp[0]`, `sp[1]`. Clobbers `index` and a scratch (`ecx`/`x14`/`r18`).
