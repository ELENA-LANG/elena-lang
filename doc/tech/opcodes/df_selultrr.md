# 0xDF -- selult

- **Category:** TripleOp
- **Enum:** `ByteCode::SelULtRR`
- **Operand(s):** `selult r1, r2` (R+R)
- **Reads:** `acc -> [acc]` (4 bytes), `sp[0] -> [sp[0]]` (4 bytes)
- **Writes:** `acc`

## Semantics
`temp1 : 4 << acc; temp2 : 4 << sp[0]; acc := (temp2 < temp1) ? r1 : r2` -- unsigned 32-bit compare followed by conditional pointer select. Combines the work of `icmp 4` + a signed compare-on-unsigned select.

## JIT (amd64)
**Selection rule:** R+R command dispatched through `loadRROp` (`jitcompiler.cpp:1334`). Slot picker keys on `arg1` via `retrieveCode`; both refs are then materialised through `writeArgAddress` with `mskRef32`/`mskRef64`. amd64/x32 collapse to a single `cmovb` variant; aarch64/ppc64le emit a single `csel cc`/`isellt` form.

**Template:** load both 4-byte values, `cmp`, then `cmovb` (unsigned below) to pick between the two refs.

```asm
mov   rax, [r10]
cmp   eax, dword ptr [rbx]
mov   rcx, __ptr64_1
mov   rbx, __ptr64_2
cmovb rbx, rcx
```

## JIT (x32)
**Template:** same shape -- 32-bit `cmp` then `cmovb`.

```asm
mov   eax, [esi]
cmp   eax, [ebx]
mov   ecx, __ptr32_1
mov   ebx, __ptr32_2
cmovb ebx, ecx
```

## JIT (aarch64)
**Template:** `ldrsw + ldrsw + cmp`, then materialise both refs and `csel ... cc` (carry-clear === unsigned-below).

```asm
ldrsw   x17, [x0]
ldrsw   x18, [x10]
cmp     x17, x18

movz    x11, __ptr32lo_1
movz    x12, __ptr32lo_2
movk    x11, __ptr32hi_1, lsl #16
movk    x12, __ptr32hi_2, lsl #16

csel    x10, x11, x12, cc
```

## JIT (ppc64le)
**Template:** unsigned `cmplw`, materialise both refs from TOC, `isellt` (CR0 LT after `cmplw` == unsigned less-than).

```asm
lwz      r17, 0(r3)
lwz      r18, 0(r15)

cmplw    r17, r18

ld      r16, toc_code(r2)
addis   r17, r16, __xdisp32hi_1
addis   r18, r16, __xdisp32hi_2
addi    r17, r17, __xdisp32lo_1
addi    r18, r18, __xdisp32lo_2

isellt  r15, r17, r18
```

## Notes
- Equivalent to `icmp 4` + `sellt` but with **unsigned** semantics.
- Combines the compare and select into a single opcode -- sets COMP flags from a fresh 32-bit unsigned `cmp`/`cmplw` and immediately consumes them.
- Used for range checks and pointer-tag comparisons where values must not be interpreted as signed.
- Compare with `selgr` (0xD7) which uses signed greater-than against flags from a *previous* compare; `selult` is self-contained.
- Both refs are materialised unconditionally to avoid branch mispredictions.
