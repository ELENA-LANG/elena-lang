// --- System Core Data  --
define CORE_RT_TABLE   20006h

// CORE RT TABLE
define rt_Instance      0000h
define rt_loadSymbol    0004h
define rt_loadName      0008h
define rt_interprete    000Ch
define rt_lasterr       0010h
define rt_loadaddrinfo  0014h
define rt_loadSubject   0018h
define rt_loadSubjName  001Ch

rstructure core_rt'dll_name

   db 101 // e
   db 108 // l
   db 101 // e
   db 110 // n
   db 097 // a
   db 114 // r
   db 116 // t
   db 046 // .
   db 100 // d
   db 108 // l
   db 108 // l
   db 0
   
end

rstructure core_rt'LoadAddressInfoFunc

   db 076 // L
   db 111 // o
   db 097 // a
   db 100 // d
   db 065 // A
   db 100 // d
   db 100 // d
   db 114 // r
   db 101 // e
   db 115 // s
   db 115 // s
   db 073 // I
   db 110 // n
   db 102 // f
   db 111 // o
   db 0

end

rstructure core_rt'LoadClassNameFunc

   db 076 // L
   db 111 // o
   db 097 // a
   db 100 // d
   db 067 // C
   db 108 // l
   db 097 // a
   db 115 // s
   db 115 // s
   db 078 // N
   db 097 // a
   db 109 // m
   db 101 // e
   db 0

end

rstructure core_rt'GetSymbolRefFunc

   db 071 // G
   db 101 // e
   db 116 // t
   db 083 // S
   db 121 // y
   db 109 // m
   db 098 // b
   db 111 // o
   db 108 // l
   db 082 // R
   db 101 // e
   db 102 // f
   db 0

end

rstructure core_rt'InterpreterFunc

   db 073 // I
   db 110 // n
   db 116 // t
   db 101 // e
   db 114 // r
   db 112 // p
   db 114 // r
   db 101 // e
   db 116 // t
   db 101 // e
   db 114 // r
   db 0

end

rstructure core_rt'LastErrFunc

   db 071 // G
   db 101 // e
   db 116 // t
   db 082 // R
   db 084 // T
   db 076 // L
   db 097 // a
   db 115 // s
   db 116 // t
   db 069 // E
   db 114 // r
   db 114 // r
   db 111 // o
   db 114 // r
   db 0

end

rstructure core_rt'InitFunc 

   db 073 // I
   db 110 // n
   db 105 // i
   db 116 // t
   db 0

end

rstructure core_rt'LoadSubjectFunc

   db 076 // L
   db 111 // o
   db 097 // a
   db 100 // d
   db 083 // S
   db 117 // u
   db 098 // b
   db 106 // j
   db 101 // e
   db 099 // c
   db 116 // t
   db 0

end

rstructure core_rt'LoadSubjectNameFunc

   db 076 // L
   db 111 // o
   db 097 // a
   db 100 // d
   db 083 // S
   db 117 // u
   db 098 // b
   db 106 // j
   db 101 // e
   db 099 // c
   db 116 // t
   db 078 // N
   db 097 // a
   db 109 // m
   db 101 // e
   db 0

end

procedure core_rt'init_rt_info

  // load dll  
  mov  eax, rdata : "$native'core_rt'dll_name"
  push eax
  call extern 'dlls'KERNEL32.LoadLibraryA

  test eax, eax
  jz   lbCannotLoadDLL

  push eax                    // save hModule

  // ; init rt_table
  
  mov  esi, rdata:"$native'core_rt'LoadAddressInfoFunc" 
  push esi
  push eax
  call extern 'dlls'KERNEL32.GetProcAddress
  
  test eax, eax
  jz   lbCannotLoadRT

  mov  esi, data : %CORE_RT_TABLE
  mov  [esi + rt_loadaddrinfo], eax

  mov  eax, [esp]
  mov  esi, rdata:"$native'core_rt'LoadClassNameFunc" 
  push esi
  push eax
  call extern 'dlls'KERNEL32.GetProcAddress
  
  test eax, eax
  jz   lbCannotLoadRT

  mov  esi, data : %CORE_RT_TABLE
  mov  [esi + rt_loadName], eax

  mov  eax, [esp]
  mov  esi, rdata:"$native'core_rt'GetSymbolRefFunc" 
  push esi
  push eax
  call extern 'dlls'KERNEL32.GetProcAddress
  
  test eax, eax
  jz   lbCannotLoadRT

  mov  esi, data : %CORE_RT_TABLE
  mov  [esi + rt_loadSymbol], eax

  mov  eax, [esp]
  mov  esi, rdata:"$native'core_rt'InterpreterFunc" 
  push esi
  push eax
  call extern 'dlls'KERNEL32.GetProcAddress
  
  test eax, eax
  jz   lbCannotLoadRT

  mov  esi, data : %CORE_RT_TABLE
  mov  [esi + rt_interprete], eax

  mov  eax, [esp]
  mov  esi, rdata:"$native'core_rt'LastErrFunc" 
  push esi
  push eax
  call extern 'dlls'KERNEL32.GetProcAddress
  
  test eax, eax
  jz   lbCannotLoadRT

  mov  esi, data : %CORE_RT_TABLE
  mov  [esi + rt_lasterr], eax

  mov  eax, [esp]
  mov  esi, rdata:"$native'core_rt'LoadSubjectFunc" 
  push esi
  push eax
  call extern 'dlls'KERNEL32.GetProcAddress
  
  test eax, eax
  jz   lbCannotLoadRT

  mov  esi, data : %CORE_RT_TABLE
  mov  [esi + rt_loadSubject], eax

  mov  eax, [esp]
  mov  esi, rdata:"$native'core_rt'LoadSubjectNameFunc" 
  push esi
  push eax
  call extern 'dlls'KERNEL32.GetProcAddress
  
  test eax, eax
  jz   lbCannotLoadRT

  mov  esi, data : %CORE_RT_TABLE
  mov  [esi + rt_loadSubjName], eax

  mov  eax, [esp]
  mov  esi, rdata:"$native'core_rt'InitFunc" 
  push esi
  push eax
  call extern 'dlls'KERNEL32.GetProcAddress
  
  test eax, eax
  jz   lbCannotLoadRT

  push const : "$elena'@package"
  push 0
  call eax
  add  esp, 8

  mov  esi, data : %CORE_RT_TABLE
  mov  [esi + rt_Instance], eax

lbCannotLoadRT:
  add  esp, 4

lbCannotLoadDLL:
  ret
  
end
