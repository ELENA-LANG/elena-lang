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
  mov  edx, EXEC_MESSAGE_ID
  mov  esi, [eax - 4]
  call [esi + 4]

  mov  eax, 0                         // exit code
  push eax
  call extern 'dlls'KERNEL32.ExitProcess     // exit

  ret

end
