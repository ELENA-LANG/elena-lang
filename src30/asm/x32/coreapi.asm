// verbs
define EXEC_MESSAGE_ID  085000000h

// --- API ---

procedure coreapi'console_entry

  call code : "$package'core'init"
  call code : "$package'core'newframe"
  call code : "$package'core'init_ex_tbl"

  // 'program start
  xor  edi, edi
  call code : "'program"
  push eax
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

// ; in  : acc - contains the object
// ; out : index
procedure coreapi'alloc_index

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
  
  pop  eax
  pop  edi  

  ret

end

// ; in  : index - reference table index
procedure coreapi'free_index

  mov  edx, esi
  mov  esi, [stat : "$elena'@referencetable"]
  mov  [esi + edx * 4], const : "system'nil"
  
  ret

end

// ; in  : index - reference table index
// ; out : acc - referred object
procedure coreapi'resolve_index

  mov  edx, esi
  mov  esi, [stat : "$elena'@referencetable"]
  mov  eax, [esi + edx * 4]
  
  ret

end

// ; in : acc - param
procedure coreapi'start_thread

  push eax

  // ; init thread
  call code : "$package'core'newthread"
  test eax, eax
  jz   short lErr

  call code : "$package'core'init_ex_tbl"

  mov  eax, [esp+0Ch]

  push  eax
  mov   ecx, EXEC_MESSAGE_ID
  mov   esi, [eax - 4]
  call [esi + 4]

  // ; close thread
  call code : "$package'core'closethread"

  mov  eax, 0FFFFFFFFh

lErr:
  push  eax
  call  extern 'dlls'kernel32.ExitThread
  lea   esp, [esp+4]

  ret
end
