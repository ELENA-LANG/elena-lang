import system;
import system'collections;

// --- DynamicTape ---

public mixin DynamicTape
{
   __string TapeFunction[] list;

   constructor(params TapeFunction[] args)
      <= load(params args);

   internal constructor allocate(int len)
      = new TapeFunction[](len);
   
   constructor load(params TapeFunction[] args)
      <= allocate(args.Length)
   {
      int i := args.Length;
      while (i > 0)
      {
         i -= 1;
            
         list[i] := args[i]
      }
   }

   constructor load(List<TapeFunction> args)
      <= allocate(args.Length)
   {
      int i := args.Length;
      while (i > 0)
      {
         i -= 1;
            
         list[i] := args[i]
      }
   }
        
   constructor load(List<TapeFunction> args, int index, int length)        
      <= allocate(length)
   {
      int i := index;
      int c := 0;
      while (c < length)
      {
         list[c] := args[i];

         c += 1;
         i += 1
      }
   }
        
   constructor load(List<object> args, int index, int length)        
      <= allocate(length)
   {
      int i := index;
      int c := 0;
      while (c < length)
      {
         list[c] := args[i];

         c += 1;
         i += 1
      }
   }

   dispatch() : external(system'core_routines'__closuretape);
}

// --- DynamicClass ---

public mixin DynamicClass
{
   __string object[] items;
    
   internal constructor allocate(int n)
        = new object[](n);
    
   constructor create(List<Tuple<Message,DynamicTape>> methods)
      <= allocate(methods.Length * 2)
   {
      int length := methods.Length;
      for (int i := 0, i < length, i += 1)
      {
         var t := methods[i];
            
         var o1 := t.Item1;
         var o2 := t.Item2;

         items[i*2]:= o1;
            
         int k := i*2 + 1;
         items[k]:= o2;
      }
   }
    
   dispatch() : external(system'core_routines'__closureclass);
}

// --- StreamTapeInterpreter ---

public singleton StreamTapeInterpreter
{
   build(byte[] buffer, int len) : external(system'core_routines'__tape_eval);
}