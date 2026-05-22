# 0xA4 -- xflush sp

- **Category:** DoubleOp
- **Enum:** `ByteCode::XFlushSI`
- **Operand(s):** `xflush sp:i`
- **Reads:** `cached_sp[i]` (per-arch register cache)
- **Writes:** `sp[i]` (memory)
- **Side effects:** None.

## Semantics
`sp[i] := cached_sp[i]` -- flush the register-cached top-of-stack slot back to the actual stack. Used before any operation that inspects stack memory directly so the cached value becomes visible at `[sp + i*ptr]`. Paired with `xrefresh sp:i` (0xA7).

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %0A4h`

```asm
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0A4h` | `xflush sp:i` (i > 1) | empty (no slot cached) |
| `%1A4h` | `sp:0`                | `mov [rsp+i], r10` |
| `%2A4h` | `sp:1`                | `mov [rsp+i], r11` |

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0A4h`

```asm
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0A4h` | `xflush sp:i` (i > 0) | empty |
| `%1A4h` | `sp:0`                | `mov [esp+i], esi` |

x32 has only one cached slot (`sp[0]` in `esi`).

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0A4h`

```asm
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0A4h` | `xflush sp:i` (i > 1) | empty |
| `%1A4h` | `sp:0`                | `str x0, [sp]` |
| `%2A4h` | `sp:1`                | `add x11, sp, 8; str x1, [x11]` |

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0A4h`

```asm
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0A4h` | `xflush sp:i` (i > 1) | empty |
| `%1A4h` | `sp:0`                | `std r3, 0(r1)` |
| `%2A4h` | `sp:1`                | `std r4, 8(r1)` |

## Notes

- Pure no-op for non-cached slots (`i > 1` on most backends, `i > 0` on x32); the JIT still emits the opcode for safety because the bytecode writer does not know which slots are cached on each target.
- Emitted by the compiler before any `xcall`/`callr`/runtime helper that may inspect stack memory through a different path than the cache.
- Always paired with `xrefresh sp:i` (0xA7) on the return side.
- Does not touch `acc` or `index`.
