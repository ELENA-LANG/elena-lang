// ; --- Predefined References  --
define INVOKER           10001h
define VEH_HANDLER       10003h

define CORE_ET_TABLE     2000Bh

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

// VEH_HANDLER() 
procedure % VEH_HANDLER

  mov  esi, edx
  mov  edx, eax   // ; set exception code
  mov  eax, [data : % CORE_ET_TABLE]
  jmp  eax

end

// ; ==== Overridden Command Set ==

// ; system prepare
inline %4CFh

  mov  eax, esp
  push eax 
  call extern "$rt.PrepareLA"

end

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
