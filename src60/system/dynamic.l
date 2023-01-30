// --- Symbol ---

public sealed const struct Symbol
{
   __ptr pointer;

   private load(string name) : external(system'core_routines'__symbol_load);
   private copy(pointer ptr) : external(system'core_routines'__symbol_copy);

   /// Loads a symbol
   constructor(string name)
   {
      self.load(name);
   }   

   constructor(pointer ptr)
   {
      self.copy(ptr);
   }

   static Symbol tryLoad(string ns, string name)
   {
      pointer ptr := extern LoadSymbolByString2LA(ns, name);
      if (ptr != 0)
      {
         ^ ptr
      }
      else
      {
         ^ nil
      }
   }

   dispatch() : external(system'core_routines'__symbol_dispatcher);
}

// --- ClassReference ---

public sealed const struct ClassReference
{
   __ptr pointer;

   private load(string name) : external(system'core_routines'__classref_load);
  
   constructor(string name)
      : info("Loads a symbol")
   {
      self.load(name);
   }
   
   dispatch() : external(system'core_routines'__classref_dispatcher);
}
