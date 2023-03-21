import system;
import system'runtime;

// --- ThreadHandle ---
public sealed struct ThreadHandle
{
   Handle  _handle;
   pointer _actionPtr;

   private handle createThreadHandle(pointer arg, int flags) : external(system'core_routines'__thread_create);

   constructor(Func f)
   {
      _actionPtr := PermVectorTable.allocate(f);

      _handle := self.createThreadHandle(_actionPtr, 4);

      if (_handle.isUnassigned()) {
         InvalidOperationException.raise();
      }
   }
}
