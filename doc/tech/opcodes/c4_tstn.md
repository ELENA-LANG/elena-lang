# 0xC4 -- tst n

- **Category:** DoubleOp
- **Enum:** `ByteCode::TstN`
- **Operand(s):** `tstn n` (32-bit immediate)
- **Reads:** `index`
- **Writes:** `COMP.EQ`

## Semantics
`temp := index & n`; `COMP.EQ := (temp == 0)`. Bit-test of the data accumulator.

## JIT (amd64)
**Selection rule:** dispatched through `loadDPNOp` -> `retrieveICode(scope, arg)`. The picker keys on the magnitude of the immediate -- small constants use slot 0; larger constants pick slot 10 (extended-form). amd64/x32 always collapse to a single `test edx, imm32`; aarch64/ppc64le emit a `movz`+`movk` materialisation when the constant exceeds 16 bits.

**Template:** `test edx, imm32`.

```asm
test edx, __n_1
```

## JIT (x32)
**Template:** `test edx, imm32`.

```asm
test edx, __n_1
```

## JIT (aarch64)
**Template:** `mov` the immediate, `tst`. Two extra variants (`%09C4h`, `%0AC4h`) materialise larger constants via `movz/movk`.

```asm
mov     x11, __n16_1
tst     x9, x11
```

### Variants
| Prefix | Form |
|---|---|
| `%0C4h` | small constant -- `mov x11, __n16_1; tst x9, x11` |
| `%09C4h` / `%0AC4h` | 32-bit constant -- `movz x11, lo; movk x11, hi, lsl #16; tst x9, x11` |

## JIT (ppc64le)
**Template:** build immediate with `li`+`addis`, `and.` sets CR0.

```asm
li      r17, __n16lo_1
addis   r17, r17, __n16hi_1
and.    r17, r14, r17
```

## Notes
- Reads only `index`; does not touch `acc`.
- Followed by `jeq`/`jne`. `EQ == 1` means *all mask bits clear* in `index`.
- Common usage: extract message argCount / argFlags from `index` and dispatch on a bitfield.
- aarch64/ppc64le pick a wider materialisation when the mask doesn't fit in 16 bits -- this is the only width-sensitivity of the opcode.
