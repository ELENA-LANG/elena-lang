# 0xD7 -- selgr

- **Category:** TripleOp
- **Enum:** `ByteCode::SelGrRR`
- **Operand(s):** `selgr r1, r2` (R+R)
- **Reads:** `COMP` flags (set by a preceding compare)
- **Writes:** `acc`

## Semantics
`acc := COMP.GT ? r1 : r2` -- conditional select on signed greater-than. (The spec text says "`COMP.LE`" but the implementations all use the signed-greater predicate; the predicate is whichever the preceding compare set as "left > right".)

## JIT (amd64)
**Selection rule:** R+R command dispatched through `loadRROp` (`jitcompiler.cpp:1334`). The slot picker keys on `arg1` (`retrieveCode`); both ref kinds are resolved through `writeArgAddress` with `mskRef64`/`mskRef32` according to the host. amd64/x32 collapse to a single `cmovg` variant.

**Template:** `cmovg` chooses between the two materialised pointers.

```asm
mov   rax, __ptr64_1
mov   rbx, __ptr64_2
cmovg rbx, rax
```

## JIT (x32)
**Template:** same `cmovg` idiom.

```asm
mov   eax, __ptr32_1
mov   ebx, __ptr32_2
cmovg ebx, eax
```

## JIT (aarch64)
**Template:** materialise both refs, `csel ... gt`.

```asm
movz    x11, __ptr32lo_1
movz    x12, __ptr32lo_2
movk    x11, __ptr32hi_1, lsl #16
movk    x12, __ptr32hi_2, lsl #16

csel    x10, x11, x12, gt
```

## JIT (ppc64le)
**Template:** materialise both refs from TOC + 32-bit displacements, `iselgt`.

```asm
ld      r16, toc_code(r2)
addis   r17, r16, __xdisp32hi_1
addis   r18, r16, __xdisp32hi_2
addi    r17, r17, __xdisp32lo_1
addi    r18, r18, __xdisp32lo_2

iselgt  r15, r17, r18
```

## Notes
- cmov-style: selects between two refs based on COMP flags set by an earlier compare opcode (`cmp`, `xcmp`, etc.).
- Predicate is signed greater-than (`cmovg` / `csel gt` / `iselgt`).
- Sibling of `sellt r1, r2` (less-than) and `selult r1, r2` (0xDF -- unsigned).
- Both refs are materialised regardless of which branch is taken -- there is no short-circuit; this is intentional to avoid branch mispredictions.
