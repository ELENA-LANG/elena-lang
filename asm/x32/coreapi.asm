// --- System Core API  --

define CORE_ET_TABLE     2000Bh

// ; === internal ===

procedure coreapi'default_handler                                                       

  // ; exit code
  push 0
  call extern 'rt_dlls.Exit

end

procedure coreapi'get_seh_handler

  mov  ecx, data : % CORE_ET_TABLE
  ret

end

procedure coreapi'seh_handler

  push ebp
  mov  ebp, esp
  // ;** now [EBP+8]=pointer to EXCEPTION_RECORD
  // ;** [EBP+0Ch]=pointer to ERR structure
  // ;** [EBP+10h]=pointer to CONTEXT record
  push ebx
  push edi
  push esi
  mov  ebx, [ebp+8]
  test ebx, ebx
  jz   short lab5
  test dword ptr[ebx+4],1h
  jnz  lab5
  test dword ptr[ebx+4],2h
  jz   lab2
//; ...
//; ...
//; ...
  jmp lab5
lab2:
//; PUSH 0
//; PUSH [EBP+8h]
//; PUSH ADDR UN23
//; PUSH [EBP+0Ch]
//; CALL RtlUnwind
//; UN23:
  mov esi, [ebp + 10h]
//; MOV EDX,[EBP+0Ch]
//; MOV [ESI+0C4h],EDX   // ; esp

  // ; get critical exception handler
  mov  eax, [data : % CORE_ET_TABLE]
  mov  [esi+0B8h], eax    // ; eip

//; MOV EAX,[EDX+14h]
//; MOV [ESI+0B4h],EAX   // ; ebp
  xor eax, eax
  jmp short lab6
lab5:
  mov eax, 1
lab6:
  pop esi
  pop edi
  pop ebx 
  mov esp, ebp
  pop ebp
  ret

end

