// --- base inline templates ---

inline external<routine>()
{
   #include intern routine;
}

public inline __included<routine>()
{
   #let routine[$name __decl] := __decl;
}

