# 0x91 -- close n

- **Category:** DoubleOp
- **Enum:** `ByteCode::CloseN`
- **Operand(s):** `close n`
- **Reads:** previous frame chain
- **Writes:** `fp`, `sp`
- **Side effects:** Function epilogue -- restores caller frame. Must pair with `open` / `extopen`. Do not confuse with `extclose n` (0xCA).

## Semantics
Restore the previous frame. `n` is an extra adjustment added to `fp` before the standard epilogue (used to undo per-frame padding / data area).

## Compiler behaviour
`compileClose` (`elenasrc3/engine/jitcompiler.cpp:2919-2927`) adjusts `arg1 += dataHeader` before emit -- i.e. it folds the frame-header byte count into the released-stack size -- and then resets `scope->stackOffset := scope->unframedOffset`. This pairs the close with the matching `open i, n` (0xF0): the JIT tracks frame state across the pair, so emitting a `close` without a preceding `open` will desync `stackOffset` for any subsequent stack-relative opcodes.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %91h`

```asm
  add  rbp, __n_1
  mov  rsp, rbp
  pop  rbp
```

### Variants
| Prefix | Selected when | Body differs by |
|---|---|---|
| `%91h`  | `n != 0` | three-insn form above |
| `%191h` | `n == 0` | drops the `add rbp, 0`; just `mov rsp, rbp` / `pop rbp` |

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %91h`

```asm
  add  ebp, __n_1
  mov  esp, ebp
  pop  ebp
```

### Variants
| Prefix | Selected when | Body differs by |
|---|---|---|
| `%91h`  | `n != 0` | three-insn form above |
| `%191h` | `n == 0` | `mov esp, ebp` / `pop ebp` only |

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %91h`

```asm
  add     x29, x29, __n12_1
  mov     sp, x29
  ldp     x29, x30, [sp], #16
```

### Variants
| Prefix | Selected when | Body differs by |
|---|---|---|
| `%91h`  | `n != 0` | three-insn form above |
| `%191h` | `n == 0` | drops the `add`; `mov sp, x29` / `ldp x29, x30, [sp], #16` |

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %91h`

```asm
  addi    r31, r31, __n16_1  ; skip unframed stack
  mr      r1, r31              ; restore stack pointer

  ld      r31, 00h(r1)         ; restore frame pointer
  ld      r0,  08h(r1)         ; restore  return address

  mtlr    r0
  addi    r1, r1, 10h          ; free stack
```

### Variants
| Prefix | Selected when | Body differs by |
|---|---|---|
| `%91h`  | `n != 0` | full epilogue with `addi r31, r31, n` |
| `%191h` | `n == 0` | drops the leading `addi` |

## Notes
- Pairs with `open i, n` (0xF0); `extopen` (0xC9) pairs with `extclose` (0xCA) -- do not mix.
- Restores `rbp`/`x29`/`r31` from the saved-frame chain and `rsp`/`sp`/`r1` to the caller's stack pointer.
- Compiler folds `dataHeader` into `arg1` and resets `stackOffset` -- must be one per function unless multiple converge.
- `n == 0` selects a short form that drops the redundant frame-adjust.
