// ; --- Predefined References  --
define VEH_HANDLER       	10003h

// ; ==== System commands ===

// VEH_HANDLER() 
procedure % VEH_HANDLER

  mov  esi, edx
  mov  edx, eax   // ; set exception code

  mov  ecx, fs:[2Ch]
  mov  ecx, [ecx]
  jmp  [ecx]

end
