# 0xB4 -- jne

- **Category:** DoubleOp
- **Enum:** `ByteCode::Jne`
- **Operand(s):** `jne label`
- **Reads:** `COMP.EQ` flag
- **Writes:** `pc` (conditionally)
- **Side effects:** None.

## Semantics
Branch to `label` if `COMP.EQ == 0`. Inverse of `jeq` (0xB3). No inline template -- the JIT emits the conditional branch directly via `LabelHelper::writeJne{Forward,Back}`.

## Compiler behaviour
`compileJne` (`jitcompiler.cpp:3010-3021`) mirrors `compileJeq`:

```cpp
pos_t label = scope->tapeReader->position() + scope->command.arg1;
if (scope->command.arg1 < 0)      lh->writeJneBack(label, *codeWriter);
else if (scope->command.arg1 > 0) lh->writeJneForward(label, *codeWriter, scope->command.arg1);
```

`COMP.EQ == 0` maps to:
- amd64/x32: `jne` (ZF clear)
- aarch64: `b.ne` (`writeBcc(JumpType::NE)`)
- ppc64le: `bne cr0` (`writeBCxx(BO=4, BI=2)`)

## JIT (amd64)
*Not implemented in this backend as an inline template.* Emitted via `X86LabelHelper` as `X86JumpType::JNZ`:

```asm
  ; short:  75 ib        ; jne rel8
  ; near:   0F 85 id     ; jne rel32
```

## JIT (x32)
*Not implemented in this backend as an inline template.* Same `jne` encoding as amd64.

## JIT (aarch64)
*Not implemented in this backend as an inline template.* Emitted via `ARM64LabelHelper::writeJne{Forward,Back}` using `writeBcc` with `JumpType::NE`:

```asm
  b.ne  target
```

## JIT (ppc64le)
*Not implemented in this backend as an inline template.* Emitted via `PPC64LabelHelper::writeJne{Forward,Back}` as `writeBCxx(4, 2, ...)` -- `BO=4, BI=2` selects "branch if CR0[EQ] clear":

```asm
  bne   cr0, target
```

## Notes

- No inline template; backend-specific branches emitted by `LabelHelper`.
- Inverse predicate of `jeq` (0xB3).
- Used heavily by `if`/`while`-style flow lowering from the front-end.
- Same short/near selection rule as `jeq`.
