import system;
import system'collections;

// --- DynamicTape ---

public mixin DynamicTape
{
   __string TapeFunction[] list;

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

// --- StreamTapeInterpreter ---

public singleton StreamTapeInterpreter
{
   build(byte[] buffer, int len) : external(system'core_routines'__tape_eval);
}