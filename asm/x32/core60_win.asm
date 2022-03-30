// ; --- Predefined References  --
define INVOKER         10001h

define CORE_TOC        20001h
define SYSTEM_ENV      20002h

// ; ==== System commands ===

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
  push esi            // ; arg

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
