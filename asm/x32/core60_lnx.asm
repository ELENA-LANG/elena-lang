// ; --- Predefined References  --
define INVOKER           10001h
define VEH_HANDLER       10003h
define PREPARE	         10006h

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
  mov  eax, ebx

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

procedure %PREPARE

  push eax 
  call extern "$rt.PrepareLA"
  add  esp, 4
  ret

end
