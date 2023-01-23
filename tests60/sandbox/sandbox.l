import extensions;

inline xexternal(routine)
{
   #include intern routine;
}

public sealed class XCallStack
   : info("A call stack")
{
   pointer[] stack;

   private load(pointer[] stack, int length, ref int retVal) : xexternal(system'core_routines'__loadCallStack);

   constructor()
   {
      pointer stack[64];
      self.load(stack, 64, ref int length);

      this stack := stack;
   }
/*
   string toPrintable()
      : info("Generates the call stack literal representation")
   {
      byte    buffer[512];
      int     bufferLength := 0; 
      pointer address;
      string  callStackText := emptyString;
      int     length := stack.length;
      for (int i := 0, i < length, i += 1) {
         address := stack[i];

         bufferLength := extern elenart.LoadAddressInfoLM(address, buffer, 512);
         if (bufferLength > 0) {
            callStackText := callStackText + String.fromByteArray(0, bufferLength, buffer).add(newLine)
         }
      };

      ^ callStackText
   }*/
}

public program()
{
   //var s := new XCallStack();

   var s := newLine

   //console.writeLine("Test from ELENAVM")
}




