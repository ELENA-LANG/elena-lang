// --- base inline templates ---

public inline external(routine)
{
   #include intern routine;
}

// --- class meta info ---

public inline info(descr)
{
   #new super string descriptions;

   #let super descriptions[$name __decl] := descr;
}

// --- preloaded ---

public inline preloaded()
{
   #new __string preloadedSymbols;

   #let preloadedSymbols += __decl;
}

// --- prop ---

public field prop<name, T>
{
   get T name()
   {
      ^ name
   }

   set name(T value)
   {
      name := value
   }
}

public field prop<name>
{
   get name()
   {
      ^ name
   }

   set name(value)
   {
      name := value
   }
}

// --- rprop ---

public field rprop<name, T>
{
   get T name()
   {
      ^ name
   }
}

public field rprop<name>
{
   get name()
   {
      ^ name
   }
}

// --- dto_prop ---

public field dto_prop<name, T>
{
   get T name()
   {
      ^ name
   }

   set name(T value)
   {
      name := value
   }

   const subject[] __dto_properties;

   this __dto_properties += mssg name;

   static MessageName[] __dto_properties()
   {
      ^ __dto_properties
   }
}

// --- interface<T> ---

public weak interface<T>
{
   T cast() = new T
   {
      embeddable dispatch() => self;
   };
}
