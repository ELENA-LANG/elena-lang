
// --- System Core API  --
define NEWFRAME          10014h
define INIT_ET           10015h
define ENDFRAME          10016h
define RESTORE_ET        10017h
define OPENFRAME         10019h
define CLOSEFRAME        1001Ah

rstructure core_vm'dll_name

   db 101 // e
   db 108 // l
   db 101 // e
   db 110 // n
   db 097 // a
   db 118 // v
   db 109 // m
   db 046 // .
   db 100 // d
   db 108 // l
   db 108 // l
   db 0
   
end

rstructure core_vm'SetDebugMode  // SetDebugMode

   db 083 // S
   db 101 // e
   db 116 // t
   db 068 // D
   db 101 // e
   db 098 // b
   db 117 // u
   db 103 // g
   db 077 // M
   db 111 // o
   db 100 // d
   db 101 // e
   db 0

end

rstructure core_vm'Interpret  // Interpret

   db 073 // I
   db 110 // n
   db 116 // t
   db 101 // e
   db 114 // r
   db 112 // p
   db 114 // r
   db 101 // e
   db 116 // t
   db 0

end

rstructure core_vm'ErrorProc  // GetLVMStatus

   db 071 // G
   db 101 // e
   db 116 // t
   db 076 // L
   db 086 // V
   db 077 // M
   db 083 // S
   db 116 // t
   db 097 // a
   db 116 // t
   db 117 // u
   db 115 // s
   db 0

end

rstructure core_vm'DllError  // Cannot load 

   dd 12
   dw 067 // C
   dw 097 // a
   dw 110 // n
   dw 110 // n
   dw 111 // o
   dw 116 // t
   dw 032 //  
   dw 108 // l
   dw 111 // o
   dw 097 // a
   dw 100 // d
   dw 032 //  
   dw 0

end

rstructure core_vm'InvalidDllError  // Incorrect elenavm.dll\n

   dd 21
   dw 073 // I
   dw 110 // n
   dw 099 // c
   dw 111 // o
   dw 114 // r
   dw 101 // e
   dw 099 // c
   dw 116 // t
   dw 032 //  
   dw 101 // e
   dw 108 // l
   dw 101 // e 
   dw 110 // n
   dw 097 // a 
   dw 118 // v 
   dw 109 // m 
   dw 046 // . 
   dw 100 // d 
   dw 108 // l 
   dw 108 // l 
   dw 010 // \n 
   dw 0
   
end

procedure core_vm'console_vm_start

  // load dll  
  mov  eax, rdata : "$native'core_vm'dll_name"
  push eax
  call extern 'dlls'KERNEL32.LoadLibraryA

  test eax, eax
  jz   lbCannotLoadVM

  push eax                    // save hModule

  // ; set debug mode if debug hook is set
  mov  ebx, [data:"'vm_hook"]
  mov  ebx, [ebx]
  test ebx, ebx
  jz   short labHookEnd

  mov  esi, rdata : "$native'core_vm'SetDebugMode" // load SetDebugMode
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
  mov  esi, rdata : "$native'core_vm'Interpret" // load entry procedure name

  push esi
  push eax
  call extern 'dlls'KERNEL32.GetProcAddress

  test eax, eax
  jz   lbCannotFindEntry

  push eax
  mov  eax, esp

  mov  ebx, data:"'vm_tape"
  push ebx

  call [eax]               // call Interpret
  lea   esp, [esp + 8]

  test  eax, eax
  jz    short lbFailed

  call extern 'dlls'KERNEL32.FreeLibrary

  xor  eax, eax
  push eax
  call extern 'dlls'KERNEL32.ExitProcess

  ret

lbCannotLoadVM:

  mov  ebx, rdata : "$native'core_vm'DllError" // Cannot load elenavm.dll
     
  mov  ecx, [ebx]
  lea  ebx, [ebx+4]

  jmp  short lbError

lbFailed:

  mov  eax, [esp]
  mov  esi, rdata : "$native'core_vm'ErrorProc" // load error procedure name
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

  mov  ebx, rdata : "$native'core_vm'InvalidDllError" // Incorrect elenavm.dll

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
