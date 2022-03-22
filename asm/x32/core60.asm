// ; --- Predefined References  --
define INVOKER         10001h
define GC_ALLOC	       10002h

define CORE_TOC        20001h
define SYSTEM_ENV      20002h
define CORE_GC_TABLE   20003h

// ; --- Object header fields ---
define elSizeOffset          0004h
define elVMTOffset           0008h 
define elObjectOffset        0008h

// ; --- VMT header fields ---
define elVMTSizeOffset       0004h
define elVMTFlagOffset       000Ch
define elPackageOffset       0010h

// ; --- GC TABLE OFFSETS ---
define gc_header             0000h
define gc_start              0004h
define gc_yg_start           0008h
define gc_yg_current         000Ch
define gc_yg_end             0010h
define gc_shadow             0014h
define gc_shadow_end         0018h
define gc_mg_start           001Ch
define gc_mg_current         0020h
define gc_end                0024h
define gc_mg_wbar            0028h

// ; --- System Core Preloaded Routines --

structure % CORE_TOC

  dd 0         // ; reserved

end
 
structure %CORE_GC_TABLE

  dd 0 // ; gc_header             : +00h
  dd 0 // ; gc_start              : +04h
  dd 0 // ; gc_yg_start           : +08h
  dd 0 // ; gc_yg_current         : +0Ch
  dd 0 // ; gc_yg_end             : +10h
  dd 0 // ; gc_shadow             : +14h
  dd 0 // ; gc_shadow_end         : +18h
  dd 0 // ; gc_mg_start           : +1Ch
  dd 0 // ; gc_mg_current         : +20h
  dd 0 // ; gc_end                : +24h
  dd 0 // ; gc_mg_wbar            : +28h

end

// ; NOTE : the table is tailed with GCMGSize,GCYGSize and MaxThread fields
structure %SYSTEM_ENV

  dd data : %CORE_GC_TABLE
  dd code : %INVOKER
  // ; dd GCMGSize
  // ; dd GCYGSize

end

// ; --- GC_ALLOC ---
// ; in: ecx - size ; out: ebx - created object
inline % GC_ALLOC

  mov  edi, [data : %CORE_GC_TABLE + gc_yg_current]
  add  ecx, edi
  cmp  ecx, [data : %CORE_GC_TABLE + gc_yg_end]
  jae  short labYGCollect
  mov  [data : %CORE_GC_TABLE + gc_yg_current], ecx
  lea  ebx, [edi + elObjectOffset]
  ret

labYGCollect:
  xor  ebx, ebx  // !! temporal stub
  ret

end

// ; ==== Command Set ==

// ; redirect
inline % 03h // (ebx - object, eax - message, esi - arg0, edx - arg1)

  mov   [esp+4], esi
  mov   edi, [ebx - elVMTOffset]
  mov   [esp+8], edx
  xor   ecx, ecx
  mov   esi, [edi - elVMTSizeOffset]

labSplit:
  test  esi, esi
  jz    short labEnd

labStart:
  shr   esi, 1
  setnc cl
  cmp   eax, [edi+esi*8]
  je    short labFound
  lea   edx, [edi+esi*8]
  jb    short labSplit
  lea   edi, [edx+8]
  sub   esi, ecx
  jmp   short labSplit
labFound:
  mov   edi, [edi+esi*8+4]
  mov   esi, [esp+4]
  mov   edx, [esp+8]
  jmp   edi

labEnd:
                                                                
end

// ; quit
inline %4

  ret

end

// ; movenv
inline %5

  mov  eax, rdata32 : %SYSTEM_ENV

end

// ; load
inline %6

  mov  eax, dword ptr [ebx]

end

// ; setr
inline %80h

  mov  ebx, __ptr32_1

end 

// ; setr 0
inline %180h

  xor  ebx, ebx

end 
// ; setddisp
inline %81h

  lea  ebx, [ebp + __arg32_1]

end 

// ; movm
inline %88h

  mov  eax, __arg32_1

end

// ; closen
inline %91h

  add  ebp, __arg32_1
  mov  esp, ebp
  pop  ebp
  
end

// ; closen 0
inline %191h

  mov  esp, ebp
  pop  ebp
  
end

// ; alloci
inline %92h

  sub  esp, __arg32_1
  xor  eax, eax
  mov  ecx, __n_1
  mov  edi, esp
  rep  stos

end

// ; alloci 0
inline %192h

end

// ; alloci 1
inline %292h

  push 0

end

// ; alloci 2
inline %392h

  push 0
  push 0

end

// ; alloci 3
inline %492h

  push 0
  push 0
  push 0

end

// ; alloci 4
inline %592h

  push 0
  push 0
  push 0
  push 0

end

// ; freei
inline %93h

  add  esp, __arg32_1

end

// ; saveddisp
inline %0A0h

  mov  [ebp + __arg32_1], eax

end

// ; storefp
inline %0A1h

  mov  [ebp + __arg32_1], ebx

end

// ; savesi
inline %0A2h

  mov [esp + __arg32_1], eax

end 

// ; savesi 0
inline %1A2h

  mov esi, eax

end 

// ; savesi 1
inline %2A2h

  mov edx, eax

end 

// ; storesi
inline %0A3h

  mov [esp + __arg32_1], ebx

end 

// ; storesi 0
inline %1A3h

  mov esi, ebx

end 

// ; storesi 1
inline %2A3h

  mov edx, ebx

end 

// ; peekfi
inline %0A8h

  mov  ebx, [ebp + __arg32_1]

end 

// ; callr
inline %0B0h

  call __relptr32_1

end

// ; callvi
inline %0B1h

  mov  edx, [ebx - elVMTOffset]
  call [edx + __arg32_1]

end

// ; openin
inline %0F0h

  push ebp
  xor  eax, eax
  mov  ebp, esp
  sub  esp, __n_2
  push ebp
  push eax
  mov  ebp, esp
  mov  ecx, __n_1
  sub  esp, __arg32_1
  mov  edi, esp
  rep  stos

end 

// ; openin 0, 0
inline %1F0h

  push ebp
  mov  ebp, esp

end 

// ; openin 1, 0
inline %2F0h

  push ebp
  mov  ebp, esp
  push 0

end 

// ; openin 2, 0
inline %3F0h

  push ebp
  xor  eax, eax
  mov  ebp, esp
  push eax
  push eax

end 

// ; openin 3, 0
inline %4F0h

  push ebp
  xor  eax, eax
  mov  ebp, esp
  push eax
  push eax
  push eax

end 

// ; openin 0, n
inline %5F0h

  push ebp
  xor  eax, eax
  mov  ebp, esp
  sub  esp, __n_2
  push ebp
  push eax
  mov  ebp, esp

end 

// ; openin i, 0
inline %6F0h

  push ebp
  xor  eax, eax
  mov  ebp, esp
  mov  ecx, __n_1
  sub  esp, __arg32_1
  mov  edi, esp
  rep  stos

end 

// ; xstoresir
inline %0F1h

  mov  ecx, __ptr32_2
  mov  [esp+__arg32_1], ecx

end

// ; xstoresir :0, ...
inline %1F1h

  mov  esi, __ptr32_2

end

// ; xstoresir :1, ...
inline %2F1h

  mov  edx, __ptr32_2

end

// ; openheaderin
inline %0F2h

  push ebp
  xor  eax, eax
  mov  ebp, esp
  sub  esp, __n_2
  push ebp
  push eax
  mov  ebp, esp
  mov  ecx, __n_1
  sub  esp, __arg32_1
  mov  edi, esp
  rep  stos

end 

// ; openheaderin 0, 0
inline %1F2h

  push ebp
  mov  ebp, esp

end 

// ; openheaderin 1, 0
inline %2F2h

  push ebp
  mov  ebp, esp
  push 0

end 

// ; openheaderin 2, 0
inline %3F2h

  push ebp
  xor  eax, eax
  mov  ebp, esp
  push eax
  push eax

end 

// ; openheaderin 3, 0
inline %4F2h

  push ebp
  xor  eax, eax
  mov  ebp, esp
  push eax
  push eax
  push eax

end 

// ; openheaderin 0, n
inline %5F2h

  push ebp
  xor  eax, eax
  mov  ebp, esp
  sub  esp, __n_2
  push ebp
  push eax
  mov  ebp, esp

end 

// ; openheaderin i, 0
inline %6F2h

  push ebp
  xor  eax, eax
  mov  ebp, esp
  mov  ecx, __n_1
  sub  esp, __arg32_1
  mov  edi, esp
  rep  stos

end 

// ; movsifi
inline %0F3h

  mov  ecx, [ebp+__arg32_2]
  mov  [esp+__arg32_1], ecx

end

// ; movsifi sp:0, fp:i2
inline %1F3h

  mov  esi, [ebp+__arg32_2]

end

// ; movsifi sp:1, fp:i2
inline %2F3h

  mov  edx, [ebp+__arg32_2]

end

// ; newir i, r
inline %0F4h

  mov  ecx, __arg32_1
  call %GC_ALLOC

  mov  ecx, __n_1
  mov  edi, __ptr32_2
  mov  [ebx - elSizeOffset], ecx
  mov  [ebx - elVMTOffset], edi

end

// ; callext
inline %0FEh

  mov  [esp], esi
  mov  [esp+4], edx
  call extern __ptr32_1

end

// ; callext
inline %1FEh

  call extern __ptr32_1

end

// ; callext
inline %2FEh

  mov  [esp], esi
  call extern __ptr32_1

end

