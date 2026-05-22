# 0xBA -- jle

- **Category:** DoubleOp
- **Enum:** `ByteCode::Jle`
- **Operand(s):** `jle label`
- **Reads:** `COMP.LT`, `COMP.EQ` flags
- **Writes:** `pc` (conditionally)
- **Side effects:** None.

## Semantics
Branch to `label` if `COMP.LT == 1 || COMP.EQ == 1` -- **signed** `<=` (the abstract VM text "LT == 1 && EQ == 1" reads as a conjunction but maps to the standard `<=` predicate on native flags). No inline template -- the JIT emits the conditional branch directly via `LabelHelper::writeJle{Forward,Back}`.

## Compiler behaviour
`compileJle` (`jitcompiler.cpp:3049-3060`):

```cpp
pos_t label = scope->tapeReader->position() + scope->command.arg1;
if (scope->command.arg1 < 0)      lh->writeJleBack(label, *codeWriter);
else if (scope->command.arg1 > 0) lh->writeJleForward(label, *codeWriter, scope->command.arg1);
```

Maps to signed less-or-equal:
- amd64/x32: `jle` (`ZF == 1 || SF != OF`)
- aarch64: `b.le` (`writeBcc(JumpType::LE)`)
- ppc64le: `ble cr0` (`writeBCxx(BO=4, BI=1)`)

## JIT (amd64)
*Not implemented in this backend as an inline template.* Emitted via `X86LabelHelper` as `X86JumpType::JLE` (ZF == 1 || SF != OF):

```asm
  ; short:  7E ib        ; jle rel8
  ; near:   0F 8E id     ; jle rel32
```

## JIT (x32)
*Not implemented in this backend as an inline template.* Same `jle` encoding as amd64.

## JIT (aarch64)
*Not implemented in this backend as an inline template.* Emitted via `ARM64LabelHelper::writeJle{Forward,Back}` using `writeBcc` with `JumpType::LE`:

```asm
  b.le  target
```

## JIT (ppc64le)
*Not implemented in this backend as an inline template.* Emitted via `PPC64LabelHelper::writeJle{Forward,Back}` as `writeBCxx(4, 1, ...)` -- `BO=4, BI=1` selects "branch if CR0[GT] clear":

```asm
  ble   cr0, target
```

## Notes

- **Signed** less-or-equal.
- Inverse: `jgr` (0xB9).
- The abstract-VM `LT == 1 && EQ == 1` phrasing is non-standard but the predicate is the canonical signed `<=`.
- No inline template; emitted by `LabelHelper`.
