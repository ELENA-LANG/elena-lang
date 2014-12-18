// --- System Core API  --
define GC_ALLOC	         10001h
define HOOK              10010h
define INIT_RND          10012h
define INIT              10013h
define NEWFRAME          10014h
define INIT_ET           10015h

// verbs
define EXEC_MESSAGE_ID  085000000h

// ; --- API ---

// ; console_entry()
procedure coreapi'console_entry

  call code : % INIT
  call code : % NEWFRAME
  mov  ebx, code : "$native'coreapi'default_handler"
  call code : % INIT_ET

  call code : "$native'core_rt'init_rt_info"  

  // 'program start
  xor  edi, edi
  call code : "'program"

  mov  ecx, EXEC_MESSAGE_ID
  mov  esi, [eax - 4]
  call [esi + 4]

  // ; exit code
  mov  eax, 0                         
  push eax
  // ; exit
  call extern 'dlls'KERNEL32.ExitProcess     

  ret

end

procedure coreapi'default_handler
                                                       
  mov  eax, 1                           
  push eax
  // ; exit  
  call extern 'dlls'KERNEL32.ExitProcess     

end

// ; console_vm_entry()
procedure coreapi'vm_console_entry

  push ebx
  push ecx
  push edi
  push esi
  push ebp
  
  call code : % INIT

  pop  ebp
  pop  esi
  pop  edi
  pop  ecx
  pop  ebx
                                                           
  ret

end
