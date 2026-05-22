# 0xA7 -- xrefresh sp

- **Category:** DoubleOp
- **Enum:** `ByteCode::XRefreshSI`
- **Operand(s):** `xrefresh sp:i`
- **Reads:** `sp[i]` (memory)
- **Writes:** `cached_sp[i]` (per-arch register cache)
- **Side effects:** None.

## Semantics
`cached_sp[i] := sp[i]` -- reload the register-cached top-of-stack slot from real stack memory. Companion of `xflush sp:i` (0xA4); the flush/refresh pair brackets any operation whose stack-memory effects the JIT cannot model in the cache.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %0A7h`

```asm
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0A7h` | `xrefresh sp:i` (i > 1) | empty |
| `%1A7h` | `sp:0`                  | `mov r10, [rsp+i]` |
| `%2A7h` | `sp:1`                  | `mov r11, [rsp+i]` |

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0A7h`

```asm
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0A7h` | `xrefresh sp:i` (i > 0) | empty |
| `%1A7h` | `sp:0`                  | `mov esi, [esp+i]` |

x32 caches only `sp[0]`.

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0A7h`

```asm
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0A7h` | `xrefresh sp:i` (i > 1) | empty |
| `%1A7h` | `sp:0`                  | `ldr x0, [sp]` |
| `%2A7h` | `sp:1`                  | `add x11, sp, 8; ldr x1, [x11]` |

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0A7h`

```asm
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0A7h` | `xrefresh sp:i` (i > 1) | empty |
| `%1A7h` | `sp:0`                  | `ld r3, 0(r1)` |
| `%2A7h` | `sp:1`                  | `ld r4, 8(r1)` |

## Notes

- Inverse of `xflush sp:i` (0xA4). The flush/refresh pair brackets any opcode whose stack-memory effect the JIT cache cannot model.
- No-op for non-cached slots; the bytecode writer emits it uniformly because cache layout is target-specific.
- Cached slot mapping: amd64 `r10`/`r11`, x32 `esi`, aarch64 `x0`/`x1`, ppc64le `r3`/`r4`.
- Does not touch `acc` or `index`.
