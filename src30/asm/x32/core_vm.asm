
// --- System Core API  --
define NEWFRAME          10014h
define INIT_ET           10015h
define ENDFRAME          10016h
define RESTORE_ET        10017h
define OPENFRAME         10019h
define CLOSEFRAME        1001Ah

procedure core_vm'console_vm_start

  // ; set debug mode if debug hook is set
  mov  ebx, [data:"'vm_hook"]
  mov  ebx, [ebx]
  test ebx, ebx
  jz   short labHookEnd
  
  call extern 'dlls'elenavm.SetDebugMode

  // ; set debug section info
  mov  esi, [data:"'vm_hook"]
  mov  [esi+4], eax

  mov  eax, [esp]

labHookEnd:

  mov  ebx, data:"'vm_tape"
  push ebx

  call extern 'dlls'elenavm.InterpretTape

  lea   esp, [esp + 4]

  test  eax, eax
  jz    short lbFailed

  xor  eax, eax
  push eax
  call extern 'dlls'KERNEL32.ExitProcess

  ret
  
lbFailed:

  call extern 'dlls'elenavm.GetVMLastError

  mov  ebx, eax
  xor  ecx, ecx
lbNextLEn:
  cmp byte ptr [eax], 0
  jz  lbError
  add ecx, 1
  lea eax, [eax+1] 
  jmp  short lbNextLEn

lbError:

  push 0FFFFFFF5h
  call extern 'dlls'KERNEL32.GetStdHandle

  push 00
  mov  edx, esp

  push 00                    // place lpReserved
  push edx
  push ecx                   // place length
  push ebx
  push eax

  call extern 'dlls'KERNEL32.WriteConsoleA
  lea  esp, [esp+4]

  call extern 'dlls'KERNEL32.ExitProcess
  ret
    
end

procedure core_vm'start_n_eval

  mov eax, [esp+4]

  push ebx
  push ecx
  push edi
  push esi
  push ebp

  call code : % NEWFRAME
  
  // set default exception handler
  mov  ebx, code : "$native'core_vm'default_handler"
  call code : % INIT_ET
  
  // invoke symbol
  mov ebp, esp
  mov edx, [ebp+20h]  
  mov esi, [edx]
  
  call eax

  call code : % ENDFRAME
  
  pop ebp
  pop esi
  pop edi
  pop ecx
  pop ebx
  ret

end

procedure core_vm'eval

  mov eax, [esp+4]

  push ebx
  push ecx
  push edi
  push esi
  
  call code : % OPENFRAME

  // invoke symbol
  call eax

  call code : % CLOSEFRAME

  pop esi
  pop edi
  pop ecx
  pop ebx
  ret

end

procedure core_vm'default_handler
                                                       
  call code : % RESTORE_ET

  call code : % ENDFRAME

  xor eax, eax
  pop ebp
  pop esi
  pop edi
  pop ecx
  pop ebx
  ret

end
