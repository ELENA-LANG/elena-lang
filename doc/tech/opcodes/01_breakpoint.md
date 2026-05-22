# 0x01 -- breakpoint

- **Category:** SingleOp
- **Enum:** `ByteCode::Breakpoint`
- **Operand(s):** (none)
- **Reads:** (none)
- **Writes:** debug section
- **Side effects:** Records the current code position as a breakpoint marker in the target debug section when `withDebugInfo` is enabled.

## Semantics
Debug breakpoint marker. The JIT does **not** emit a trap instruction into the code stream; instead `compileBreakpoint` calls the linker helper `addBreakpoint`, which writes the current code address into the debug section. Debuggers consult that table when planting INT3/BRK traps at runtime.

## Compiler behaviour
Handled by `compileBreakpoint` in `elenasrc3/engine/jitcompiler.cpp` (lines 2973-2977). Emits exactly one debug-trap instruction (`int 3` on x86/amd64, `brk #0` on aarch64, `twge r2,r2` on ppc64le) **only when** the JIT scope has `withDebugInfo` set. In release builds (no debug info) the opcode is elided entirely and zero bytes are emitted, so `Breakpoint` carries no runtime cost outside debug builds.

## JIT (amd64)
**Template:** `elenasrc3/engine/jitcompiler.cpp` `compileBreakpoint`

*Emitted only when the JIT is in debug mode (`withDebugInfo` set): one `int 3` byte plus a debug-section entry recording the code offset. In release mode no instruction is emitted.*

## JIT (x32)
**Template:** `elenasrc3/engine/jitcompiler.cpp` `compileBreakpoint`

*Emitted only when the JIT is in debug mode (`withDebugInfo` set): one `int 3` byte plus a debug-section entry recording the code offset. In release mode no instruction is emitted.*

## JIT (aarch64)
**Template:** `elenasrc3/engine/jitcompiler.cpp` `compileBreakpoint`

*Emitted only when the JIT is in debug mode (`withDebugInfo` set): a single `brk #0` instruction plus a debug-section entry recording the code offset. In release mode no instruction is emitted.*

## JIT (ppc64le)
**Template:** `elenasrc3/engine/jitcompiler.cpp` `compileBreakpoint`

*Emitted only when the JIT is in debug mode (`withDebugInfo` set): a single `twge r2,r2` trap instruction plus a debug-section entry recording the code offset. In release mode no instruction is emitted.*

## Notes
- Zero-cost in release builds: when `withDebugInfo` is unset, the opcode is elided entirely (no bytes emitted, no debug entry).
- The JIT does NOT plant the trap directly into the code stream as a runtime guard -- `addBreakpoint` records the code address in the debug section so debuggers can install INT3/BRK lazily at runtime.
- Preserves all abstract roles (acc, index, sp[0], sp[1]).
