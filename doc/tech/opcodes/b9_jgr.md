# 0xB9 -- jgr

- **Category:** DoubleOp
- **Enum:** `ByteCode::Jgr`
- **Operand(s):** `jgr label`
- **Reads:** `COMP.LT`, `COMP.EQ` flags
- **Writes:** `pc` (conditionally)
- **Side effects:** None.

## Semantics
Branch to `label` if `COMP.LT == 0 && COMP.EQ == 0` -- **strictly greater** (signed). Use `jgr` for `>`, `jge` (0xB8) for `>=`. No inline template -- the JIT emits the conditional branch directly via `LabelHelper::writeJgr{Forward,Back}`.

## Compiler behaviour
`compileJgr` (`jitcompiler.cpp:3062-3073`):

```cpp
pos_t label = scope->tapeReader->position() + scope->command.arg1;
if (scope->command.arg1 < 0)      lh->writeJgrBack(label, *codeWriter);
else if (scope->command.arg1 > 0) lh->writeJgrForward(label, *codeWriter, scope->command.arg1);
```

`COMP.LT == 0 && COMP.EQ == 0` maps to signed strictly-greater:
- amd64/x32: `jg` (`ZF == 0 && SF == OF`)
- aarch64: `b.gt` (`writeBcc(JumpType::GT)`)
- ppc64le: `bgt cr0` (`writeBCxx(BO=12, BI=1)`)

## JIT (amd64)
*Not implemented in this backend as an inline template.* Emitted via `X86LabelHelper` as `X86JumpType::JG` (ZF == 0 && SF == OF):

```asm
  ; short:  7F ib        ; jg rel8
  ; near:   0F 8F id     ; jg rel32
```

## JIT (x32)
*Not implemented in this backend as an inline template.* Same `jg` encoding as amd64.

## JIT (aarch64)
*Not implemented in this backend as an inline template.* Emitted via `ARM64LabelHelper::writeJgr{Forward,Back}` using `writeBcc` with `JumpType::GT`:

```asm
  b.gt  target
```

## JIT (ppc64le)
*Not implemented in this backend as an inline template.* Emitted via `PPC64LabelHelper::writeJgr{Forward,Back}` as `writeBCxx(12, 1, ...)` -- `BO=12, BI=1` selects "branch if CR0[GT] set":

```asm
  bgt   cr0, target
```

## Notes

- **Signed** strictly-greater.
- Inverse: `jle` (0xBA).
- No inline template; emitted by `LabelHelper`.
- Same short/near selection as the rest of the conditional branch family.
