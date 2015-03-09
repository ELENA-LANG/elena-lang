// --- System Core API  --
define GC_ALLOC	         10001h
define HOOK              10010h
define INIT_RND          10012h
define INIT              10013h
define NEWFRAME          10014h
define INIT_ET           10015h
define LOAD_CLASSNAME    10018h
define NEWTHREAD         1001Bh
define CLOSETHREAD       1001Ch
define EXIT              1001Dh
define CALC_SIZE         1001Eh
define SET_COUNT         1001Fh
define GET_COUNT         10020h
define LOCK              10021h
define UNLOCK            10022h
define LOAD_ADDRESSINFO  10023h
define LOAD_CALLSTACK    10024h

// verbs
define EXEC_MESSAGE_ID  085000000h

// ; --- API ---

// ; console_entry()
procedure coreapi'console_entry

  // !! temporal
  ret 

/*
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
  call code : % EXIT

  ret
*/  
end

procedure coreapi'default_handler                                                       

  // ; exit code
  call code : % EXIT

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

// ; load_classname(object,out buffer, out length)
procedure coreapi'load_classname

  mov  eax, [esp + 12]
  mov  esi, [eax]
  mov  eax, [esp + 8]
  mov  edi, [esp + 4]
  
  call code : % LOAD_CLASSNAME

  mov  edi, [esp + 12]
  mov  [edi], eax
  
  ret 12

end

// ; new ebx - size, 
procedure coreapi'reallocate

  push eax
  call code : %CALC_SIZE

  call code : %GET_COUNT  
  mov  ecx, esi
  
  call code : %GC_ALLOC

  mov  esi, ecx
  call code : %SET_COUNT

  mov  edi, eax
  pop  esi

labNext:
  mov  edx, [edi]
  mov  [esi], edx
  add  edi, 4
  add  esi, 4
  sub  ecx, 1
  jnz  short labNext

  ret

end

procedure coreapi'alloc_index

  mov  eax, [stat : "$elena'@referencetable"]
  
  test eax, eax
  jnz  short labStart

  mov  ebx, 020h
  call code : %CALC_SIZE
  call code : %GC_ALLOC  
  xor  esi, esi
  call code : %SET_COUNT 

  mov  [stat : "$elena'@referencetable"], eax

labStart:
  // ; lock the reference table
  call code : %LOCK
  
  // ; try to increase eax
  call code : %GET_COUNT  
  add  esi, 1  
  call code : %SET_COUNT   // ; if the object size cannot be expanded - returns 0    
  test esi, esi
  // ; if enough place jump to the indexing part
  jnz  short labIndex

  // ; try to reuse existing slots
  call code : %GET_COUNT
  mov  ecx, esi
  xor  edx, edx
  mov  esi, eax
labNext:
  cmp  [esi], 0
  jz   short labReuse
  add  esi, 4
  add  edx, 1
  sub  ecx, 1 
  ja   short labNext                                                                                               

  // ; if no place reallocate the reference table
  call code : %GET_COUNT
  mov  ebx, esi
  add  ebx, 10h

  call code : "$native'coreapi'reallocate"

  mov  [stat : "$elena'@referencetable"], eax
  jmp  labStart

labReuse:
  mov  [eax + esi * 4], const : "system'nil"
  jmp  short labEnd
  
labIndex:
  sub  esi, 1
  mov  [eax + esi * 4], const : "system'nil"
labEnd:
  call code : %UNLOCK

  ret

end

// ; free_index
procedure coreapi'free_index

  mov  esi, [stat : "$elena'@referencetable"]
  mov  [esi + esi * 4], 0
  
  ret 4

end

// ; resolve_index (index)
procedure coreapi'resolve_index

  mov  ebx, [esp + 4]
  mov  edx, [ebx]
  mov  esi, [stat : "$elena'@referencetable"]
  mov  eax, [esi + edx * 4]
  
  ret 4

end

// ; start_thread(param)
procedure coreapi'start_thread

  mov  eax, [esp + 4]
           
  // ; init thread
  call code : % NEWTHREAD
  mov  ecx, 1
  test eax, eax
  jz   short lErr

  call code : % INIT_ET

  push  eax
  mov   ecx, EXEC_MESSAGE_ID
  mov   esi, [eax - 4]
  call [esi + 4]

  // ; close thread
  call code : % CLOSETHREAD

  xor  ecx, ecx

lErr:
  
  ret 4
end

// ; load_addressinfo(array,index,out buffer, out length)
procedure coreapi'load_addressinfo

  mov  eax, [esp + 4]
  mov  ebx, [esp + 8]
  mov  edx, [ebx]
  mov  ecx, [eax + edx * 4]

  mov  eax, [esp + 16]
  mov  esi, [eax]
  mov  eax, [esp + 12]

  call code : % LOAD_ADDRESSINFO

  mov  edi, [esp + 16]
  mov  [edi], eax
  
  ret 16
                                                     
end

// ; load_addressinfo(array,max length,out length)
procedure coreapi'load_callstack

  mov  eax, [esp + 4]
  mov  ecx, [esp + 8]

  call code : % LOAD_CALLSTACK

  mov  edi, [esp + 12]
  mov  [edi], esi

  ret 12

end
