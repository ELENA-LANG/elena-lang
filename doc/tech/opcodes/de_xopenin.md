# 0xDE -- xopen i, n

- **Category:** TripleOp
- **Enum:** `ByteCode::XOpenIN`
- **Operand(s):** `xopen i, n` (managed slot count i, unmanaged size n)
- **Reads:** --
- **Writes:** `fp`, `sp`, managed/unmanaged frame layout

## Semantics
Frame variant of `open i, n` -- adjusts the compiler's frame layout for a secondary entry without emitting any prologue code. It signals "the frame was already opened (e.g. by an outer trampoline), now just re-establish slot accounting". Compare with `open i, n` (full program-body entry) and `extopen i, n` (0xF2 -- full external entry with callee-saved spill).

## Compiler behaviour
Dispatched through `compileXOpen` (`jitcompiler.cpp:2945-2951`). Two-step state update on the JIT scope:

1. `frameOffset := calcFrameOffset(arg2, false)` -- establishes the byte offset to the first managed slot using the same arithmetic as `open`/`extopen`, but **without** the external-entry layout bias.
2. If `arg1 == 0`, `stackOffset := 0` -- signals "frame already open; just re-base slot accounting". Otherwise `stackOffset` is left at whatever value the previous opcode set.

**No machine code is emitted**: this is a pure compile-time directive. All downstream `loadStackIndexOp` / `loadFrameIndexOp` calls in the same scope use the updated offsets.

## JIT (amd64)
*No inline template* -- `compileXOpen` does not call `loadCode`. Pure scope mutation.

## JIT (x32)
*No inline template* -- same as amd64.

## JIT (aarch64)
*No inline template* -- same as amd64.

## JIT (ppc64le)
*No inline template* -- same as amd64.

## Notes
- Many variants for `(i, n)` combinations like `OpenIN` are described in the spec but in this branch the JIT does NOT emit code; it only updates `frameOffset`/`stackOffset` on the compiler scope.
- `compileXOpen` resets `scope->stackOffset = 0` if `arg1 == 0` (signals "frame already open, just allocate slots"); otherwise it behaves like a fresh open at the bookkeeping level.
- Callers that need a real frame prologue route through `open i, n` (program body) or `extopen i, n` (0xF2, external entries with callee-saved spill).
- Use cases include nested compilation units that share a parent's prologue (e.g. inline expansion of a method body into its caller's frame).
