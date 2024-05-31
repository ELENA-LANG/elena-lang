// ; --- Predefined References  --
define VEH_HANDLER       	10003h

define CORE_TLS_INDEX       	20004h

// ; ==== System commands ===

// VEH_HANDLER() 
procedure % VEH_HANDLER

  mov  esi, edx
  mov  edx, eax   // ; set exception code

  mov  ecx, fs:[2Ch]
  mov  eax, [data : %CORE_TLS_INDEX]
  mov  ecx, [ecx+eax*4]
  jmp  [ecx]

end
