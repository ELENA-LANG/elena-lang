public sealed EventHandler<TF>
{
   TF[] functions;

   constructor(TF f)
   {
      functions := new TF[](1);
      functions[0] := f
   }

   append(TF func)
   {
      int len := functions.Length;
        
      TF[] fs := new TF[](len + 1);

      for (int i := 0; i < len; i += 1) {
         fs[i] := this functions[i]
      };
        
      fs[len] := func;
        
      this functions := fs
   }
    
   dispatch() : external(system'core_routines'__event_dispatcher);
}

// --- event ---

public field event<name, T>
{
   EventHandler<T> name;

   set name(T f)
   {
      if (nil == name)
      {
         name := f
      }
      else
      {
         name.append(f);
      }
   }   
}
