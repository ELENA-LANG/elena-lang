// ; --- Predefined References  --
define VEH_HANDLER       	10003h

define CORE_TLS_INDEX       	20004h

// ; ==== System commands ===

// VEH_HANDLER() 
procedure % VEH_HANDLER

  mov  esi, edx
  mov  edx, eax   // ; set exception code

  mov  rcx, gs:[58h]
  mov  rax, [data : %CORE_TLS_INDEX]
  mov  rcx, [rcx+rax*8]
  jmp  [rcx]

end
