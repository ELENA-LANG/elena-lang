# 0x31 -- altmode

- **Category:** SingleOp
- **Enum:** `ByteCode::AltMode`
- **Operand(s):** (none)
- **Reads:** (compile-time only)
- **Writes:** JIT scope flag
- **Side effects:** no machine code emitted

## Semantics
Compile-time pseudo-opcode that flips the JIT scope into "alternate dispatch" mode. The next dispatch opcode emits its index-table form instead of the direct form, after which the flag is cleared. Implementation: `compileAltMode` in `elenasrc3/engine/jitcompiler.cpp`.

## Compiler behaviour
`compileAltMode` (`jitcompiler.cpp` lines 2979-2982) sets `scope->altMode = true`; no bytes are emitted. The flag is consumed by the next dispatch opcode -- `compileDispatchMR` (line 3080) reads `altModifier = scope->altMode ? 6 : 0` to pick the alt-mode block of 6 variant slots in DispatchMR/XDispatchMR and then resets `altMode`. This pairs `altmode` with the following `dispatch`/`xdispatch`/`vcall`/`vjump` so they use the VMT index-table fast path instead of the default linear scan.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %31h`

*JIT scope flag; no emission. Subsequent dispatch opcode uses alternate (index-table) form, then flag is cleared.*

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %31h`

*JIT scope flag; no emission. Subsequent dispatch opcode uses alternate (index-table) form, then flag is cleared.*

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %31h`

*JIT scope flag; no emission. Subsequent dispatch opcode uses alternate (index-table) form, then flag is cleared.*

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %31h`

*JIT scope flag; no emission. Subsequent dispatch opcode uses alternate (index-table) form, then flag is cleared.*

## Notes
- Pure JIT-scope flag -- emits no native instructions; cannot be reordered or hoisted.
- Must be emitted **immediately** before its consumer; the very next dispatch opcode reads and clears it.
- Consumers are `xdispatch` (0xFA), `dispatch mssg` (0xFB), `vcall mssg` (0xFC), and `vjump mssg` (0xEC).
- When set, the consumer emits its **index-table fast path** instead of a linear VMT scan -- substantial speedup for dispatch-heavy code.
- Reset automatically by the consumer; never persists across dispatch boundaries.
