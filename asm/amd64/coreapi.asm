// --- System Core API  --

define CORE_ET_TABLE     2000Bh

// ; --- API ---

procedure coreapi'initProgramHeader

  lea  rdx, [rsp+10h]
  mov  rax, [rsp+8]
  mov  [rax+8], rdx
  mov  [rax+10h], rbp
  ret

end

procedure coreapi'seh_handler

  push rbp
  mov  rbp, rsp
  // ;** now [EBP+8]=pointer to EXCEPTION_RECORD
  // ;** [EBP+0Ch]=pointer to ERR structure
  // ;** [EBP+10h]=pointer to CONTEXT record
  push rbx
  push rdi
  push rsi
  mov  rbx, [rbp+16]
  test rbx, rbx
  jz   short lab5
  test qword ptr[rbx+8],1h
  jnz  lab5
  test qword ptr[rbx+8],2h
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
  mov rsi, [rbp + 20h]
//; MOV EDX,[EBP+0Ch]
//; MOV [ESI+0C4h],EDX   // ; esp

  // ; get critical exception handler
  mov  rax, [data : % CORE_ET_TABLE]
  mov  [rsi+0B8h], rax    // ; eip

//; MOV EAX,[EDX+14h]
//; MOV [ESI+0B4h],EAX   // ; ebp
  xor rax, rax
  jmp short lab6
lab5:
  mov rax, 1
lab6:
  pop rsi
  pop rdi
  pop rbx 
  mov rsp, rbp
  pop rbp
  ret
end

procedure coreapi'default_handler                                                       

  // ; exit code
  push 0
  call extern 'rt_dlls.Exit

end
