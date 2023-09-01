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
        
      TF[] functions := new TF[](len + 1);

      for (int i := 0, i < len, i += 1) {
         functions[i] := this functions[i]
      };
        
      functions[len] := func;
        
      this functions := functions
   }
    
   dispatch() : external(system'core_routines'__event_dispatcher);
}

// --- event ---

public field event<name, T>
{
   override EventHandler<T> name;

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
