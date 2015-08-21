// --- Predefined References  --
define GC_ALLOC	            10001h
define HOOK                 10010h
define INIT_RND             10012h
define INIT                 10013h
define NEWFRAME             10014h
define INIT_ET              10015h
define ENDFRAME             10016h
define RESTORE_ET           10017h
define LOAD_CLASSNAME       10018h
define OPENFRAME            10019h
define CLOSEFRAME           1001Ah
define NEWTHREAD            1001Bh
define CLOSETHREAD          1001Ch
define EXIT                 1001Dh
define CALC_SIZE            1001Eh
define SET_COUNT            1001Fh
define GET_COUNT            10020h
define LOCK                 10021h
define UNLOCK               10022h
define LOAD_ADDRESSINFO     10023h
define LOAD_CALLSTACK       10024h
define NEW_HEAP             10025h
define BREAK                10026h
define PREPARE              10027h
define EXITTHREAD           1002Ah
define NEW_EVENT            10101h

define CORE_OS_TABLE        20009h

define PROT_READ_WRITE      03h
define MAP_ANONYMOUS        22h
define SIGABRT              06h

structure % CORE_OS_TABLE

  dd 0 // ; vargs       : +x00   - program command line

end

procedure % PREPARE

  lea  ebx, [esp + 04h]
  mov  [data : %CORE_OS_TABLE], ebx
  ret  

end

// ; in - eax - total size
// ; out - eax - heap
procedure % NEW_HEAP

  // ; addr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
  // ;              MAP_ANONYMOUS, -1, 0);  
  push 0
  push 0FFFFFFFFh
  push MAP_ANONYMOUS
  push PROT_READ_WRITE
  push eax
  push 0
  call extern : "libc.so.6.mmap"
  add  esp, 24
  ret

end

// ; ebx - exception code
procedure % BREAK

  // ; SIGABRT
  push SIGABRT
  call extern : "libc.so.6.raise"

end

procedure % INIT_RND

/*
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
*/
  // ; !  temporally
  xor  eax, eax
  xor  edx, edx  

  ret
  
end

procedure % EXIT
  
  mov  eax, 1
  mov  ebx, 0
  int  80h

end


procedure % EXITTHREAD
  
  ret

end

procedure % NEW_EVENT

  ret

end
