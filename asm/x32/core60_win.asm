// ; --- Predefined References  --
define VEH_HANDLER       	10003h

define CORE_TOC          	20001h
define SYSTEM_ENV        	20002h
define CORE_SINGLE_CONTENT	2000Bh

// ; ==== System commands ===

// VEH_HANDLER() 
procedure % VEH_HANDLER

  mov  esi, edx
  mov  edx, eax   // ; set exception code
  mov  eax, [data : % CORE_SINGLE_CONTENT]
  jmp  eax

end
