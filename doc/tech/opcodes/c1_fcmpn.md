# 0xC1 -- fcmp n

- **Category:** DoubleOp
- **Enum:** `ByteCode::FCmpN`
- **Operand(s):** `fcmp n` (n = 8 only)
- **Reads:** `acc -> [acc]`, `sp[0] -> [sp[0]]` (both 8-byte doubles)
- **Writes:** `COMP.EQ`, `COMP.LT`

## Semantics
Load `double:[acc]` and `double:[sp[0]]`, compare, set `COMP.EQ := (sp[0] == acc)` and `COMP.LT := (sp[0] < acc)`. Only `n = 8` (binary64) is defined.

## JIT (amd64)
**Selection rule:** dispatched through `loadDPNOp` -> `retrieveICode(scope, arg2)` (`jitcompiler.cpp:2399`). The width selector picks slot 4 (n=8) since `FCmpN` is defined only for double; effectively a single-variant opcode in this branch.

**Template:** x87 `fcomip` sequence; result materialised into integer flags via `cmp eax, ecx`.

```asm
mov    rsi, r10
xor    eax, eax
fld    qword ptr [rbx]
mov    ecx, 1
fld    qword ptr [rsi]
fcomip st, st(1)
sete   al
seta   ah
fstp   st(0)
cmp    eax, ecx
```

## JIT (x32)
**Template:** same x87 sequence; `sp[0]` shadow already lives in `esi`.

```asm
xor    eax, eax
fld    qword ptr [ebx]
mov    ecx, 1
fld    qword ptr [esi]
fcomip st, st(1)
sete   al
seta   ah
fstp   st(0)
cmp    eax, ecx
```

## JIT (aarch64)
**Template:** load both doubles into NEON FP registers, `fcmp` -- sets ARM NZCV flags directly usable by `jeq/jlt`.

```asm
ldr     d17, [x0]
ldr     d18, [x10]
fcmp    d17, d18
```

## JIT (ppc64le)
**Template:** `lfd` + `fcmpu` -- IEEE-754 unordered compare.

```asm
lfd      f17, 0(r3)
lfd      f18, 0(r15)
fcmpu    f17, f18
```

## Notes
- IEEE-754 double-precision compare (`n = 8` only); narrower FP widths are not emitted.
- On amd64/x32 the trailing `cmp eax, ecx` re-materialises x87 condition bits into integer EFLAGS so the following `jeq/jne/jlt/...` (which read EFLAGS) work uniformly.
- aarch64/ppc64le set their condition flags directly through `fcmp`/`fcmpu`.
- Unordered NaN comparisons follow host semantics (typically set both EQ and LT to false).
