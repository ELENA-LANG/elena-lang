// verbs
define EXEC_MESSAGE_ID  085000000h

// ; --- API ---

// ; console_entry()
procedure coreapi'console_entry

  call code : "$package'core'init"
  call code : "$package'core'newframe"
  call code : "$package'core'init_ex_tbl"
  call code : "$package'core_rt'init_rt_info"

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

// ; console_vm_entry()
procedure coreapi'vm_console_entry

  push ebx
  push ecx
  push edi
  push esi
  push ebp
  
  call code : "$package'core'init"

  pop  ebp
  pop  esi
  pop  edi
  pop  ecx
  pop  ebx
                                                           
  ret

end

// ; alloc_index(object,out'index) 
procedure coreapi'alloc_index

  mov  eax, [esp + 4]
  push edi
  push eax
  mov  edi, [stat : "$elena'@referencetable"]
  
  test edi, edi
  jnz  short labStart

  mov  ebx, 080h
  call code : "$package'core'allocate"
  mov  [stat : "$elena'@referencetable"], eax
  mov  edi, eax

labStart:
  // ; lock the reference table
  call code : "$package'core'lock"
  mov  eax, [esp]
  
  // ; try to append a new reference to the table
  call code : "$package'core'append"
  test eax, eax
  // ; if enough place jump to the indexing part
  jnz  short labIndex

  // ; try to reuse existing slots
  call code : "$package'core'getcount"
  mov  esi, edi
  xor  edx, edx
labNext:
  cmp  [esi], const : "system'nil"
  jz   short labReuse
  add  esi, 4
  add  edx, 1
  sub  ecx, 1 
  ja   short labNext                                                                                               

  // ; if no place reallocate the reference table
  call code : "$package'core'getcount"
  mov  ebx, ecx
  add  ebx, 10h
  call code : "$package'core'reallocate"
  jmp  labStart

labReuse:
  mov  eax, [esp]
  mov  [edi + edx * 4], eax
  mov  esi, edx
  jmp  short labEnd
  
labIndex:
  call code : "$package'core'getcount"
  sub  ecx, 1
  mov  esi, ecx
labEnd:
  call code : "$package'core'unlock"

  mov  eax, [esp + 16]
  mov  [eax], esi
  
  pop  eax
  pop  edi  
  

  ret 8

end

// ; free_index (index)
procedure coreapi'free_index

  mov  ebx, [esp + 4]
  mov  edx, [ebx]
  mov  esi, [stat : "$elena'@referencetable"]
  mov  [esi + edx * 4], const : "system'nil"
  
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
  call code : "$package'core'newthread"
  mov  ecx, 1
  test eax, eax
  jz   short lErr

  call code : "$package'core'init_ex_tbl"  

  push  eax
  mov   ecx, EXEC_MESSAGE_ID
  mov   esi, [eax - 4]
  call [esi + 4]

  // ; close thread
  call code : "$package'core'closethread"

  xor  ecx, ecx

lErr:
  push  ecx
  call  extern 'dlls'kernel32.ExitThread
  lea   esp, [esp+4]
  
  ret 4
end

// ; load_symbol (name,out reference)
procedure coreapi'load_symbol

  mov  edx, [esp+4]
  call code : "$package'core'loadsymbol"  
  test eax, eax
  jz   short labEnd
  mov  ecx, eax
  mov  eax, [esp + 8]
  mov  [eax], ecx
  
labEnd:
  ret 8
  
end

// ; load_classname(object,out buffer, out length)
procedure coreapi'load_classname

  mov  eax, [esp + 12]
  mov  esi, [eax]
  mov  eax, [esp + 8]
  mov  edi, [esp + 4]
  
  call code : "$package'core'loadclassname"

  mov  edi, [esp + 12]
  mov  [edi], eax
  
  ret 12

end

// ; load_addressinfo(array,index,out buffer, out length)
procedure coreapi'load_callstackinfo

  mov  eax, [esp + 4]
  mov  ebx, [esp + 8]
  mov  edx, [ebx]
  mov  ecx, [eax + edx * 4]

  mov  eax, [esp + 16]
  mov  esi, [eax]
  mov  eax, [esp + 12]

  call code : "$package'core'loadaddressinfo"

  mov  edi, [esp + 16]
  mov  [edi], eax
  
  ret 16

end
