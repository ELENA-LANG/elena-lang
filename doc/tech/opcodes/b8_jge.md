# 0xB8 -- jge

- **Category:** DoubleOp
- **Enum:** `ByteCode::Jge`
- **Operand(s):** `jge label`
- **Reads:** `COMP.LT`, `COMP.EQ` flags
- **Writes:** `pc` (conditionally)
- **Side effects:** None.

## Semantics
Branch to `label` if `COMP.LT == 0 && COMP.EQ == 1` (per `bytecode60.txt`). On native flags this is plain signed `>=` -- when `cmp` is the previous flag-setter the `EQ == 1` clause is automatically covered by the GE condition. No inline template -- the JIT emits the conditional branch directly via `LabelHelper::writeJge{Forward,Back}`.

## Compiler behaviour
`compileJge` (`jitcompiler.cpp:3036-3047`):

```cpp
pos_t label = scope->tapeReader->position() + scope->command.arg1;
if (scope->command.arg1 < 0)      lh->writeJgeBack(label, *codeWriter);
else if (scope->command.arg1 > 0) lh->writeJgeForward(label, *codeWriter, scope->command.arg1);
```

`COMP.LT == 0 && COMP.EQ == 1` collapses to native signed `>=`:
- amd64/x32: `jge` (`SF == OF`)
- aarch64: `b.ge` (`writeBcc(JumpType::GE)`)
- ppc64le: `bge cr0` (`writeBCxx(BO=4, BI=0)`)

## JIT (amd64)
*Not implemented in this backend as an inline template.* Emitted via `X86LabelHelper` as `X86JumpType::JGE` (SF == OF):

```asm
  ; short:  7D ib        ; jge rel8
  ; near:   0F 8D id     ; jge rel32
```

## JIT (x32)
*Not implemented in this backend as an inline template.* Same `jge` encoding as amd64.

## JIT (aarch64)
*Not implemented in this backend as an inline template.* Emitted via `ARM64LabelHelper::writeJge{Forward,Back}` using `writeBcc` with `JumpType::GE`:

```asm
  b.ge  target
```

## JIT (ppc64le)
*Not implemented in this backend as an inline template.* Emitted via `PPC64LabelHelper::writeJge{Forward,Back}` as `writeBCxx(4, 0, ...)` -- `BO=4, BI=0` selects "branch if CR0[LT] clear":

```asm
  bge   cr0, target
```

## Notes

- **Signed** greater-or-equal.
- The bytecode-level definition uses two flags (`LT==0 && EQ==1`) but on native flags the standard `jge`/`b.ge`/`bge` already implements the same semantic for the common `cmp`-then-branch pattern.
- Inverse: `jlt` (0xB7).
- No inline template; emitted by `LabelHelper`.
