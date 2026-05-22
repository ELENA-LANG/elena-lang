# 0x00 -- nop

- **Category:** SingleOp
- **Enum:** `ByteCode::Nop`
- **Operand(s):** (none)
- **Reads:** (none)
- **Writes:** (none)

## Semantics
Placeholder instruction. Consumed by the JIT and not emitted into the native code stream.

## Compiler behaviour
Handled by `loadNop` in `elenasrc3/engine/jitcompiler.cpp` (lines 439-449). The JIT emits zero bytes for `ByteCode::Nop`; instead the opcode is used as a **label fixup anchor**. Any pending forward-label references whose target is "the next instruction" are resolved to the current code offset at this point. As a result `nop` is the canonical label anchor in optimized tape, and the back-end never emits a real x86/ARM/PPC `nop` instruction for it.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` (no inline body)

*No emission; the opcode is dropped by the JIT (handled by `loadNop`).*

## JIT (x32)
**Template:** `asm/x32/core60.asm` (no inline body)

*No emission; the opcode is dropped by the JIT (handled by `loadNop`).*

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` (no inline body)

*No emission; the opcode is dropped by the JIT (handled by `loadNop`).*

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` (no inline body)

*No emission; the opcode is dropped by the JIT (handled by `loadNop`).*

## Notes
- Acts purely as a label fixup site -- emits zero bytes on every arch. Real native NOPs come from `xnop` (0x32).
- Preserves all abstract roles (acc, index, sp[0], sp[1], COMP flags) because nothing is emitted.
- The canonical label anchor in optimised tape: any pending forward-reference whose target is "next instruction" is patched to the current code offset here.
