// --- base inline templates ---

inline external<routine>()
{
   #include intern routine;
}

// --- class meta info ---

// --- prop ---

public field prop<name, T>
{
   get T name()
   {
      ^ name
   }
}
