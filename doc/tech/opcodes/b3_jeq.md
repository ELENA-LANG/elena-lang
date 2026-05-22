# 0xB3 -- jeq

- **Category:** DoubleOp
- **Enum:** `ByteCode::Jeq`
- **Operand(s):** `jeq label`
- **Reads:** `COMP.EQ` flag (set by `cmp*`/`tst*`)
- **Writes:** `pc` (conditionally)
- **Side effects:** None.

## Semantics
Branch to `label` if `COMP.EQ == 1`. Pair with any opcode that updates `COMP.EQ`: `cmpr`, `cmpn`, `cmpfi`, `cmpsi`, `tstn`, `tstflag`, `tstm`, `tststck`. No inline template -- the JIT emits the conditional branch directly via `LabelHelper::writeJeq{Forward,Back}`.

## Compiler behaviour
`compileJeq` (`jitcompiler.cpp:2997-3008`) follows the same pattern as `compileJump`:

```cpp
pos_t label = scope->tapeReader->position() + scope->command.arg1;
if (scope->command.arg1 < 0)      lh->writeJeqBack(label, *codeWriter);
else if (scope->command.arg1 > 0) lh->writeJeqForward(label, *codeWriter, scope->command.arg1);
```

The abstract `COMP.EQ` flag maps to:
- amd64/x32: `je` / `jne` (ZF)
- aarch64: `b.eq` (`writeBcc(JumpType::EQ)`)
- ppc64le: `beq cr0` (`writeBCxx(BO=12, BI=2)`)

## JIT (amd64)
*Not implemented in this backend as an inline template.* Emitted via `X86LabelHelper` as `X86JumpType::JZ` (ZF set):

```asm
  ; short:  74 ib        ; je rel8
  ; near:   0F 84 id     ; je rel32
```

Short form is chosen when the bytecode-level offset is `< 0x10` (forward) or fits in a signed 8-bit native offset (back).

## JIT (x32)
*Not implemented in this backend as an inline template.* Same `je` encoding as amd64.

## JIT (aarch64)
*Not implemented in this backend as an inline template.* Emitted via `ARM64LabelHelper::writeJeq{Forward,Back}` using `writeBcc` with `JumpType::EQ`:

```asm
  b.eq  target
```

## JIT (ppc64le)
*Not implemented in this backend as an inline template.* Emitted via `PPC64LabelHelper::writeJeq{Forward,Back}` as `writeBCxx(12, 2, ...)` -- `BO=12, BI=2` selects "branch if CR0[EQ] set":

```asm
  beq   cr0, target
```

## Notes

- No inline template; the JIT emits arch-specific conditional branches resolved by `LabelHelper`.
- Predicate is **signed** semantically on amd64 (`ZF`) -- though for raw equality signed vs unsigned does not matter.
- Inverse: `jne` (0xB4).
- Must be preceded by a flag-setting opcode within the same basic block; intervening non-flag opcodes (most ELENA opcodes preserve flags) are safe but the abstract VM model assumes the previous `cmp*`/`tst*` is the producer.
