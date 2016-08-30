// --- Predefined References  --
define GC_ALLOC	            10001h
define HOOK                 10010h
define INIT_RND             10012h
define INIT                 10013h
define NEWFRAME             10014h
define INIT_ET              10015h
define ENDFRAME             10016h
define RESTORE_ET           10017h
define OPENFRAME            10019h
define CLOSEFRAME           1001Ah
define NEWTHREAD            1001Bh
define CLOSETHREAD          1001Ch
define EXIT                 1001Dh
define LOCK                 10021h
define UNLOCK               10022h
define LOAD_CALLSTACK       10024h
define NEW_HEAP             10025h
define BREAK                10026h
define PREPARE              10027h
define EXPAND_HEAP          10028h
define EXITTHREAD           1002Ah
define NEW_EVENT            10101h

define CORE_OS_TABLE        20009h

define GC_HEAP_ATTRIBUTE    00Dh

structure % CORE_OS_TABLE

  dd 0 // ; dummy

end

procedure % PREPARE

  ret    // ; idle

end

// ; in - ecx - total size, ebx - reserved
// ; out - eax - heap
procedure % NEW_HEAP

  push 4
  push 00001000h
  push ebx

  push 4
  push 00002000h
  push ecx
  push 0
  call extern 'dlls'KERNEL32.VirtualAlloc

  push eax
  call extern 'dlls'KERNEL32.VirtualAlloc

  ret

end

// ; in - eax - heap, ebx - size
// ; out - eax - heap
procedure % EXPAND_HEAP

  push 4
  push 00001000h
  push ecx
  push eax
  call extern 'dlls'KERNEL32.VirtualAlloc

  ret

end

// ; ebx - exception code
procedure % BREAK

  push 0
  push 0
  push 1
  push ebx
  call extern 'dlls'KERNEL32.RaiseException

end

procedure % INIT_RND

  sub  esp, 8h
  mov  eax, esp
  sub  esp, 10h
  lea  ebx, [esp]
  push eax 
  push ebx
  push ebx
  call extern 'dlls'KERNEL32.GetSystemTime
  call extern 'dlls'KERNEL32.SystemTimeToFileTime
  add  esp, 10h
  pop  eax
  pop  edx
  ret
  
end

procedure % EXIT
    
  mov  eax, 0                         
  push eax
  // ; exit
  call extern 'dlls'KERNEL32.ExitProcess     

end

procedure % EXITTHREAD
  
  // ; close thread
  call code : % CLOSETHREAD

  mov  eax, 0FFFFFFFFh
  push eax
  // ; exit
  call extern 'dlls'KERNEL32.ExitThread

end

procedure % NEW_EVENT

  // ; set thread event handle
  push 0
  push 0
  push 0FFFFFFFFh // -1
  push 0
  call extern 'dlls'KERNEL32.CreateEventW  
  ret

end
