// --- Handle ---

public sealed const struct Handle
{
   embeddable __word value[4];

   constructor(int intValue)
   {
      value := intValue
   }
}

// --- UnsafePointer ---

public sealed const struct UnsafePointer
{
   embeddable __ptr pointer;
}

// --- CallStack ---

public sealed class CallStack
   : info("A call stack")
{
   pointer[] stack;

   private load(pointer[] stack, int length, ref int retVal) : external(system'core_routines'__loadCallStack);

   constructor()
   {
      pointer stack[64];
      self.load(stack, 64, ref int length);

      this stack := stack;
   }
}
