// ; --- Predefined References  --
define INVOKER         10001h

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

  xor  edi, edi 
  // ; declare new frame
  push edi            // ; FrameHeader.previousFrame
  push edi            // ; FrameHeader.reserved
  mov  ebp, esp       // ; FrameHeader
  push edi   
  push esi            // ; arg

  call eax
  add  esp, 16        // ; clear FrameHeader+arg
  xor  eax, eax

  // ; restore registers
  pop  ebp
  pop  ebx
  pop  ecx
  pop  edi
  pop  esi
  ret

end

// ; ==== Overridden Command Set ==

// ; openheaderin
inline %0F2h

  xor  ebp, ebp
  push ebp
  xor  eax, eax
  mov  ebp, esp
  sub  esp, __n_2
  push ebp
  push eax
  mov  ecx, __n_1
  mov  ebp, esp
  mov  edi, esp
  rep  stos
  sub  esp, __arg32_1

end 

// ; openheaderin 0, 0
inline %1F2h

  xor  ebp, ebp
  push ebp
  mov  ebp, esp

end 

// ; openheaderin 1, 0
inline %2F2h

  xor  ebp, ebp
  push ebp
  mov  ebp, esp
  push 0

end 

// ; openheaderin 2, 0
inline %3F2h

  xor  ebp, ebp
  push ebp
  xor  eax, eax
  mov  ebp, esp
  push eax
  push eax

end 

// ; openheaderin 3, 0
inline %4F2h

  xor  ebp, ebp
  push ebp
  xor  eax, eax
  mov  ebp, esp
  push eax
  push eax
  push eax

end 

// ; openheaderin 0, n
inline %5F2h

  xor  ebp, ebp
  push ebp
  xor  eax, eax
  mov  ebp, esp
  sub  esp, __n_2
  push ebp
  push eax
  mov  ebp, esp

end 

// ; openheaderin 0, n
inline %6F2h

  xor  ebp, ebp
  push ebp
  xor  eax, eax
  mov  ebp, esp
  mov  ecx, __n_1
  sub  esp, __arg32_1
  mov  edi, esp
  rep  stos

end 
