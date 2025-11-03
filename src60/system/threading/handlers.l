import system'runtime;

#if (__project["_Win"]) // Windows

public const int INFINITE = 0FFFFFFFFh;

public const int WAIT_TIMEOUT  = 00000102h;
public const int WAIT_OBJECT_0 = 0;

public const int THREAD_PRIORITY_HIGHEST = 2;
public const int THREAD_PRIORITY_ABOVE_NORMAL = 1;
public const int THREAD_PRIORITY_NORMAL = 0;
public const int THREAD_PRIORITY_BELOW_NORMAL = -1;
public const int THREAD_PRIORITY_LOWEST = -2;

#endif

// --- ThreadHandle ---
public sealed struct ThreadHandle
{
   Handle  _handle;
   pointer _actionPtr;

   private handle createThreadHandle(pointer arg, int flags, int stackSafe) : external(system'core_routines'__thread_create);

   private create(int stackSize)
   {
      _handle := self.createThreadHandle(_actionPtr, 4, stackSize);

      if (_handle.isUnassigned()) {
         InvalidOperationException.raise();
      }
   }

#if (__project["_Win"]) // Windows

   constructor(object f)
   {
      _actionPtr := PermVectorTable.allocate(f);

      create(0)
   }

   constructor(object f, int stackSize)
   {
      _actionPtr := PermVectorTable.allocate(f);

      create(stackSize)
   }

   private ThreadPriority toPriority(int win_priority)
   {
      if (win_priority == THREAD_PRIORITY_HIGHEST)
         { ^ ThreadPriority.Highest }
      else if (win_priority == THREAD_PRIORITY_ABOVE_NORMAL)
         { ^ ThreadPriority.AboveNormal }
      else if (win_priority == THREAD_PRIORITY_NORMAL)
         { ^ ThreadPriority.Normal }
      else if (win_priority == THREAD_PRIORITY_BELOW_NORMAL)
         { ^ ThreadPriority.BelowNormal }
      else if (win_priority == THREAD_PRIORITY_LOWEST)
         { ^ ThreadPriority.Lowest };

      ^ ThreadPriority.Lowest
   }

   private int fromPriority(ThreadPriority priority)
   {
      if (priority == THREAD_PRIORITY_HIGHEST)
         { ^ ThreadPriority.Highest }
      else if (priority == THREAD_PRIORITY_ABOVE_NORMAL)
         { ^ ThreadPriority.AboveNormal }
      else if (priority == THREAD_PRIORITY_NORMAL)
         { ^ ThreadPriority.Normal }
      else if (priority == ThreadPriority.BelowNormal)
         { ^ THREAD_PRIORITY_BELOW_NORMAL }
      else if (priority == ThreadPriority.Lowest)
         { ^ THREAD_PRIORITY_LOWEST };

      ^ THREAD_PRIORITY_LOWEST
   }

   constructor()
   {
      _handle := extern KERNEL32.GetCurrentThread();
   }

   ThreadPriority getPriority()
   {
      int win_priority := extern KERNEL32.GetThreadPriority(_handle);

      ^ toPriority(win_priority);
   }

   setPriority(ThreadPriority priority)
   {
      int win_priority := fromPriority(priority);
   
      extern KERNEL32.SetThreadPriority(_handle, win_priority);
   }

   start()
   {
      extern KERNEL32.ResumeThread(_handle)
   }

   join()
      <= join(INFINITE);
                                                                   
   join(int timeOut)
   {
      extern {
         extern KERNEL32.WaitForSingleObject(_handle, timeOut);
      }
   }     
   
   bool checkIfAlive()
   {
      int retVal := extern KERNEL32.WaitForSingleObject(_handle, 0);

      ^ retVal == WAIT_TIMEOUT;
   }                                                                

   close()
   {
      extern KERNEL32.CloseHandle(_handle)
   }

   static sleep(int interval)
   {
      extern
      {
         extern KERNEL32.Sleep(interval)
      }
   }

#elif (__project["_Linux"] || __project["_FreeBSD"])

   int _stackSize;

   constructor(object f)
   {
      _actionPtr := PermVectorTable.allocate(f);
      _stackSize := 0
   }

   constructor(object f, int stackSize)
   {
      _actionPtr := PermVectorTable.allocate(f);
      _stackSize := stackSize
   }

   constructor()
   {
      NotSupportedException.raise()
   }

   ThreadPriority getPriority()
   {
      NotSupportedException.raise()
   }

   setPriority(ThreadPriority priority)
   {
      NotSupportedException.raise()
   }

   start()
   {
      create(_stackSize)
   }

   join()
   {
      extern
      {
         extern libc.pthread_join(_handle);
      }
   }
                                                                   
   join(int timeOut)
   {
      NotSupportedException.raise()
   }     
   
   bool checkIfAlive()
   {
      NotSupportedException.raise()
   }                                                                

   close()
   {
      _handle := default;
   }

   static sleep(int interval)
   {
      extern
      {
         extern libc.sleep(interval);
      }
   }

#endif

}
