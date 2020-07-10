// --- Predefined References  --
define GC_ALLOC	            10001h
define HOOK                 10010h
define INVOKER              10011h
define INIT_RND             10012h
define ENDFRAME             10016h
define RESTORE_ET           10017h
define OPENFRAME            10019h
define CLOSEFRAME           1001Ah
define LOCK                 10021h
define UNLOCK               10022h
define LOAD_CALLSTACK       10024h
define NEW_HEAP             10025h
define BREAK                10026h
define EXPAND_HEAP          10028h
define EXITTHREAD           1002Ah
define NEW_EVENT            10101h

define PROT_READ_WRITE      03h
define MAP_ANONYMOUS        22h
define SIGABRT              06h

// ; in - ecx - total size, ebx - reserved
// ; out - eax - heap
procedure % NEW_HEAP

  // ; addr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
  // ;              MAP_ANONYMOUS, -1, 0);  
  push 0
  push 0FFFFFFFFh
  push MAP_ANONYMOUS
  push PROT_READ_WRITE
  push ecx
  push 0
  //call extern : "libc.so.6.mmap"
  add  esp, 24
  ret

end

// ; in - eax - heap, ebx - size
// ; out - eax - heap
procedure % EXPAND_HEAP

  ret

end

// ; ebx - exception code
procedure % BREAK

  // ; SIGABRT
  push SIGABRT
 // call extern : "libc.so.6.raise"

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

// INVOKER(prevFrame, function)
procedure % INVOKER

  // ; save registers
  mov  eax, [esp+8]   // ; function
  push esi
  mov  esi, [esp+8]   // ; prevFrame
  push edi
  push ecx
  push ebx
  push ebp

  // declare new frame
  push esi            // ; FrameHeader.previousFrame
  push 0              // ; FrameHeader.reserved
  mov  ebp, esp       // ; FrameHeader
  call eax
  add  esp, 8         // ; clear FrameHeader

  // ; restore registers
  pop  ebp
  pop  ebx
  pop  ecx
  pop  edi
  pop  esi
  ret

end
