// --- Predefined References  --
define GC_ALLOC	            10001h
define HOOK                 10010h
define INVOKER              10011h
define INIT_RND             10012h
define INIT_ET              10015h
define LOCK                 10021h
define UNLOCK               10022h
define LOAD_CALLSTACK       10024h

define GC_HEAP_ATTRIBUTE    00Dh

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

// INVOKER(prevFrame, function, arg)
procedure % INVOKER

  // ; save registers
  mov  eax, [esp+8]   // ; function
  push esi
  mov  esi, [esp+8]   // ; prevFrame
  push edi
  mov  edi, [esp+20]  // ; arg
  push ecx
  push ebx
  push ebp

  // declare new frame
  push esi            // ; FrameHeader.previousFrame
  push 0              // ; FrameHeader.reserved
  mov  ebp, esp       // ; FrameHeader
  push edi            // ; arg

  call eax
  add  esp, 12        // ; clear FrameHeader+arg
  xor  eax, eax

  // ; restore registers
  pop  ebp
  pop  ebx
  pop  ecx
  pop  edi
  pop  esi
  ret

end
