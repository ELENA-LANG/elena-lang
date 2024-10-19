// ; --- Predefined References  --
define VEH_HANDLER       10003h

define CORE_ET_TABLE     2000Bh

// ; ==== System commands ===

// VEH_HANDLER() 
procedure % VEH_HANDLER

  mov  r10, rdx
  mov  rdx, rax   // ; set exception code
  mov  rax, [data : % CORE_ET_TABLE]
  jmp  rax

end

// ; callext
inline %0FEh

  mov  r9, [rsp+24]
  mov  r8, [rsp+16]
  mov  rdx, r11
  mov  rcx, r10
  call extern __relptr32_1
  mov  rdx, rax

end

// ; callext
inline %1FEh

  call extern __relptr32_1
  mov  rdx, rax

end

// ; callext
inline %2FEh

  mov  rcx, r10
  call extern __relptr32_1
  mov  rdx, rax

end

// ; callext
inline %3FEh

  mov  rdx, r11
  mov  rcx, r10
  call extern __relptr32_1
  mov  rdx, rax

end

// ; callext
inline %4FEh

  mov  r8, [rsp+16]
  mov  rdx, r11
  mov  rcx, r10
  call extern __relptr32_1
  mov  rdx, rax

end

// ; callext
inline %5FEh

  mov  r9, [rsp+24]
  mov  r8, [rsp+16]
  mov  rdx, r11
  mov  rcx, r10
  call extern __relptr32_1
  mov  rdx, rax

end
