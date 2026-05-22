# 0xB7 -- jlt

- **Category:** DoubleOp
- **Enum:** `ByteCode::Jlt`
- **Operand(s):** `jlt label`
- **Reads:** `COMP.LT` flag
- **Writes:** `pc` (conditionally)
- **Side effects:** None.

## Semantics
Branch to `label` if `COMP.LT == 1` -- **signed** less-than. Pair with `cmpn`, `xcmp`, `xlcmp`, `cmp dp:i`, etc., which set signed comparison flags. For unsigned compares use `jult` (separate opcode). No inline template -- the JIT emits the conditional branch directly via `LabelHelper::writeJlt{Forward,Back}`.

## Compiler behaviour
`compileJlt` (`jitcompiler.cpp:3023-3034`) mirrors the rest of the conditional-branch family:

```cpp
pos_t label = scope->tapeReader->position() + scope->command.arg1;
if (scope->command.arg1 < 0)      lh->writeJltBack(label, *codeWriter);
else if (scope->command.arg1 > 0) lh->writeJltForward(label, *codeWriter, scope->command.arg1);
```

`COMP.LT == 1` maps to (signed forms throughout):
- amd64/x32: `jl` (`SF != OF`)
- aarch64: `b.lt` (`writeBcc(JumpType::LT)`)
- ppc64le: `blt cr0` (`writeBCxx(BO=12, BI=0)`)

## JIT (amd64)
*Not implemented in this backend as an inline template.* Emitted via `X86LabelHelper` as `X86JumpType::JL` (SF != OF):

```asm
  ; short:  7C ib        ; jl rel8
  ; near:   0F 8C id     ; jl rel32
```

## JIT (x32)
*Not implemented in this backend as an inline template.* Same `jl` encoding as amd64.

## JIT (aarch64)
*Not implemented in this backend as an inline template.* Emitted via `ARM64LabelHelper::writeJlt{Forward,Back}` using `writeBcc` with `JumpType::LT`:

```asm
  b.lt  target
```

## JIT (ppc64le)
*Not implemented in this backend as an inline template.* Emitted via `PPC64LabelHelper::writeJlt{Forward,Back}` as `writeBCxx(12, 0, ...)` -- `BO=12, BI=0` selects "branch if CR0[LT] set":

```asm
  blt   cr0, target
```

## Notes

- **Signed** less-than. For unsigned compares the bytecode emits `jult` (separate opcode family).
- No inline template; emitted by `LabelHelper`.
- Companion ordering predicates: `jge` (0xB8), `jgr` (0xB9), `jle` (0xBA).
- Must follow a flag-setting opcode in the same basic block.
