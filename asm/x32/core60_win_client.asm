define INVOKER           10001h

define CORE_SINGLE_CONTENT  2000Bh

define tt_stack_root         0014h

// INVOKER(function, arg)
procedure % INVOKER

  // ; save registers
  mov  eax, [esp+4]   // ; function
  push esi
  mov  esi, [esp+12]  // ; arg
  push edi
  push ecx
  push ebx
  push ebp

  // ; declare new frame
  push 0              // ; FrameHeader.previousFrame
  push 0              // ; FrameHeader.reserved
  mov  ebp, esp       // ; FrameHeader

  mov  [data : %CORE_SINGLE_CONTENT + tt_stack_root], esp  

  push esi            // ; arg

  call eax
  add  esp, 12        // ; clear FrameHeader+arg
  mov  eax, ebx

  // ; restore registers
  pop  ebp
  pop  ebx
  pop  ecx
  pop  edi
  pop  esi
  ret

end
