# 0x88 -- mov mssg

- **Category:** DoubleOp
- **Enum:** `ByteCode::MovM`
- **Operand(s):** `mov mssg:m` (M-command, message reference)
- **Reads:** --
- **Writes:** `index`

## Semantics
`index := m` -- load a 32-bit message reference (action + arg count + flags) into the data accumulator. Same emission shape as `mov n:n` (0x89) but the immediate carries a message-table relocation so it is patched correctly when message layouts are finalised.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %88h`

**Selection rule:** `retrieveCode` picks the variant slot; additionally the argument is **rewritten by `importMessage(arg)`** before emit -- the JIT translates the local module's `mssg_t` index through the module's message table into a globally-stable reference. Therefore the immediate in the emitted code is the final cross-module message ref, not the source-bytecode value.

```asm
  mov  edx, __arg32_1
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %88h`

```asm
  mov  edx, __arg32_1
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %88h`

```asm
  movz    x9,  __arg32lo_1
  movk    x9,  __arg32hi_1, lsl #16
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %88h`

```asm
  lis     r14, __arg32hi_1
  addi    r14, r14, __arg32lo_1
```

## Notes
- Emission shape matches `mov n` (0x89) but the immediate is a message-table relocation.
- The argument is translated through `importMessage` so cross-module messages resolve correctly after final layout.
- Preserves `acc`, `sp[0..1]`; only `index` (the data accumulator) is written.
- Pairs with `dispatch` (0x78) and other message-driven control flow.
