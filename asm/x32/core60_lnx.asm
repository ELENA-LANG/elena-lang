// ; --- Predefined References  --
define VEH_HANDLER       	10003h
define PREPARE	         	10006h

define CORE_SINGLE_CONTENT     2000Bh

// ; ==== System commands ===

// VEH_HANDLER() 
procedure % VEH_HANDLER

  mov  esi, edx
  mov  edx, eax   // ; set exception code
  mov  eax, [data : % CORE_SINGLE_CONTENT]
  jmp  eax

end

procedure %PREPARE

  push eax 
  call extern "$rt.PrepareLA"
  add  esp, 4
  ret

end
