// ; --- Predefined References  --
define INVOKER              10001h
define GC_ALLOC	            10002h
define VEH_HANDLER          10003h

define CORE_TOC             20001h
define SYSTEM_ENV           20002h
define CORE_GC_TABLE   	    20003h
define CORE_THREAD_TABLE    2000Bh
define VOID           	    2000Dh
define VOIDPTR              2000Eh

define ACTION_ORDER              9
define ACTION_MASK            1E0h
define ARG_MASK               05Fh
define ARG_COUNT_MASK         01Fh

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

define et_current            0004h
define tt_stack_frame        0008h

define es_prev_struct        0000h
define es_catch_addr         0004h
define es_catch_level        0008h
define es_catch_frame        000Ch

// ; --- Page Size ----
define page_mask        0FFFFFFF0h
define page_ceil               17h
define page_size_order          4h
define struct_mask_inv     7FFFFFh
define struct_mask         800000h

// ; --- System Core Preloaded Routines --

structure % CORE_TOC

  dd 0         // ; reserved

end

structure % CORE_THREAD_TABLE

  dd 0 // ; et_critical_handler    ; +x00   - pointer to ELENA critical handler
  dd 0 // ; et_current             ; +x04   - pointer to the current exception struct
  dd 0 // ; tt_stack_frame         ; +x08   - pointer to the stack frame

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

  dd 0
  dd data : %CORE_GC_TABLE
  dd data : %CORE_THREAD_TABLE
  dd code : %INVOKER
  dd code : %VEH_HANDLER
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
  // ; save registers
  sub  ecx, eax
  push esi
  push ebp

  // ; lock frame
  mov  [data : %CORE_THREAD_TABLE + tt_stack_frame], esp

  push ecx
  
  // ; create set of roots
  mov  ebp, esp
  xor  ecx, ecx
  push ecx        // ; reserve place 
  push ecx
  push ecx

  // ;   save static roots
  mov  ecx, [rdata : %SYSTEM_ENV]
  mov  esi, stat : %0
  shl  ecx, 2
  push esi
  push ecx

  // ;   collect frames
  mov  eax, [data : %CORE_THREAD_TABLE + tt_stack_frame]  
  mov  ecx, eax

labYGNextFrame:
  mov  esi, eax
  mov  eax, [esi]
  test eax, eax
  jnz  short labYGNextFrame
  
  push ecx
  sub  ecx, esi
  neg  ecx
  push ecx  
  
  mov  eax, [esi + 4]
  test eax, eax
  mov  ecx, eax
  jnz  short labYGNextFrame

  mov [ebp-4], esp      // ; save position for roots

  mov  ebx, [ebp]
  mov  eax, esp

  // ; restore frame to correctly display a call stack
  mov  edx, ebp
  mov  ebp, [edx+4]

  // ; call GC routine
  push edx
  push ebx
  push eax
  call extern "$rt.CollectGCLA"

  mov  ebp, [esp+8] 
  add  esp, 12
  mov  ebx, eax

  mov  esp, ebp 
  pop  ecx 
  pop  ebp
  pop  esi
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
  mov   esi, [esp+4]
                                                                
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

// ; class
inline %8

  mov ebx, [ebx - elVMTOffset] 

end

// ; save
inline %9

  mov  dword ptr [ebx], edx

end

// ; throw
inline %0Ah

  mov  eax, [data : %CORE_THREAD_TABLE + et_current]
  jmp  [eax + es_catch_addr]

end

// ; unhook
inline %0Bh

  mov  edi, [data : %CORE_THREAD_TABLE + et_current]

  mov  eax, [edi + es_prev_struct]
  mov  ebp, [edi + es_catch_frame]
  mov  esp, [edi + es_catch_level]

  mov  [data : %CORE_THREAD_TABLE + et_current], eax

end

// ; loadv
inline % 0Ch

  and  edx, ARG_MASK
  mov  ecx, [ebx]
  and  ecx, ~ARG_MASK
  or   edx, ecx

end

// ; xcmp
inline % 0Dh

  mov  ecx, [ebx]
  cmp  edx, ecx 

end

// ; bload
inline %0Eh

  mov  edx, dword ptr [ebx]
  and  edx, 0FFh 

end

// ; wload
inline %0Fh

  mov  eax, dword ptr [ebx]
  cwde
  mov  edx, eax

end

// ; exclude
inline % 10h
                                                       
  push ebp     
  mov  [data : %CORE_THREAD_TABLE + tt_stack_frame], esp

end

// ; include
inline % 11h

  add  esp, 4                                                       

end

// ; assign
inline %12h

  mov  eax, ebx
  mov  [ebx + edx*4], esi
  // calculate write-barrier address
  sub  eax, [data : %CORE_GC_TABLE + gc_start]
  mov  ecx, [data : %CORE_GC_TABLE + gc_header]
  shr  eax, page_size_order
  mov  byte ptr [eax + ecx], 1  	

end

// ; movfrm
inline %13h

  mov  edx, ebp

end

// ; loads
inline % 14h

  mov   edx, [ebx]
  shr   edx, ACTION_ORDER
  mov   eax, mdata : %0
  mov   edx, [eax + edx]

end

// ; mlen
inline % 15h

  and   edx, ARG_COUNT_MASK

end

// ; dalloc
inline %16h

  lea  eax, [edx*4]
  sub  esp, eax
  mov  ecx, edx
  xor  eax, eax
  mov  edi, esp
  rep  stos

end

// ; xassignsp
inline % 17h

  mov   ebx, esp

end

// ; dtrans
inline %18h

  mov  eax, esi
  mov  ecx, edx
  mov  edi, ebx
  rep  movsd
  mov  esi, eax

end

// ; xassign
inline %19h

  mov  [ebx + edx*4], esi

end

// ; lload
inline %1Ah

  mov  eax, dword ptr [ebx]
  mov  edx, dword ptr [ebx+4]

end

// ; convl
inline % 1Bh

  mov  eax, edx
  cdq

end

// ; xlcmp
inline % 1Ch

  push  eax
  push  edx

  mov   edi, eax
  xor   eax, eax
  sub   edi, [ebx]
  sbb   edx, [ebx+4]
  sets  ah
  or    edx, edi
  setz  al 
  mov   ecx, 1
  cmp   eax, ecx

  pop   edx
  pop   eax

end

// ; coalesce
inline % 20h

   test   ebx, ebx
   cmovz  ebx, esi

end

// ; not
inline % 21h

   not    edx

end

// ; neg
inline % 22h

   neg    edx

end

// ; bread
inline %23h

  xor  eax, eax
  mov  al, byte ptr [esi+edx]
  mov  dword ptr [ebx], eax

end

// ; lsave
inline %24h

  mov  [ebx + 4], edx
  mov  [ebx], eax

end

// ; fsave
inline %25h

  push edx
  fild [esp]
  fstp qword ptr [ebx]
  add  esp, 4

end

// ; wread
inline %26h

  xor  eax, eax
  mov  ax, word ptr [esi+edx]
  mov  dword ptr [ebx], eax

end

// ; xjump
inline %027h

  jmp ebx

end

// ; xget
inline %02Eh

  mov  ebx, [ebx + edx*4]

end

// ; xcall
inline %02Fh

  call ebx

end

// ; setr
inline %80h

  mov  ebx, __ptr32_1

end 

// ; setr 0
inline %180h

  xor  ebx, ebx

end 

// ; setr -1
inline %680h

  xor  ebx, ebx
  dec  ebx

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

// ; xassigni
inline %83h

  mov  [ebx + __arg32_1], esi

end

// ; peekr
inline %84h

  mov  ebx, [__ptr32_1]

end 

// ; storer
inline %85h

  mov  [__ptr32_1], ebx

end 

// ; xswapsi
inline %86h

  mov  eax, [esp+__arg32_1]
  mov  [esp+__arg32_1], esi
  mov  esi, eax

end

// ; xswapsi 0
inline %186h


end

// ; swapsi
inline %87h

  mov  eax, [esp+__arg32_1]
  mov  [esp+__arg32_1], ebx
  mov  ebx, eax

end

// ; xswapsi 0
inline %187h

  mov  eax, ebx
  mov  ebx, esi
  mov  esi, eax

end

// ; movm
inline %88h

  mov  edx, __arg32_1

end

// ; movn
inline %89h

  mov  edx, __n_1

end

// ; loaddp
inline %8Ah

  mov  edx, [ebp + __arg32_1]

end 

// ; xcmpdp
inline %8Bh

  mov  ecx, [ebp + __arg32_1]
  cmp  edx, ecx 

end 

// ; subn
inline %8Ch

  sub  edx, __n_1

end

// ; addn
inline %8Dh

  add  edx, __n_1

end

// ; setfp
inline %08Eh

  lea  ebx, [ebp + __arg32_1]

end 

// ; creater r
inline %08Fh

  mov  eax, [esi]
  mov  ecx, page_ceil
  shl  eax, 2
  add  ecx, eax
  and  ecx, page_mask 
  call %GC_ALLOC

  mov  ecx, [esi]
  shl  ecx, 2
  mov  eax, __ptr32_1
  mov  [ebx - elVMTOffset], eax
  mov  [ebx - elSizeOffset], ecx

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

// ; andn
inline %94h

  and  edx, __n_1

end

// ; readn
inline %95h

  mov  eax, edx
  mov  ecx, __n_1 
  imul eax, ecx
  mov  edi, esi
  add  esi, eax
  mov  eax, edi
  mov  edi, ebx
  rep  movsb
  mov  esi, eax

end

// ; writen
inline %96h

  mov  eax, edx
  mov  ecx, __n_1 
  imul eax, ecx
  mov  edi, esi
  add  esi, eax
  mov  eax, edi
  mov  edi, esi
  mov  esi, ebx
  rep  movsb
  mov  esi, eax

end

// ; cmpn n
inline %097h

  cmp  edx, __n_1

end

// ; nconf dp
inline %098h

  lea   edi, [ebp + __arg32_1]
  fld   qword ptr [ebx]
  fistp dword ptr [edi]

end

// ; ftrunc dp
inline %099h

  lea   edi, [ebp + __arg32_1]

  mov   ecx, 0
  fld   qword ptr [esi]

  push  ecx                // reserve space on stack
  fstcw word ptr [esp]     // get current control word
  mov   edx, [esp]
  or    dx,0c00h           // code it for truncating
  push  edx
  fldcw word ptr [esp]    // change rounding code of FPU to truncate

  frndint                  // truncate the number
  pop   edx                // remove modified CW from CPU stack
  fldcw word ptr [esp]     // load back the former control word
  pop   edx                // clean CPU stack
      
  fstsw ax                 // retrieve exception flags from FPU
  shr   al,1               // test for invalid operation
  jc    short labErr       // clean-up and return error

labSave:
  fstp  qword ptr [edi]    // store result
  jmp   short labEnd
  
labErr:
  ffree st(1)
  
labEnd:

end

// ; dcopy
inline %9Ah

  mov  ecx, __n_1 
  imul ecx, edx
  mov  eax, esi
  mov  edi, ebx
  rep  movsb
  mov  esi, eax

end

// ; orn
inline %9Bh

  or  edx, __n_1

end

// ; savedp
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

// ; geti
inline %0A5h

  mov  ebx, [ebx + __arg32_1]

end

// ; assigni
inline %0A6h

  mov  eax, ebx
  mov  [ebx + __arg32_1], esi
  // calculate write-barrier address
  sub  eax, [data : %CORE_GC_TABLE + gc_start]
  mov  ecx, [data : %CORE_GC_TABLE + gc_header]
  shr  eax, page_size_order
  mov  byte ptr [eax + ecx], 1  	

end

// ; xrefreshsi i
inline %0A7h

end 

// ; xrefreshsi 0
inline %1A7h

  mov esi, [esp+4]

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

// ; lsavedp
inline %0AAh

  lea  edi, [ebp + __arg32_1]
  mov  [edi], eax
  mov  [edi + 4], edx

end

// ; lsavesi
inline %0ABh

  lea  edi, [esp + __arg32_1]
  mov [edi], eax
  mov [edi+4], edx

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

// ; jumpvi
inline %0B5h

  mov  eax, [ebx - elVMTOffset]
  jmp  [eax + __arg32_1]

end

// ; xredirect
inline % 0B6h // (ebx - object, edx - message, esi - arg0, edi - arg1)

  mov   [esp+4], esi                      // ; saving arg0
  xor   ecx, ecx
  push  edx 
  mov   edi, [ebx - elVMTOffset]
  mov   eax, __arg32_1
  and   edx, ARG_MASK
  and   eax, ~ARG_MASK
  mov   esi, [edi - elVMTSizeOffset]
  or    edx, eax

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
  pop   edx 
  mov   eax, [edi+esi*8+4]
  mov   esi, [esp+4]
  jmp   eax

labEnd:
  pop   edx 
                                                                
end

// ; cmpr r
inline %0C0h

  cmp  ebx, __ptr32_1

end 

// ; cmpr 0
inline %1C0h

  cmp  ebx, 0

end 

// ; cmpr -1
inline %6C0h

  cmp  ebx, -1

end 

// ; fcmpn 8
inline %0C1h

  xor    eax, eax
  fld    qword ptr [ebx]
  mov    ecx, 1
  fld    qword ptr [esi]
  fcomip st, st(1)
  sete   al
  seta   ah
  fstp   st(0)
  cmp    eax, ecx

end

// ; icmpn 4
inline %0C2h

  mov  eax, [esi]
  cmp  eax, [ebx]

end

// ; icmpn 1
inline %1C2h

  mov  eax, [esi]
  cmp  al, byte ptr [ebx]

end

// ; icmpn 2
inline %2C2h

  mov  eax, [esi]
  cmp  ax, word ptr [ebx]

end

// ; icmpn 8
inline %4C2h

  xor   eax, eax
  mov   edi, [esi]
  sub   edi, [ebx]
  mov   ecx, [esi+4]
  sbb   ecx, [ebx+4]
  sets  ah
  or    ecx, edi
  setz  al 
  mov   ecx, 1
  cmp   ecx, eax

end

// ; tstflg
inline %0C3h

  mov  ecx, [ebx - elVMTOffset] 
  mov  eax, [ecx - elVMTFlagOffset]
  test eax, __n_1

end

// ; tstn
inline %0C4h

  test edx, __n_1

end

// ; tstm
inline % 0C5h // (ebx - object)

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
  mov   eax, __arg32_1
  cmp   eax, [edi+esi*8]
  je    short labFound
  lea   eax, [edi+esi*8]
  jb    short labSplit
  lea   edi, [eax+8]
  sub   esi, ecx
  jmp   short labSplit

labFound:
  mov   esi, 1

labEnd:
  cmp   esi, 1
  mov   esi, [esp+4]                                                              

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

// ; faddndp
inline %0D0h

  lea  edi, [ebp + __arg32_1]

  fld   qword ptr [edi]
  fadd  qword ptr [esi] 
  fstp  qword ptr [edi]

end

// ; fsubndp
inline %0D1h

  lea  edi, [ebp + __arg32_1]

  fld   qword ptr [edi]
  fsub  qword ptr [esi] 
  fstp  qword ptr [edi]

end

// ; fmulndp
inline %0D2h

  lea  edi, [ebp + __arg32_1]

  fld   qword ptr [edi]
  fmul  qword ptr [esi] 
  fstp  qword ptr [edi]

end

// ; fdivndp
inline %0D3h

  lea  edi, [ebp + __arg32_1]

  fld   qword ptr [edi]
  fdiv  qword ptr [esi] 
  fstp  qword ptr [edi]

end

// ; ianddpn
inline %0D8h

  lea  edi, [ebp + __arg32_1]
  mov  eax, [esi]
  and  [edi], eax

end

// ; ianddpn
inline %1D8h

  lea  edi, [ebp + __arg32_1]
  mov  eax, [esi]
  and  byte ptr [edi], al

end

// ; ianddpn
inline %2D8h

  lea  edi, [ebp + __arg32_1]
  mov  eax, [esi]
  and  word ptr [edi], ax

end

// ; ianddpn
inline %4D8h

  lea  edi, [ebp + __arg32_1]
  mov  eax, [esi + 4]
  mov  ecx, [esi]
  and  [edi], ecx
  and  [edi+4], eax

end

// ; iordpn
inline %0D9h

  lea  edi, [ebp + __arg32_1]
  mov  eax, [esi]
  or   [edi], eax

end

// ; iordpn
inline %1D9h

  lea  edi, [ebp + __arg32_1]
  mov  eax, [esi]
  or   byte ptr [edi], al

end

// ; iordpn
inline %2D9h

  lea  edi, [ebp + __arg32_1]
  mov  eax, [esi]
  or   word ptr [edi], ax

end

// ; iordpn
inline %4D9h

  lea  edi, [ebp + __arg32_1]
  mov  eax, [esi + 4]
  mov  ecx, [esi]
  or   [edi], ecx
  or   [edi+4], eax

end

// ; ixordpn
inline %0DAh

  lea  edi, [ebp + __arg32_1]
  mov  eax, [esi]
  xor  [edi], eax

end

// ; ixordpn
inline %1DAh

  lea  edi, [ebp + __arg32_1]
  mov  eax, [esi]
  xor  byte ptr [edi], al

end

// ; ixordpn
inline %2DAh

  lea  edi, [ebp + __arg32_1]
  mov  eax, [esi]
  xor  word ptr [edi], ax

end

// ; ixordpn
inline %4DAh

  lea  edi, [ebp + __arg32_1]
  mov  eax, [esi + 4]
  mov  ecx, [esi]
  xor  [edi], ecx
  xor  [edi+4], eax

end

// ; inotdpn
inline %0DBh

  lea  edi, [ebp + __arg32_1]
  mov  eax, [esi]
  not  eax 
  mov  [edi], eax

end

// ; inotdpn 1
inline %1DBh

  lea  edi, [ebp + __arg32_1]
  mov  eax, [esi]
  not  eax 
  mov  byte ptr [edi], al

end

// ; inotdpn 2
inline %2DBh

  lea  edi, [ebp + __arg32_1]
  mov  eax, [esi]
  not  eax 
  mov  word ptr [edi], ax

end

// ; inotdpn 8
inline %4DBh

  lea  edi, [ebp + __arg32_1]
  mov  eax, [esi + 4]
  mov  ecx, [esi]
  not  eax 
  not  ecx 
  mov  [edi], ecx
  mov  [edi+4], eax

end

// ; ishldpn
inline %0DCh

  lea  edi, [ebp + __arg32_1]
  mov  ecx, [esi]
  mov  eax, [edi]
  shl  eax, cl
  mov  [edi], eax

end

// ; ishldpn 1
inline %1DCh

  lea  edi, [ebp + __arg32_1]
  mov  ecx, [esi]
  mov  eax, [edi]
  shl  eax, cl
  mov  byte ptr [edi], al

end

// ; ishldpn 2
inline %2DCh

  lea  edi, [ebp + __arg32_1]
  mov  ecx, [esi]
  mov  eax, [edi]
  shl  eax, cl
  mov  word ptr [edi], ax

end

// ; ishldpn 8
inline %4DCh

  push edx
  lea  edi, [ebp + __arg32_1]
  mov  ecx, [esi]
  mov  eax, [edi]
  mov  edx, [edi+4]

  cmp  cl, 40h 
  jae  short lErr
  cmp  cl, 20h
  jae  short LL32
  shld eax, edx, cl
  shl  edx, cl
  jmp  short lEnd

LL32:
  mov  edx, eax
  xor  eax, eax
  sub  cl, 20h
  shl  eax, cl 
  jmp  short lEnd
  
lErr:
  xor  eax, eax
  xor  edx, edx
  jmp  short lEnd2

lEnd:
  mov  [edi], eax
  mov  [edi+4], edx

lEnd2:
  pop   edx

end

// ; ishrdpn
inline %0DDh

  lea  edi, [ebp + __arg32_1]
  mov  ecx, [esi]
  mov  eax, [edi]
  shr  eax, cl
  mov  [edi], eax

end

// ; ishrdpn 1
inline %1DDh

  lea  edi, [ebp + __arg32_1]
  mov  ecx, [esi]
  mov  eax, [edi]
  shr  eax, cl
  mov  byte ptr [edi], al

end

// ; ishrdpn 2
inline %2DDh

  lea  edi, [ebp + __arg32_1]
  mov  ecx, [esi]
  mov  eax, [edi]
  shr  eax, cl
  mov  word ptr [edi], ax

end

// ; ishrdpn 8
inline %4DDh

  push edx
  lea  edi, [ebp + __arg32_1]
  mov  ecx, [esi]
  mov  eax, [edi]
  mov  edx, [edi+4]

  cmp  cl, 64
  jae  short lErr

  cmp  cl, 32
  jae  short LR32
  shrd eax, edx, cl
  sar  edx, cl
  jmp  short lEnd

LR32:
  mov  eax, edx
  xor  edx, edx
  sub  cl, 20h
  shr  eax, cl 
  jmp  short lEnd
  
lErr:
  xor  eax, eax
  xor  edx, edx
  jmp  short lEnd2

lEnd:
  mov  [edi], eax
  mov  [edi+4], edx

lEnd2:
  pop   edx

end

// ; copydpn
inline %0E0h

  mov  eax, esi
  lea  edi, [ebp + __arg32_1]
  mov  ecx, __n_2
  rep  movsb
  mov  esi, eax

end

// ; copydpn dpn, 1
inline %1E0h

  mov  eax, [esi]
  mov  byte ptr [ebp + __arg32_1], al

end

// ; copydpn dpn, 2
inline %2E0h

  mov  eax, [esi]
  mov  word ptr [ebp + __arg32_1], ax

end

// ; copydpn dpn, 4
inline %3E0h

  mov  eax, [esi]
  mov  [ebp + __arg32_1], eax

end

// ; copydpn dpn, 8
inline %4E0h

  mov  eax, [esi]
  lea  edi, [ebp + __arg32_1]
  mov  ecx, [esi+4]
  mov  [edi], eax
  mov  [edi + 4], ecx

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
  mov  ecx, [edx]
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

// ; nsavedpn
inline %0E5h

  mov  eax, __n_2
  mov  [ebp+__arg32_1], eax

end

// ; xhookdpr
inline %0E6h

  lea  edi, [ebp + __arg32_1]
  mov  eax, [data : %CORE_THREAD_TABLE + et_current]

  mov  [edi + es_prev_struct], eax
  mov  [edi + es_catch_frame], ebp
  mov  [edi + es_catch_level], esp
  mov  [edi + es_catch_addr], __ptr32_2

  mov  [data : %CORE_THREAD_TABLE + et_current], edi

end

// ; xnewnr
inline %0E7h

  lea  ebx, [ebx + elObjectOffset]
  mov  ecx, __n_1
  mov  eax, __ptr32_2
  mov  [ebx - elVMTOffset], eax
  mov  [ebx - elSizeOffset], ecx

end

// ; nadddpn
inline %0E8h

  mov  eax, __n_2
  add  [ebp+__arg32_1], eax

end

// ; dcopydpn
inline %0E9h

  mov  eax, esi
  lea  edi, [ebp + __arg32_1]
  mov  ecx, __n_2
  imul ecx, edx
  rep  movsb
  mov  esi, eax

end

// ; xwriteon
inline %0EAh

  mov  eax, esi

  mov  edi, esi
  mov  ecx, __n_2 
  lea  esi, [ebx + __arg32_1]
  rep  movsb

  mov  esi, eax

end

// ; xcopyon
inline %0EBh

  mov  ecx, __n_2 
  lea  edi, [ebx + __arg32_1]
  rep  movsb
  sub  esi, __n_2          // ; to set back ESI register

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

// ; selltrr
inline %0EFh

  mov   eax, __ptr32_1
  mov   ebx, __ptr32_2
  cmovl ebx, eax

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

// ; xstoresir :0, 0
inline %6F1h

  mov  esi, 0

end

// ; openheaderin
inline %0F2h

  finit

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

// ; createnr n,r
inline %0F7h

  mov  eax, [esi]
  mov  ecx, page_ceil
  imul eax, __n_1
  add  ecx, eax
  and  ecx, page_mask 
  call %GC_ALLOC

  mov  ecx, [esi]
  mov  eax, __n_1
  imul ecx, eax
  mov  eax, __ptr32_2
  or   ecx, struct_mask
  mov  [ebx - elVMTOffset], eax
  mov  [ebx - elSizeOffset], ecx

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
  mov  edx, __arg32_1

end

// ; dispatchmr
// ; NOTE : __arg32_1 - variadic message; __n_1 - arg count; __ptr32_2 - list, __n_2 - argument list offset
inline % 5FAh

  lea  eax, [esp + __n_2]
  xor  ecx, ecx
  push ecx
  push ecx
  mov  [esp+4], esi                      // ; saving arg0
  push ebx
  mov  ebx, eax 

labCountParam:
  lea  ebx, [ebx+4]
  cmp  [ebx], -1
  lea  ecx, [ecx+1]
  jnz  short labCountParam
  mov  [esp+4], ecx 

  mov  esi, __ptr32_2
  xor  edx, edx
  mov  ebx, [esi] // ; message from overload list

labNextOverloadlist:
  shr  ebx, ACTION_ORDER
  mov  edi, mdata : %0
  mov  ebx, [edi + ebx * 8 + 4]
  xor  ecx, ecx

  lea  ebx, [ebx - 4]
  mov  [esp+8], ebx

labNextParam:
  add  ecx, 1
  cmp  ecx, [esp+4]
  jnz  short labMatching

  mov  esi, __ptr32_2
  pop  ebx
  mov  eax, [esi + edx * 8 + 4]
  add  esp, 8
  mov  edx, [esi + edx * 8]
  mov  esi, [esp+4]                      // ; restore arg0
  jmp  eax

labMatching:
  mov    esi, [esp+8]
  lea    edi, [esi+4]
  cmp    [edi], 0
  cmovnz esi, edi
  mov    [esp+8], esi

  mov  edi, [eax + ecx * 4]

  //; check nil
  mov   esi, rdata : %VOIDPTR + elObjectOffset
  test  edi, edi
  cmovz edi, esi

  mov  edi, [edi - elVMTOffset]
  mov  esi, [esp+8]
  mov  esi, [esi]

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
  mov  edx, __arg32_1

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
  jmp  [ecx + eax + 4]

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
  mov  edx, __arg32_1

end

// ; dispatchmr
// ; NOTE : __arg32_1 - message; __n_1 - arg count; __ptr32_2 - list, __n_2 - argument list offset
inline % 5FBh

  xor  ecx, ecx
  lea  eax, [esp + __n_2]
  push ecx
  push ecx
  mov  [esp+4], esi                      // ; saving arg0
  push ebx
  mov  ebx, eax 

labCountParam:
  lea  ebx, [ebx+4]
  cmp  [ebx], -1
  lea  ecx, [ecx+1]
  jnz  short labCountParam
  mov  [esp+4], ecx 

  mov  esi, __ptr32_2
  xor  edx, edx
  mov  ebx, [esi] // ; message from overload list

labNextOverloadlist:
  shr  ebx, ACTION_ORDER
  mov  edi, mdata : %0
  mov  ebx, [edi + ebx * 8 + 4]
  xor  ecx, ecx

  lea  ebx, [ebx - 4]
  mov  [esp+8], ebx

labNextParam:
  add  ecx, 1
  cmp  ecx, [esp+4]
  jnz  short labMatching

  mov  esi, __ptr32_2
  pop  ebx
  mov  eax, [esi + edx * 8 + 4]
  add  esp, 8
  mov  edx, [esi + edx * 8]
  mov  ecx, [ebx - elVMTOffset]
  mov  esi, [esp+4]                      // ; restore arg0
  jmp  [ecx + eax + 4]

labMatching:
  mov    esi, [esp+8]
  lea    edi, [esi+4]
  cmp    [edi], 0
  cmovnz esi, edi
  mov    [esp+8], esi

  mov  edi, [eax + ecx * 4]

  //; check nil
  mov   esi, rdata : %VOIDPTR + elObjectOffset
  test  edi, edi
  cmovz edi, esi

  mov  edi, [edi - elVMTOffset]
  mov  esi, [esp+8]
  mov  esi, [esi]

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
  mov  edx, __arg32_1

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
