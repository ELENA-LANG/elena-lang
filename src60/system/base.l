// --- base inline templates ---

inline external<routine>()
{
   #include intern routine;
}

// --- class meta info ---

public inline info<descr>()
{
   #new super descriptions;

   #let descriptions[$name __decl] := descr;
}

// --- prop ---

public field prop<name, T>
{
   get T name()
   {
      ^ name
   }
}
