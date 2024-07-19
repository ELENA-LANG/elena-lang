define INVOKER           10001h

define CORE_SINGLE_CONTENT  2000Bh

define tt_stack_frame        0008h
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

  // ; check if it is a nested call
  mov  ecx, [data : %CORE_SINGLE_CONTENT + tt_stack_root]
  test ecx, ecx
  jnz  labNested

  // ; declare new frame
  push 0              // ; FrameHeader.previousFrame
  push 0              // ; FrameHeader.reserved
  mov  ebp, esp       // ; FrameHeader

  mov  [data : %CORE_SINGLE_CONTENT + tt_stack_root], esp  
  mov  [data : %CORE_SINGLE_CONTENT + tt_stack_frame], esp  

  push esi            // ; arg

  call eax
  add  esp, 12        // ; clear FrameHeader+arg
  mov  eax, ebx

  // ; clear root
  xor  ecx, ecx
  mov  [data : %CORE_SINGLE_CONTENT + tt_stack_root], ecx
  mov  [data : %CORE_SINGLE_CONTENT + tt_stack_frame], ecx

  jmp  labEnd

labNested:

  push esi            // ; arg
  call eax
  add  esp, 4         // ; clear arg

labEnd:

  // ; restore registers
  pop  ebp
  pop  ebx
  pop  ecx
  pop  edi
  pop  esi
  ret

end
