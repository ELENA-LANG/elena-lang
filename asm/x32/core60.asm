// ; --- Predefined References  --
define INVOKER              10001h
define GC_ALLOC	            10002h

define CORE_TOC             20001h
define SYSTEM_ENV           20002h
define CORE_GC_TABLE   	    20003h
define VOID           	    2000Dh
define VOIDPTR              2000Eh

define ACTION_ORDER              9

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

define struct_mask_inv     7FFFFFh

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

structure %VOID

  dd 0
  dd 0  // ; a reference to the super class class
  dd 0
  dd 0  
  dd 0

end

structure %VOIDPTR

  dd rdata : %VOID + elPackageOffset
  dd 0
  dd 0

end

// ; --- GC_ALLOC ---
// ; in: ecx - size ; out: ebx - created object
inline % GC_ALLOC

  mov  eax, [data : %CORE_GC_TABLE + gc_yg_current]
  add  ecx, eax
  cmp  ecx, [data : %CORE_GC_TABLE + gc_yg_end]
  jae  short labYGCollect
  mov  [data : %CORE_GC_TABLE + gc_yg_current], ecx
  lea  ebx, [eax + elObjectOffset]
  ret

labYGCollect:
  xor  ebx, ebx  // !! temporal stub
  ret

end

// ; ==== Command Set ==

// ; redirect
inline % 03h // (ebx - object, edx - message, esi - arg0, edi - arg1)

  mov   [esp+4], esi                      // ; saving arg0
  xor   ecx, ecx
  mov   edi, [ebx - elVMTOffset]
  mov   esi, [edi - elVMTSizeOffset]

labSplit:
  test  esi, esi
  jz    short labEnd

labStart:
  shr   esi, 1
  setnc cl
  cmp   edx, [edi+esi*8]
  je    short labFound
  lea   eax, [edi+esi*8]
  jb    short labSplit
  lea   edi, [eax+8]
  sub   esi, ecx
  jmp   short labSplit
labFound:
  mov   eax, [edi+esi*8+4]
  mov   esi, [esp+4]
  jmp   eax

labEnd:
                                                                
end

// ; quit
inline %4

  ret

end

// ; movenv
inline %5

  mov  edx, rdata32 : %SYSTEM_ENV

end

// ; load
inline %6

  mov  edx, dword ptr [ebx]

end

// ; len
inline %7

  mov  edx, struct_mask_inv
  mov  ecx, [ebx-elSizeOffset]
  and  edx, ecx
  shr  edx, 2

end

// ; setr
inline %80h

  mov  ebx, __ptr32_1

end 

// ; setr 0
inline %180h

  xor  ebx, ebx

end 
// ; setdp
inline %81h

  lea  ebx, [ebp + __arg32_1]

end 

// ; nlen n
inline %82h

  mov  eax, struct_mask_inv
  and  eax, [ebx-elSizeOffset]
  mov  ecx, __n_1
  cdq
  idiv ecx
  mov  edx, eax

end

// ; nlen 1
inline %182h

  mov  edx, struct_mask_inv
  mov  ecx, [ebx-elSizeOffset]
  and  edx, ecx

end

// ; nlen 2
inline %282h

  mov  edx, struct_mask_inv
  mov  ecx, [ebx-elSizeOffset]
  and  edx, ecx
  shr  edx, 1

end

// ; nlen 4
inline %382h

  mov  edx, struct_mask_inv
  mov  ecx, [ebx-elSizeOffset]
  and  edx, ecx
  shr  edx, 2

end

// ; nlen 8
inline %482h

  mov  edx, struct_mask_inv
  mov  ecx, [ebx-elSizeOffset]
  and  edx, ecx
  shr  edx, 3

end

// ; movm
inline %88h

  mov  edx, __arg32_1

end

// ; copy
inline %90h

  mov  ecx, __n_1 
  mov  edi, ebx
  rep  movsb
  sub  esi, __n_1          // ; to set back ESI register

end

// ; closen
inline %91h

  add  ebp, __n_1
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

  mov  [ebp + __arg32_1], edx

end

// ; storefp
inline %0A1h

  mov  [ebp + __arg32_1], ebx

end

// ; savesi
inline %0A2h

  mov [esp + __arg32_1], edx

end 

// ; savesi 0
inline %1A2h

  mov esi, edx

end 

// ; storesi
inline %0A3h

  mov [esp + __arg32_1], ebx

end 

// ; storesi 0
inline %1A3h

  mov esi, ebx

end 

// ; xflushsi i
inline %0A4h

end 

// ; xflushsi 0
inline %1A4h

  mov [esp+4], esi

end 

// ; peekfi
inline %0A8h

  mov  ebx, [ebp + __arg32_1]

end 

// ; peeksi
inline %0A9h

  mov ebx, [esp + __arg32_1]

end 

// ; peeksi 0
inline %1A9h

  mov ebx, esi

end 

// ; callr
inline %0B0h

  call __relptr32_1

end

// ; callvi
inline %0B1h

  mov  eax, [ebx - elVMTOffset]
  call [eax + __arg32_1]

end

// ; cmpr r
inline %0C0h

  cmp  ebx, __ptr32_1

end 

// ; cmpfi
inline %0C8h

  cmp  ebx, [ebp + __arg32_1]

end 

// ; cmpsi
inline %0C9h

  cmp  ebx, [esp + __arg32_1]

end 

// ; cmpsi 0
inline %1C9h

  cmp  ebx, esi

end 

// ; copydpn
inline %0E0h

  mov  eax, esi
  lea  edi, [ebp + __arg32_1]
  mov  ecx, __n_2
  rep  movsb
  mov  esi, eax

end

// ; iaddndp
inline %0E1h

  lea  edi, [ebp + __arg32_1]
  mov  eax, [esi]
  add  [edi], eax

end

// ; iaddndp
inline %1E1h

  lea  edi, [ebp + __arg32_1]
  mov  eax, [esi]
  add  byte ptr [edi], al

end

// ; iaddndp
inline %2E1h

  lea  edi, [ebp + __arg32_1]
  mov  eax, [esi]
  add  word ptr [edi], ax

end

// ; iaddndp
inline %4E1h

  lea  edi, [ebp + __arg32_1]
  mov  eax, [esi + 4]
  mov  ecx, [esi]
  add  word ptr [edi], ax
  add  [edi], ecx
  adc  [edi+4], eax

end

// ; isubndp 4
inline %0E2h

  lea  edi, [ebp + __arg32_1]
  mov  eax, [esi]
  sub  [edi], eax

end

// ; isubndp 1
inline %1E2h

  lea  edi, [ebp + __arg32_1]
  mov  eax, [esi]
  sub  byte ptr [edi], al

end

// ; isubndp 2
inline %2E2h

  lea  edi, [ebp + __arg32_1]
  mov  eax, [esi]
  sub  word ptr [edi], ax

end

// ; isubndp 8
inline %4E2h

  lea  edi, [ebp + __arg32_1]
  mov  eax, [esi + 4]
  mov  ecx, [esi]
  sub  [edi], ecx
  sbb  [edi+4], eax

end

// ; imulndp
inline %0E3h

  mov  eax, [ebp+__arg32_1]
  imul dword ptr [esi]
  mov  [ebp+__arg32_1], eax

end

// ; imulndp
inline %1E3h

  mov  ecx, [esi]
  mov  eax, [ebp+__arg32_1]
  imul cl
  mov  byte ptr [ebp+__arg32_1], al

end

// ; imulndp
inline %2E3h

  mov  ecx, [esi]
  mov  eax, [ebp+__arg32_1]
  imul cx
  mov  word ptr [ebp+__arg32_1], ax

end

// ; imulndp
inline %4E3h

  lea  edi, [ebp+__arg32_1]
  mov  edx, edi        // dest

  push ebx
  
  mov  ecx, [edx+4]   // DHI
  mov  eax, [esi+4]   // SHI
  or   eax, ecx
  mov  ecx, [edx]     // DLO
  jnz  short lLong
  mov  eax, [esi]
  mul  ecx
  jmp  short lEnd

lLong:
  mov  eax, [esi+4]
  mov  edi, edx
  mul  ecx               // SHI * DLO
  mov  ebx, eax
  mov  eax, dword ptr [esi]
  mul  dword ptr [edi+4]  // SLO * DHI
  add  ebx, eax     
  mov  eax, dword ptr [esi] // SLO * DLO
  mul  ecx
  add  edx, ebx 

lEnd:
  mov  [edi], eax
  pop  ebx
  mov  [edi+4], edx

end

// ; idivndp
inline %0E4h

  mov  eax, [ebp+__arg32_1]
  cdq
  idiv dword ptr [esi]
  mov  [ebp+__arg32_1], eax

end

// ; idivndp
inline %1E4h

  mov  ecx, [esi]
  mov  eax, [ebp+__arg32_1]
  cdq
  idiv cl
  mov  byte ptr [ebp+__arg32_1], al

end

// ; idivndp
inline %2E4h

  mov  ecx, [esi]
  mov  eax, [ebp+__arg32_1]
  cdq
  idiv cx
  mov  word ptr [ebp+__arg32_1], ax

end

// ; idivndp
inline %4E4h

  push ebx
  mov  [esp+4], esi
  mov  ebx, esi
  lea  esi, [ebp+__arg32_1] // ; esi - DVND, ebx - DVSR

  push [esi+4]    // ; DVND hi dword
  push [esi]      // ; DVND lo dword
  push [ebx+4]    // ; DVSR hi dword
  push [ebx]      // ; DVSR lo dword

  xor  edi, edi

  mov  eax, [esp+0Ch]    // hi DVND
  or   eax, eax
  jge  short L1
  add  edi, 1
  mov  edx, [esp+8]      // lo DVND
  neg  eax
  neg  edx
  sbb  eax, 0
  mov  [esp+0Ch], eax    // hi DVND
  mov  [esp+8], edx      // lo DVND

L1:                                                               
  mov  eax, [esp+4]      // hi DVSR
  or   eax, eax
  jge  short L2
  add  edi, 1
  mov  edx, [esp]        // lo DVSR
  neg  eax
  neg  edx
  sbb  eax, 0
  mov  [esp+4], eax      // hi DVSR
  mov  [esp], edx        // lo DVSR

L2:
  or   eax, eax
  jnz  short L3
  mov  ecx, [esp]        // lo DVSR
  mov  eax, [esp+0Ch]    // hi DVND
  xor  edx, edx
  div  ecx
  mov  ebx, eax 
  mov  eax, [esp+8]      // lo DVND
  div  ecx

  mov  esi, eax          // result
  jmp  short L4

L3:
  mov  ebx, eax 
  mov  ecx, [esp]        // lo DVSR
  mov  edx, [esp+0Ch]    // hi DVND
  mov  eax, [esp+8]      // lo DVDN
L5:
  shr  ebx, 1 
  rcr  ecx, 1
  shr  edx, 1 
  rcr  eax, 1
  or   ebx, ebx 
  jnz  short L5
  div  ecx
  mov  esi, eax          // result

  // check the result with the original
  mul  [esp+4]           // hi DVSR
  mov  ecx, eax 
  mov  eax, [esp]        // lo DVSR
  mul  esi
  add  edx, ecx

  // carry means Quotient is off by 1
  jb   short L6

  cmp  edx, [esp+0Ch]    // hi DVND
  ja   short L6
  jb   short L7
  cmp  eax, [esp+8]      // lo DVND
  jbe  short L7

L6:
  sub  esi, 1

L7:
  xor  ebx, ebx

L4:
  mov  edx, ebx
  mov  eax, esi

  sub  edi, 1
  jnz  short L8
  neg  edx
  neg  eax
  sbb  edx, 0

L8:
  lea  esp, [esp+10h]
  lea  edi, [ebp+__arg32_1]

  mov  [edi], eax
  pop  ebx
  mov  [edi+4], edx
  mov  esi, [esp]

end

// ; vjumpmr
inline %0ECh

  mov  eax, [ebx - elVMTOffset]
  mov  eax, [eax + __arg32_1]
  jmp  eax

end

// ; jumpmr
inline %0EDh

  jmp __relptr32_2

end

// ; seleqrr
inline %0EEh

  mov   eax, __ptr32_1
  mov   ebx, __ptr32_2
  cmovz ebx, eax

end

// ; openin
inline %0F0h

  push ebp
  mov  ebp, esp
  xor  eax, eax
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
  mov  ebp, esp
  xor  ecx, ecx
  push ecx
  push ecx

end 

// ; openin 3, 0
inline %4F0h

  push ebp
  xor  ecx, ecx
  mov  ebp, esp
  push ecx
  push ecx
  push ecx

end 

// ; openin 0, n
inline %5F0h

  push ebp
  mov  ebp, esp
  xor  ecx, ecx
  sub  esp, __n_2
  push ebp
  push ecx
  mov  ebp, esp

end 

// ; openin i, 0
inline %6F0h

  push ebp
  mov  ebp, esp
  xor  eax, eax
  mov  ecx, __n_1
  sub  esp, __arg32_1
  mov  edi, esp
  rep  stos

end 

// ; xstoresir
inline %0F1h

  mov  eax, __ptr32_2
  mov  [esp+__arg32_1], eax

end

// ; xstoresir :0, ...
inline %1F1h

  mov  esi, __ptr32_2

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

  mov  eax, [ebp+__arg32_2]
  mov  [esp+__arg32_1], eax

end

// ; movsifi sp:0, fp:i2
inline %1F3h

  mov  esi, [ebp+__arg32_2]

end

// ; newir i, r
inline %0F4h

  mov  ecx, __arg32_1
  call %GC_ALLOC

  mov  ecx, __n_1
  mov  eax, __ptr32_2
  mov  [ebx - elSizeOffset], ecx
  mov  [ebx - elVMTOffset], eax

end

// ; newnr n, r
inline %0F5h

  mov  ecx, __arg32_1
  call %GC_ALLOC

  mov  ecx, __n_1
  mov  eax, __ptr32_2
  mov  [ebx - elVMTOffset], eax
  mov  [ebx - elSizeOffset], ecx

end

// ; xmovsisi
inline %0F6h

  mov  eax, [esp+__arg32_2]
  mov  [esp+__arg32_1], eax

end

// ; xmovsisi 0, n
inline %1F6h

  mov  esi, [esp+__arg32_2]

end

// ; xmovsisi n, 0
inline %2F6h

  mov  [esp+__arg32_1], esi

end

// ; xmovsisi 0, 1
inline %5F6h

  mov  esi, [esp+4]

end

// ; xstorefir
inline %0F9h

  mov  eax, __ptr32_2
  mov  [ebp+__arg32_1], eax

end

// ; xdispatchmr
// ; NOTE : __arg32_1 - message; __n_1 - arg count; __ptr32_2 - list, __n_2 - argument list offset
inline % 0FAh

  mov  [esp+4], esi                      // ; saving arg0
  lea  eax, [esp + __n_2]

  mov  esi, __ptr32_2
  push ebx
  xor  edx, edx
  mov  ebx, [esi] // ; message from overload list

labNextOverloadlist:
  shr  ebx, ACTION_ORDER
  mov  edi, mdata : %0
  mov  ebx, [edi + ebx * 8 + 4]
  mov  ecx, __n_1
  lea  ebx, [ebx - 4]

labNextParam:
  sub  ecx, 1
  jnz  short labMatching

  mov  esi, __ptr32_2
  pop  ebx
  mov  eax, [esi + edx * 8 + 4]
  mov  edx, [esi + edx * 8]
  mov  esi, [esp+4]                      // ; restore arg0
  jmp  eax

labMatching:
  mov  edi, [eax + ecx * 4]

  //; check nil
  mov   esi, rdata : %VOIDPTR + elObjectOffset
  test  edi, edi
  cmovz edi, esi

  mov  edi, [edi - elVMTOffset]
  mov  esi, [ebx + ecx * 4]

labNextBaseClass:
  cmp  esi, edi
  jz   labNextParam
  mov  edi, [edi - elPackageOffset]
  and  edi, edi
  jnz  short labNextBaseClass

  mov  esi, __ptr32_2
  add  edx, 1
  mov  ebx, [esi + edx * 8] // ; message from overload list
  and  ebx, ebx
  jnz  labNextOverloadlist

  pop  ebx
  mov  esi, [esp+4]                      // ; restore arg0

end

// ; dispatchmr
// ; NOTE : __arg32_1 - message; __n_1 - arg count; __ptr32_2 - list, __n_2 - argument list offset
inline % 0FBh

  mov  [esp+4], esi                      // ; saving arg0
  lea  eax, [esp + __n_2]

  mov  esi, __ptr32_2
  push ebx
  xor  edx, edx
  mov  ebx, [esi] // ; message from overload list

labNextOverloadlist:
  shr  ebx, ACTION_ORDER
  mov  edi, mdata : %0
  mov  ebx, [edi + ebx * 8 + 4]
  mov  ecx, __n_1
  lea  ebx, [ebx - 4]

labNextParam:
  sub  ecx, 1
  jnz  short labMatching

  mov  esi, __ptr32_2
  pop  ebx
  mov  eax, [esi + edx * 8 + 4]
  mov  edx, [esi + edx * 8]
  mov  ecx, [ebx - elVMTOffset]
  mov  esi, [esp+4]                      // ; restore arg0
  jmp  [ecx + eax * 8 + 4]

labMatching:
  mov  edi, [eax + ecx * 4]

  //; check nil
  mov   esi, rdata : %VOIDPTR + elObjectOffset
  test  edi, edi
  cmovz edi, esi

  mov  edi, [edi - elVMTOffset]
  mov  esi, [ebx + ecx * 4]

labNextBaseClass:
  cmp  esi, edi
  jz   labNextParam
  mov  edi, [edi - elPackageOffset]
  and  edi, edi
  jnz  short labNextBaseClass

  mov  esi, __ptr32_2
  add  edx, 1
  mov  ebx, [esi + edx * 8] // ; message from overload list
  and  ebx, ebx
  jnz  labNextOverloadlist

  pop  ebx
  mov  esi, [esp+4]                      // ; restore arg0

end

// ; vcallmr
inline %0FCh

  mov  eax, [ebx - elVMTOffset]
  call [eax + __arg32_1]

end

// ; callmr
inline %0FDh

  call __relptr32_2

end

// ; callext
inline %0FEh

  mov  [esp], esi
  call extern __ptr32_1
  mov  edx, eax

end

// ; callext
inline %1FEh

  call extern __ptr32_1
  mov  edx, eax

end
