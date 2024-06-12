import system;
import system'runtime;

public const int INFINITE = 0FFFFFFFFh;

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

   start()
   {
      extern KERNEL32.ResumeThread(_handle)
   }

   join()
      <= join(INFINITE);
                                                                   
   join(int timeOut)
   {
      extern KERNEL32.WaitForSingleObject(_handle, timeOut)
   }     
                                                                   
   close()
   {
      extern KERNEL32.CloseHandle(_handle)
   }
}

public singleton threadControl
{
   sleep(int interval)
   {
      extern
      {
         extern KERNEL32.Sleep(interval)
      }
   }
}
