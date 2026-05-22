# 0x32 -- xnop

- **Category:** SingleOp
- **Enum:** `ByteCode::XNop`
- **Operand(s):** (none)
- **Reads:** (none)
- **Writes:** code stream (alignment padding)
- **Side effects:** no architectural state changes at runtime

## Semantics
*Emits up to N alignment-padding NOPs so the following label is aligned for CPU jump prediction. Implementation: `loadXNop`.*

## Compiler behaviour
`loadXNop` (`jitcompiler.cpp` lines 451-456) calls `scope->compiler->alignJumpAddress()` to pad the current code offset up to the CPU's preferred jump-target alignment (16 bytes on x86; on aarch64/ppc64le insns are word-sized so labels are naturally 4-aligned, but the helper may still emit NOPs to align the next basic block to 16 for I-cache locality), then delegates to `loadNop()` for label fixup. The padding bytes are real arch NOPs (`nop` on x86, `nop`/`mov x0, x0` on aarch64, `ori 0,0,0` on ppc64le).

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %32h`

*Emits up to N alignment-padding NOPs so the following label is aligned for CPU jump prediction. Implementation: `loadXNop`.*

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %32h`

*Emits up to N alignment-padding NOPs so the following label is aligned for CPU jump prediction. Implementation: `loadXNop`.*

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %32h`

*Emits up to N alignment-padding NOPs so the following label is aligned for CPU jump prediction. Implementation: `loadXNop`.*

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %32h`

*Emits up to N alignment-padding NOPs so the following label is aligned for CPU jump prediction. Implementation: `loadXNop`.*

## Notes
- Pure alignment padding -- no register or memory state changes at runtime.
- On x86 (amd64/x32) this matters: 16-byte alignment of branch targets improves the BTB hit rate and front-end fetch.
- On aarch64/ppc64le the effect is largely cosmetic since all instructions are 4-byte aligned by ISA; padding still helps I-cache line locality for the following basic block.
- Number of NOPs is variable (0 to 15 on x86); JIT chooses based on current code offset.
- Often inserted before hot-loop heads and after `jump`/`ret` so the fallthrough lands on a fresh fetch group.
