// --- base inline templates ---

inline external<routine>()
{
   #include intern routine;
}

// --- class meta info ---

public inline info<descr>()
{
   #new super string descriptions;

   #let super descriptions[$name __decl] := descr;
}

// --- prop ---

public field prop<name, T>
{
   get T name()
   {
      ^ name
   }
}
