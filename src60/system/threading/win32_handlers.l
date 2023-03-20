import system;
import system'runtime;

// --- ThreadHandle ---
public sealed struct ThreadHandle
{
   Handle  _handle;
   pointer _actionPtr;

   constructor(Func f)
   {
      //_actionPTr := PermVectorTable.allocate(f);
   }
}
