// --- System Core Data  --
define CORE_EXCEPTION_TABLE 01h

// VM_CLIENT_LOADER

procedure core_vm'console_vm_start

  // load dll  
  mov  eax, data : "'vm_path"
  push eax
  call extern 'dlls'KERNEL32.LoadLibraryW

  test eax, eax
  jz   lbCannotLoadVM

  push eax                    // save hModule

  // ; set debug mode if debug hook is set
  mov  ebx, [data:"'vm_hook"]
  mov  ebx, [ebx]
  test ebx, ebx
  jz   short labHookEnd

  mov  esi, data:"'vm_debugprocedure" // load SetDebugMode
  push esi
  push eax
  call extern 'dlls'KERNEL32.GetProcAddress

  test eax, eax
  jz   lbCannotFindEntry

  push eax
  mov  eax, esp
  call [eax]               // call SetDebugMode
  lea  esp, [esp+4]

  // ; set debug section info
  mov  esi, [data:"'vm_hook"]
  mov  [esi+4], eax

  mov  eax, [esp]

labHookEnd:

  // start the program
  mov  esi, data:"'vm_procedure" // load entry procedure name
  push esi
  push eax
  call extern 'dlls'KERNEL32.GetProcAddress

  test eax, eax
  jz   lbCannotFindEntry

  push eax
  mov  eax, esp

  mov  ebx, data:"'vm_tape"
  push ebx

  call [eax]               // call InterpretLVM
  lea   esp, [esp + 8]

  test  eax, eax
  jz    short lbFailed

  call extern 'dlls'KERNEL32.FreeLibrary

  xor  eax, eax
  push eax
  call extern 'dlls'KERNEL32.ExitProcess

  ret

lbCannotLoadVM:

  mov  ebx, data:"'vm_dllnotfound" // Cannot load elenavm.dll
  mov  ecx, [ebx]
  lea  ebx, [ebx+4]

  jmp  short lbError

lbFailed:

  mov  eax, [esp]
  mov  esi, data:"'vm_errproc" // load error procedure name
  push esi
  push eax
  call extern 'dlls'KERNEL32.GetProcAddress
  
  push eax
  mov  eax, esp
  call [eax]
  lea   esp, [esp + 4]

  mov  ebx, eax

  xor  ecx, ecx
lbNextLEn:
  cmp word ptr [eax], 0
  jz  lbError
  add ecx, 1
  lea eax, [eax+2] 
  jmp  short lbNextLEn

lbCannotFindEntry:

  mov  ebx, data:"'vm_dllinvalid" // Incorrect elenavm.dll
  mov  ecx, [ebx]
  lea  ebx, [ebx+4]

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

  call extern 'dlls'KERNEL32.WriteConsoleW
  lea  esp, [esp+4]

  call extern 'dlls'KERNEL32.FreeLibrary

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

  call code : "$package'core'newframe"
  
  // set default exception handler
  mov  [data : %CORE_EXCEPTION_TABLE + 4], esp
  mov  ebx, code : "$package'core_vm'default_handler"
  mov  [data : %CORE_EXCEPTION_TABLE], ebx
  
  // invoke symbol
  mov ebp, esp
  mov edx, [ebp+20h]  
  mov esi, [edx]
  
  call eax

  call code : "$package'core'endframe"

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
  
  call code : "$package'core'openframe"

  // set default exception handler
  mov  [data : %CORE_EXCEPTION_TABLE + 4], esp
  mov  ebx, code : "$package'core_vm'default_handler"
  mov  [data : %CORE_EXCEPTION_TABLE], ebx
/*  
  mov edx, [ebp+20h]

  mov esi, [edx]
*/  
  // invoke symbol
  call eax

  call code : "$package'core'closeframe"

  pop esi
  pop edi
  pop ecx
  pop ebx
  ret

end

procedure core_vm'default_handler
                                                       
  mov  esp, [data : %CORE_EXCEPTION_TABLE + 4]

  call code : "$package'core'endframe"

  xor eax, eax
  pop ebp
  pop esi
  pop edi
  pop ecx
  pop ebx
  ret

end
