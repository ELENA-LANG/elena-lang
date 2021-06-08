// --- Predefined References  --
define GC_ALLOC	            10001h
define HOOK                 10010h
define INVOKER              10011h
define INIT_RND             10012h
define CALC_SIZE            1001Fh
define GET_COUNT            10020h
define THREAD_WAIT          10021h
define GC_ALLOCPERM	    10031h

define CORE_GC_TABLE        20002h
define CORE_STATICROOT      20005h
define CORE_TLS_INDEX       20007h
define THREAD_TABLE         20008h
define CORE_MESSAGE_TABLE   2000Ah
define CORE_ET_TABLE        2000Bh
define SYSTEM_ENV           2000Ch
define VOID           	    2000Dh
define VOIDPTR              2000Eh

// GC TABLE OFFSETS
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
define gc_et_current         002Ch 
define gc_stack_frame        0030h 
define gc_lock               0034h 
define gc_signal             0038h 
define tt_ptr                003Ch 
define tt_lock               0040h 
define gc_rootcount          004Ch
define gc_perm_start         0050h 
define gc_perm_end           0054h 
define gc_perm_current       0058h 

// SYSTEM_ENV OFFSETS
define se_mgsize	     0014h
define se_ygsize	     001Ch

// Page Size
define page_size               10h
define page_size_order          4h
define page_size_order_minus2   2h
define page_mask        0FFFFFFF0h
define page_ceil               17h
define struct_mask         800000h
define struct_mask_inv     7FFFFFh

// Object header fields
define elSizeOffset          0004h
define elVMTOffset           0008h 
define elObjectOffset        0008h

define elPageSizeOffset     0004h
define elPageVMTOffset      0000h

// VMT header fields
define elVMTSizeOffset       0004h
define elVMTFlagOffset       000Ch
define elPackageOffset       0010h

define page_align_mask   000FFFF0h

define ACTION_ORDER              9
define ARG_ACTION_MASK        1DFh
define ACTION_MASK            1E0h
define ARG_MASK               01Fh
define ARG_MASK_INV     0FFFFFFE0h

// --- System Core Preloaded Routines --

structure % CORE_ET_TABLE

  dd 0 // ; critical_exception    ; +x00   - pointer to critical exception handler

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
  dd 0 // ; gc_et_current         : +2Ch 
  dd 0 // ; gc_stack_frame        : +30h 
  dd 0 // ; gc_lock               : +34h 
  dd 0 // ; gc_signal             : +38h 
  dd 0 // ; tt_ptr                : +3Ch 
  dd 0 // ; tt_lock               : +40h 
  dd 0 // ; dbg_ptr               : +44h 
  dd 0 // ; gc_roots              : +48h 
  dd 0 // ; gc_rootcount          : +4Ch 
  dd 0 // ; gc_perm_start         : +50h 
  dd 0 // ; gc_perm_end           : +54h 
  dd 0 // ; gc_perm_current       : +58h 

end

// ; NOTE : the table is tailed with GCMGSize,GCYGSize and MaxThread fields
rstructure %SYSTEM_ENV

  dd 0
  dd data : %CORE_STATICROOT
  dd data : %CORE_GC_TABLE
  dd data : %CORE_TLS_INDEX
  dd data : %THREAD_TABLE
  dd code : %INVOKER

end

rstructure %VOID

  dd 0
  dd 0  // ; a reference to the super class class
  dd 0
  dd 0  
  dd 0

end

rstructure %VOIDPTR

  dd rdata : %VOID + elPackageOffset
  dd 0
  dd 0

end

// --- GC_ALLOC ---
// in: ecx - size ; out: ebx - created object
procedure %GC_ALLOC

  mov  eax, [data : %CORE_GC_TABLE + gc_yg_current]
  mov  edx, [data : %CORE_GC_TABLE + gc_yg_end]
  add  ecx, eax
  cmp  ecx, edx
  jae  short labYGCollect
  mov  [data : %CORE_GC_TABLE + gc_yg_current], ecx
  lea  ebx, [eax + elObjectOffset]
  ret

labYGCollect:
  // ; restore ecx
  sub  ecx, eax

  // ; save registers
  push ebp

  // ; lock frame
  mov  [data : %CORE_GC_TABLE + gc_stack_frame], esp

  push ecx
  
  // ; create set of roots
  mov  ebp, esp
  xor  ecx, ecx
  push ecx        // ; reserve place 
  push ecx
  push ecx

  // ; save static roots
  mov  esi, data : %CORE_STATICROOT
  mov  ecx, [data : %CORE_GC_TABLE + gc_rootcount]
  push esi
  push ecx

  // ; collect frames
  mov  eax, [data : %CORE_GC_TABLE + gc_stack_frame]  
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

  // ; restore rbp to correctly display a call stack
  mov  edx, ebp
  mov  ebp, [edx+4]

  push edx
  push ebx
  push eax
  call extern 'rt_dlls.GCCollect

  mov  ebx, eax
  add  esp, 8
  pop  ebp

  mov  esp, ebp 
  pop  ecx 
  pop  ebp

  ret

end

// --- GC_ALLOCPERM ---
// in: ecx - size ; out: ebx - created object
procedure %GC_ALLOCPERM

  mov  eax, [data : %CORE_GC_TABLE + gc_perm_current]
  mov  edx, [data : %CORE_GC_TABLE + gc_perm_end]
  add  ecx, eax
  cmp  ecx, edx
  jae  short labPERMCollect
  mov  [data : %CORE_GC_TABLE + gc_perm_current], ecx
  lea  ebx, [eax + elObjectOffset]
  ret

labPERMCollect:
  // ; restore ecx
  sub  ecx, eax

  // ; lock frame
  mov  [data : %CORE_GC_TABLE + gc_stack_frame], esp

  push ecx
  call extern 'rt_dlls.GCCollectPerm

  mov  ebx, eax
  add  esp, 4

  ret

end

// ; --- HOOK ---
// ; in: ecx - catch offset
procedure %HOOK

  mov  eax, [esp]       
  lea  ecx, [eax + ecx - 5]               
  ret

end

// --- System Core Functions --

procedure % THREAD_WAIT

  ret
  
end

procedure % CALC_SIZE

  mov  ecx, edx
  add  ecx, page_ceil
  and  ecx, page_mask

  ret

end

procedure % GET_COUNT

  mov  edx, [ebx - elSizeOffset]
  test edx, struct_mask
  jnz  short labErr
  and  edx, 0FFFFFFh
  shr  edx, 2
  ret

labErr:
  xor  edx, edx
  ret 

end

// ; ==== Command Set ==

// ; coalesce
inline % 2

  mov    eax, [esp]
  test   ebx, ebx
  cmovz  ebx, eax

end

// ; peek
inline % 3

  mov  eax, [esp]
  mov  ebx, [eax+edx*4] 

end

// ; snop
inline % 4

  nop

end

// ; pushverb
inline % 5

  mov   eax, edx
  mov   ecx, edx
  shr   eax, ACTION_ORDER
  mov   edi, rdata : % CORE_MESSAGE_TABLE
  test  ecx, ecx
  cmovs eax, [edi + edx]
  push  eax

end

// ; loadverb
inline % 6

  mov   ecx, edx
  shr   edx, ACTION_ORDER
  mov   eax, rdata : % CORE_MESSAGE_TABLE
  test  ecx, ecx
  cmovs edx, [eax + edx]

end

// ; throw
inline % 7

  mov  eax, [data : %CORE_GC_TABLE + gc_et_current]
  jmp  [eax]

end

// ; push
inline % 09h

   mov  eax, [ebx + edx * 4]
   push eax

end

// ; xnew
inline % 0Ch

  mov  eax, [esp]
  mov  ecx, page_ceil
  push ebx
  mov  edx, [eax]
  lea  ecx, [ecx + edx*4]
  and  ecx, page_mask 
 
  call code : %GC_ALLOC

  pop   edi
  mov   eax, [esp]
  xor   edx, edx
  mov   [ebx-elVMTOffset], edi
  mov   ecx, [eax]
  mov   esi, struct_mask
  test  ecx, ecx
  cmovz edx, esi
  shl   ecx, 2
  or    ecx, edx
  mov   [ebx-elSizeOffset], ecx

end

// ; storev
inline % 0Dh

  mov eax, edx
  and eax, ACTION_MASK
  mov ecx, [esp]
  and ecx, ARG_MASK
  or  ecx, edx
  mov [esp], ecx

end

// ; bsredirect
inline % 0Eh // (ebx - object, edx - message)

  mov  edi, [ebx - elVMTOffset]
  xor  ecx, ecx
  mov  esi, [edi - elVMTSizeOffset]

labSplit:
  test esi, esi
  jz   short labEnd

labStart:
  shr   esi, 1
  setnc cl
  cmp   edx, [edi+esi*8]
  je    short labFound
  lea   eax, [edi+esi*8]
  jb    short labSplit
  lea   edi, [eax+8]
  sub   esi, ecx
  jmp   labSplit
  nop
  nop
labFound:
  jmp   [edi+esi*8+4]

labEnd:
                                                                
end

// ; setv
inline % 0Fh

  mov eax, [ebx]
  and eax, ARG_ACTION_MASK
  mov ecx, edx
  shl ecx, ACTION_ORDER
  or  eax, ecx
  mov [ebx], eax

end

// ; open
inline % 11h

  push ebp
  mov  ebp, esp

end

// ; sub
inline % 13h

  sub  edx, [ebx]
  
end

// ; swapd
inline % 14h

  mov  eax, [esp]
  mov  [esp], edx
  mov  edx, eax 

end

// ; close
inline % 15h

  mov  esp, ebp
  pop  ebp
  
end

// ; rexp
inline % 16h

  mov   eax, [esp]
  mov   edx, 0
  fld   qword ptr [eax]   // ; Src

  fldl2e                  // ; ->log2(e)
  fmulp                   // ; ->log2(e)*Src
                                                              
  // ; the FPU can compute the antilog only with the mantissa
  // ; the characteristic of the logarithm must thus be removed
      
  fld   st(0)             // ; copy the logarithm
  frndint                 // ; keep only the characteristic
  fsub  st(1),st(0)       // ; keeps only the mantissa
  fxch                    // ; get the mantissa on top

  f2xm1                   // ; ->2^(mantissa)-1
  fld1
  faddp                   // ; add 1 back

  //; the number must now be readjusted for the characteristic of the logarithm

  fscale                  // ;, scale it with the characteristic
      
  fstsw ax                // ; retrieve exception flags from FPU
  shr   al,1              // ; test for invalid operation
  jc    short lErr        // ; clean-up and return if error
      
  // ; the characteristic is still on the FPU and must be removed
  
  fstp  st(1)             // ; get rid of the characteristic

  fstp  qword ptr [ebx]   // ; store result 
  mov   edx, 1
  jmp   short labEnd
  
lErr:
  ffree st(1)
  
labEnd:

end

// ; get
inline % 18h

   mov  ebx, [ebx + edx * 4]

end

// ; set
inline % 19h
                                
   // ; calculate write-barrier address
   mov  ecx, ebx
   mov  esi, [data : %CORE_GC_TABLE + gc_header]
   sub  ecx, [data : %CORE_GC_TABLE + gc_start]
   mov  eax, [esp]
   shr  ecx, page_size_order
   mov  [ebx + edx*4], eax
   mov  byte ptr [ecx + esi], 1  

end

// ; swap
inline % 1Ah

  mov eax, [esp]
  mov [esp], ebx
  mov ebx, eax 

end

// ; mquit
inline % 1Bh

  mov  ecx, edx
  pop  esi
  and  ecx, ARG_MASK
  lea  esp, [esp + ecx * 4]
  jmp  esi
  nop
  nop
 
end

// ; count
inline % 1Ch

  mov  ecx, 0FFFFFh
  mov  edx, [ebx-elSizeOffset]
  and  edx, ecx
  shr  edx, 2

end

// ; unhook
inline % 1Dh

  mov  esi, [data : %CORE_GC_TABLE + gc_et_current]
  mov  esp, [esi + 4]
  mov  ebp, [esi + 8]
  pop  ecx
  mov  [data : %CORE_GC_TABLE + gc_et_current], ecx
  
end

// ; rsin
inline % 1Eh

  mov   eax, [esp]
  fld   qword ptr [eax]  
  fldpi
  fadd  st(0),st(0)       // ; ->2pi
  fxch

lReduce:
  fprem                   // ; reduce the angle
  fsin
  fstsw ax                // ; retrieve exception flags from FPU
  shr   al,1              // ; test for invalid operation
  // ; jc    short lErr        // ; clean-up and return error
  sahf                    // ; transfer to the CPU flags
  jpe   short lReduce     // ; reduce angle again if necessary
  fstp  st(1)             // ; get rid of the 2pi

  fstp  qword ptr [ebx]    // ; store result 

end

// ; allocd
inline % 1Fh

  mov  ecx, edx
  mov  edi, edx
  shl  edi, 2
  sub  esp, edi
  xor  eax, eax
  mov  edi, esp
  rep  stos

end

// ; rcos
inline % 20h

  mov   eax, [esp]
  fld   qword ptr [eax]  
  fcos
  fstp  qword ptr [ebx]    // store result 

end

// ; rarctan
inline % 21h

  mov   eax, [esp]
  fld   qword ptr [eax]  
  fld1
  fpatan                  // i.e. arctan(Src/1)
  fstp  qword ptr [ebx]    // store result 

end

// ; xtrans
inline % 24h

  mov  eax, [esp]
  mov  ecx, [eax+edx*4]
  mov  [ebx+edx*4], ecx                                                   

end

// ; include
inline % 25h

  add  esp, 4                                                       

end

// ; exclude
inline % 26h
                                                       
  push ebp     
  mov  [data : %CORE_GC_TABLE + gc_stack_frame], esp

end

// ; trylock
inline % 27h

  xor  edx, edx

end

// ; freelock
inline % 28h

  nop

end

// ; freed
inline % 29h

  mov  edi, edx
  shl  edi, 2
  add  esp, edi

end

// ; loadenv
inline % 2Ah

  mov  edx, rdata : %SYSTEM_ENV

end

// ; store
inline % 2Bh

  mov  eax, [esp]
  mov  dword ptr [eax+edx*4], ebx

end

// ; rln
inline % 2Ch

  mov   eax, [esp]
  mov   edx, 0
  fld   qword ptr [eax]  
  
  fldln2
  fxch
  fyl2x                   // ->[log2(Src)]*ln(2) = ln(Src)

  fstsw ax                // retrieve exception flags from FPU
  shr   al,1              // test for invalid operation
  jc    short lErr        // clean-up and return error

  fstp  qword ptr [ebx]    // store result 
  mov   edx, 1
  jmp   short labEnd

lErr:
  ffree st(0)

labEnd:

end

// ; read
inline % 2Dh

  mov  edx, [ebx + edx]

end

// ; clone
inline % 02Eh

  mov  ecx, [ebx - elSizeOffset]
  mov  esi, [esp]
  and  ecx, struct_mask_inv
  mov  edi, ebx
  add  ecx, 3
  shr  ecx, 2
  rep  movsd

end

// ; xset
inline % 2Fh
            
   mov  eax, [esp]                  
   mov  [ebx + edx * 4], eax

end

// ; rabs
inline %30h

  mov   eax, [esp]
  fld   qword ptr [eax]  
  fabs
  fstp  qword ptr [ebx]    // ; store result 
  
end

// ; len
inline % 31h

  mov  edx, 0FFFFFh
  mov  ecx, [ebx-elSizeOffset]
  and  edx, ecx

end

// ; rload
inline %32h

  fld   qword ptr [ebx]

end

// ; flag
inline % 33h

  mov  eax, [ebx - elVMTOffset]
  mov  edx, [eax - elVMTFlagOffset]
  
end

// ; parent
inline % 35h

  mov ebx, [ebx - elPackageOffset]

end

// ; class
inline % 36h

  mov ebx, [ebx - elVMTOffset]

end

// ; mindex
inline % 37h

  mov  ecx, edx
  mov  edi, [ebx - elVMTOffset]
  xor  edx, edx
  mov  esi, [edi - elVMTSizeOffset]

labSplit:
  test esi, esi
  jz   short labEnd

labStart:
  shr   esi, 1
  setnc dl
  cmp   ecx, [edi+esi*8]
  jb    short labSplit
  nop
  nop
  jz    short labFound
  lea   edi, [edi+esi*8+8]
  sub   esi, edx
  jnz   short labStart
  nop
  nop
labEnd:
  mov  edx, 0FFFFFFFFh
labFound:

end

// ; rround
inline %3Dh

  mov   eax, [esp]
  mov   edx, 0
  fld   qword ptr [eax]  

  push  eax               // ; reserve space on CPU stack

  fstcw word ptr [esp]    // ;get current control word
  mov   ax,[esp]
  and   ax,0F3FFh         // ; code it for rounding 
  push  eax
  fldcw word ptr [esp]    // ; change rounding code of FPU to round

  frndint                 // ; round the number
  pop   eax               // ; get rid of last push
  fldcw word ptr [esp]    // ; load back the former control word

  fstsw ax                // ; retrieve exception flags from FPU
  shr   al,1              // ; test for invalid operation
  pop   ecx               // ; clean CPU stack
  jc    short lErr        // ; clean-up and return error
  
  fstp  qword ptr [ebx]   // ; store result 
  mov   edx, 1
  jmp   short labEnd
  
lErr:
  ffree st(0)

labEnd:
  
end

// ; equal
inline % 3Eh

  mov  eax, [esp]
  xor  edx, edx
  cmp  eax, ebx
  setz dl

end

// ; nequal
inline % 40h

  mov  eax, [esp]
  xor  edx, edx
  mov  ecx, [ebx]
  cmp  ecx, [eax]
  setz dl

end

// ; nless
inline % 41h

  mov  eax, [esp]
  xor  edx, edx
  mov  ecx, [ebx]
  cmp  ecx, [eax]
  setl dl

end

// ; lequal

inline % 43h

  mov  edi, [esp]
  mov  ecx, [ebx]
  xor  edx, edx
  mov  esi, [ebx+4]  
  cmp  ecx, [edi]
  setz dl
  cmp  esi, [edi+4]
  setz cl
  and  dl, cl

end

// ; lless(lo, ro, tr, fr)
inline % 44h

  mov  edi, [esp]
  xor  edx, edx
  xor  ecx, ecx
  mov  esi, [ebx]
  cmp  esi, [edi]
  setl cl
  mov  esi, [ebx+4]  
  cmp  esi, [edi+4]
  setl dl
  cmovz edx, ecx

end

// ; rset (src, tgt)
inline % 45h

  push edx
  fild dword ptr [esp]
  pop  edx

end

// ; rsave
inline % 46h

  fstp qword ptr [ebx]

end

// ; save
inline % 47h

  mov [ebx], edx

end

// ; load
inline % 48h

  mov edx, [ebx]

end

// ; rsaven
inline % 49h

  fistp dword ptr [ebx]

end
               
// ; rsavel
inline % 4Ah

  fistp qword ptr [ebx]

end
               
// ; lsave
inline % 4Bh

  mov  [ebx], edx
  mov  [ebx+4], edi

end
               
// ; lload
inline % 4Ch

  mov  eax, edx
  cdq
  mov  [ebx+4], edx
  mov  [ebx], eax
  mov  edx, eax

end
               
// ; rint
inline % 4Fh

  mov   eax, [esp]
  mov   ecx, 0
  fld   qword ptr [eax]

  push  ecx                // reserve space on stack
  fstcw word ptr [esp]     // get current control word
  mov   dx, [esp]
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
  fstp  qword ptr [ebx]    // store result
  mov   ecx, 1
  jmp   short labEnd
  
labErr:
  ffree st(1)
  
labEnd:
  mov  edx, ecx

end

// ; addf
inline % 050h

  lea  esi, [ebp+__arg1]
  add  [esi], edx

end

// ; subf
inline % 051h

  lea  esi, [ebp+__arg1]
  sub  [esi], edx

end

// ; nxorf
inline % 52h

  mov  ecx, [ebx]
  xor  [ebp+__arg1], ecx

end

// ; norf
inline % 53h

  mov  ecx, [ebx]
  or   [ebp+__arg1], ecx

end

// ; nandf
inline % 54h

  mov  ecx, [ebx]
  and  [ebp+__arg1], ecx

end

// ; movfipd
inline % 55h

  lea  ecx, [edx*4]
  lea  ebx, [ebp+__arg1]
  sub  ebx, ecx 

end

// ; div
inline %05Bh

  mov  eax, edx
  mov  ecx, __arg1
  xor  edx, edx
  idiv ecx
  mov  edx, eax

end

// ; xwrite
inline % 5Ch

  mov  esi, [esp]
  mov  ecx, __arg1
  lea  edi, [ebx+edx]
  rep  movsb

end

// ; xwrite
inline % 15Ch

  mov  ecx, [esp]
  lea  edi, [ebx+edx]
  mov  eax, [ecx]
  mov  byte ptr [edi], al

end

// ; xwrite
inline % 25Ch

  mov  ecx, [esp]
  lea  edi, [ebx+edx]
  mov  eax, [ecx]
  mov  word ptr [edi], ax

end

// ; xwrite
inline % 35Ch

  mov  ecx, [esp]
  lea  edi, [ebx+edx]
  mov  eax, [ecx]
  mov  dword ptr [edi], eax

end

// ; xwrite
inline % 45Ch

  mov  ecx, [esp]
  lea  edi, [ebx+edx]
  mov  esi, [ecx+4]
  mov  eax, [ecx]
  mov  dword ptr [edi], eax
  mov  dword ptr [edi+4], esi

end

// ; xsave
inline % 5Ah

  lea eax, [ebx+__arg1]
  mov [eax], edx

end

// ; copyto
inline % 5Dh

  lea edi, [ebx+edx]
  mov  ecx, __arg1
  mov esi, [esp]
  rep movsd

end

// ; copyto
inline % 15Dh

  mov esi, [esp]
  lea edi, [ebx+edx]
  mov eax, [esi]
  mov [edi], eax

end

// ; copyto
inline % 25Dh

  mov esi, [esp]
  lea edi, [ebx+edx]
  mov eax, [esi]
  mov [edi], eax
  mov ecx, [esi+4]
  mov [edi+4], ecx

end

// ; copyto
inline % 35Dh

  mov esi, [esp]
  lea edi, [ebx+edx]
  mov eax, [esi]
  mov [edi], eax
  mov ecx, [esi+4]
  mov eax, [esi+8]
  mov [edi+4], ecx
  mov [edi+8], eax

end

// ; copyto
inline % 45Dh

  mov esi, [esp]
  lea edi, [ebx+edx]
  mov eax, [esi]
  mov [edi], eax
  mov ecx, [esi+4]
  mov [edi+4], ecx
  mov eax, [esi+8]
  mov [edi+8], eax
  mov ecx, [esi+12]
  mov [edi+12], ecx

end

// ; nshlf
inline % 5Eh

  mov eax, [ebp+__arg1]
  mov ecx, [ebx]
  shl eax, cl
  mov [ebp+__arg1], eax

end

// ; nshrf
inline % 5Fh

  mov eax, [ebp+__arg1]
  mov ecx, [ebx]
  shr eax, cl
  mov [ebp+__arg1], eax

end

// ; mul
inline %060h

  mov  eax, edx
  mov  ecx, __arg1
  imul ecx
  mov  edx, eax

end

// ; checksi
inline % 061h

  mov    edi, [esp+__arg1]
  xor    edx, edx
  mov    esi, [ebx-elVMTOffset]
labNext:
  mov    eax, 0
  cmp    esi, edi
  mov    esi, [esi - elPackageOffset]
  setz   dl
  cmovnz eax, esi
  and    eax, eax
  jnz    short labNext

end

// ; xredirect 
inline % 062h

  lea  ebx, [ebx + __arg2] // ; NOTE use __arg2 due to current implementation
  push ebx
  push edx 

  mov  esi, [ebx]   // ; get next overload list
  test esi, esi
  jz   labEnd

labNextList:
  xor  edx, edx
  mov  ebx, [esi] // ; message from overload list

labNextOverloadlist:
  shr  ebx, ACTION_ORDER
  mov  edi, rdata : % CORE_MESSAGE_TABLE
  mov  ecx, [esp]
  mov  ebx, [edi + ebx * 8 + 4]
  and  ecx, ARG_MASK
  lea  ebx, [edi + ebx - 4]
  inc  ecx 

labNextParam:
  sub  ecx, 1
  jnz  short labMatching

  pop  eax
  pop  esi
  mov  ecx, edx
  mov  esi, [esi]
  mov  ebx, [esp]
  mov  edx, [esi + ecx * 8]
  jmp  [esi + ecx * 8 + 4]

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

  mov  esi, [esp+4]
  add  edx, 1
  mov  esi, [esi]
  mov  ebx, [esi + edx * 8] // ; message from overload list
  and  ebx, ebx
  jnz  labNextOverloadlist
  add  [esp+4], 4
  mov  esi, [esp+4]
  mov  edx, [esi]
  test edx, edx
  jnz  labNextList

labEnd:
  pop  edx
  pop  ebx

end

// ; xvredirect
inline % 063h

  lea  ebx, [ebx + __arg2] // ; NOTE use __arg2 due to current implementation
  push ebx
  push edx 

  mov  esi, [ebx]   // ; get next overload list
  test esi, esi
  jz   labEnd

labNextList:
  xor  edx, edx
  mov  ebx, eax
  xor  ecx, ecx

labCountParam:
  lea  ebx, [ebx+4]
  cmp  [ebx], -1
  lea  ecx, [ecx+1]
  jnz  short labCountParam

  push ecx
  mov  ebx, [esi] // ; message from overload list

labNextOverloadlist:
  mov  edi, rdata : % CORE_MESSAGE_TABLE
  shr  ebx, ACTION_ORDER
  mov  ecx, [esp]              // ; param count
  mov  ebx, [edi + ebx * 8 + 4]
  lea  ebx, [edi + ebx - 4]

labNextParam:
  // ; check if signature contains the next class ptr
  lea  esi, [ebx + 4]
  cmp [esi], 0
  cmovnz ebx, esi

  sub  ecx, 1
  jnz  short labMatching

  lea  esp, [esp + 4]
  pop  eax
  pop  esi
  mov  ecx, edx
  mov  esi, [esi]
  mov  ebx, [esp]
  mov  edx, [esi + ecx * 8]
  jmp  [esi + ecx * 8 + 4]

labMatching:
  mov  esi, [esp]
  sub  esi, ecx
  mov  edi, [eax + esi * 4]

  //; check nil
  mov   esi, rdata : %VOIDPTR + elObjectOffset
  test  edi, edi
  cmovz edi, esi

  mov  edi, [edi - elVMTOffset]
  mov  esi, [ebx]

labNextBaseClass:
  cmp  esi, edi
  jz   labNextParam
  mov  edi, [edi - elPackageOffset]
  and  edi, edi
  jnz  short labNextBaseClass

  mov  esi, [esp+8]
  add  edx, 1
  mov  esi, [esi]
  mov  ebx, [esi + edx * 8] // ; message from overload list
  and  ebx, ebx
  jnz  labNextOverloadlist

  add  [esp+8], 4
  mov  esi, [esp+8]
  mov  edx, [esi]
  test edx, edx
  jnz  labNextList

labEnd:
  lea  esp, [esp + 4]
  pop  edx
  pop  ebx

end

// ; laddf
inline % 74h

  lea  edi, [ebp+__arg1]
  mov  eax, [ebx+4]
  mov  ecx, [ebx]
  add  [edi], ecx
  adc  [edi+4], eax

end

// ; lsubf
inline % 75h

  lea  edi, [ebp+__arg1]
  mov  esi, [edi]
  mov  ecx, [edi+4]
  sub  esi, [ebx]
  sbb  ecx, [ebx+4]
  mov  [edi], esi
  mov  [edi+4], ecx

end

// ; lmulf
inline % 76h

  lea  edi, [ebp+__arg1]
  mov  esi, ebx        // sour
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
  mov  [edi+4], edx
  pop  ebx

end

// ; ldiv
inline % 77h
               
  lea  edi, [ebp+__arg1]
  mov  esi, edi   // ; esi - DVND, ebx - DVSR

  push ebx

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
  lea  edi, [ebp+__arg1]

  mov  [edi], eax
  mov  [edi+4], edx
  pop  ebx
                                    
end


// ; landf
inline % 78h
  lea edi, [ebp+__arg1]
  mov eax, ebx 
  mov ebx, [edi]
  mov edx, [eax]

  mov ecx, [edi+4]
  mov esi, [eax+4]

  and ebx, edx
  and ecx, esi

  mov [edi], ebx
  mov [edi+4], ecx
  mov ebx, eax
end

// ; lorf
inline % 79h
  lea edi, [ebp+__arg1]
  mov eax, ebx 
  mov ebx, [edi]
  mov edx, [eax]

  mov ecx, [edi+4]
  mov esi, [eax+4]

  or  ebx, edx
  or  ecx, esi

  mov [edi], ebx
  mov [edi+4], ecx
  mov ebx, eax
end

// ; lxorf
inline % 7Ah
  lea edi, [ebp+__arg1]
  mov eax, ebx 
  mov ebx, [edi]
  mov edx, [eax]

  mov ecx, [edi+4]
  mov esi, [eax+4]

  xor ebx, edx
  xor ecx, esi

  mov [edi], ebx
  mov [edi+4], ecx
  mov ebx, eax
end

// ; lshlf
inline % 7Bh

  mov  eax, ebx 
  lea  edi, [ebp+__arg1]
  mov  ecx, edx
  mov  edx, [edi]
  mov  ebx, [edi+4]

  cmp  cl, 40h 
  jae  short lErr
  cmp  cl, 20h
  jae  short LL32
  shld edx, ebx, cl
  shl  ebx, cl
  jmp  short lEnd

LL32:
  mov  ebx, edx
  xor  edx, edx
  sub  cl, 20h
  shl  ebx, cl 
  jmp  short lEnd
  
lErr:
  xor  eax, eax
  jmp  short lEnd2

lEnd:
  mov  [edi], edx
  mov  [edi+4], ebx

lEnd2:
  mov   ebx, eax

end

// ; lshrf
inline % 7Dh

  mov  eax, ebx 
  lea  edi, [ebp+__arg1]
  mov  ecx, edx
  mov  edx, [edi]
  mov  ebx, [edi+4]

  cmp  cl, 64
  jae  short lErr

  cmp  cl, 32
  jae  short LR32
  shrd edx, ebx, cl
  sar  ebx, cl
  jmp  short lEnd

LR32:
  mov  edx, ebx
  xor  ebx, ebx
  sub  cl, 20h
  shr  edx, cl 
  jmp  short lEnd
  
lErr:
  xor  eax, eax
  jmp  short lEnd2

lEnd:
  mov  [edi], edx
  mov  [edi+4], ebx

lEnd2:
  mov   ebx, eax

end

// ; raddnf
inline % 80h

  lea   edi, [ebp+__arg1]
  fild  dword ptr [ebx]
  fadd  qword ptr [edi] 
  fstp  qword ptr [edi]

end

// ; rsubnf
inline % 81h

  lea   edi, [ebp+__arg1]
  fld   qword ptr [edi]
  fisub dword ptr [ebx] 
  fstp  qword ptr [edi]

end

// ; rmulnf
inline % 82h

  lea   edi, [ebp+__arg1]
  fld   qword ptr [edi]
  fimul dword ptr [ebx] 
  fstp  qword ptr [edi]

end

// ; requal
inline % 83h

  mov    edi, [esp]
  fld    qword ptr [edi]
  fld    qword ptr [ebx]
  xor    edx, edx
  fcomip st, st(1)
  sete   dl
  fstp  st(0)

end

// ; rless(lo, ro, tr, fr)
inline % 84h

  mov    edi, [esp]
  fld    qword ptr [edi]
  fld    qword ptr [ebx]
  xor    edx, edx
  fcomip st, st(1)
  setb   dl
  fstp  st(0)

end

// ; raddf
inline % 85h

  lea  edi, [ebp+__arg1]
  fld  qword ptr [ebx]
  fadd qword ptr [edi] 
  fstp qword ptr [edi]

end

// ; rsubf
inline % 86h

  lea  edi, [ebp+__arg1]
  fld  qword ptr [edi]
  fsub qword ptr [ebx] 
  fstp qword ptr [edi]

end

// ; rmulf
inline % 87h

  lea  edi, [ebp+__arg1]
  fld  qword ptr [edi]
  fmul qword ptr [ebx] 
  fstp qword ptr [edi]

end

// ; rdivf
inline % 88h
                                                   
  lea  edi, [ebp+__arg1]
  fld  qword ptr [edi]
  fdiv qword ptr [ebx] 
  fstp qword ptr [edi]

end

// ; rdivnf
inline % 89h
                                                   
  lea   edi, [ebp+__arg1]
  fld   qword ptr [edi]
  fidiv dword ptr [ebx] 
  fstp  qword ptr [edi]

end

// ; rintf

inline % 8Eh

  lea  edi, [ebp+__arg1]
  mov   ecx, 0
  fld   qword ptr [ebx]

  push  ecx                // reserve space on stack
  fstcw word ptr [esp]     // get current control word
  mov   dx, [esp]
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
  mov   edx, 1
  jmp   short labEnd
  
labErr:
  ffree st(1)
  
labEnd:

end

// ; geti
inline % 91h

  mov  ebx, [ebx+__arg1]
  
end

// ; restore
inline % 92h

  add  ebp, __arg1
  
end

// ; peekfi
inline % 94h

  mov  ebx, [ebp+__arg1]

end

// ; peeksi
inline % 95h

  mov  ebx, [esp+__arg1]

end

// ; ifheap - part of the command
inline % 96h

  xor    edx, edx
  mov    eax,[data : %CORE_GC_TABLE + gc_start]
  mov    esi, 1
  mov    ecx,[data : %CORE_GC_TABLE + gc_end]
  cmp    ebx, eax
  cmovl  edx, esi
  cmp    ebx, ecx
  cmovg  edx, esi
  and    edx, edx

end

// ; xseti
inline %97h

  mov  eax, [esp]                   
  mov [ebx + __arg1], eax

end

// ; create
inline % 9Ah

  mov  eax, [esp]
  mov  ecx, page_ceil
  mov  edx, [eax]
  lea  ecx, [ecx + edx*4]
  and  ecx, page_mask 
 
  call code : %GC_ALLOC

  mov   eax, [esp]
  xor   edx, edx
  mov   [ebx-elVMTOffset], __arg1
  mov   ecx, [eax]
  mov   esi, struct_mask
  test  ecx, ecx
  cmovz edx, esi
  shl   ecx, 2
  or    ecx, edx
  mov   [ebx-elSizeOffset], ecx

end

// ; fillr (__arg1 - r)
inline % 09Bh
  mov  esi, [esp]	
  mov  eax, __arg1	
  mov  edi, ebx
  mov  ecx, [esi]
  rep  stosd

end

// ; ajumpvi
inline % 0A1h

  mov  eax, [ebx - elVMTOffset]
  jmp  [eax + __arg1]

end

// ; callvi (ecx - offset to VMT entry)
inline % 0A2h

  mov  eax, [ebx - elVMTOffset]
  call [eax + __arg1]

end

// ; hook label (ecx - offset)
// ; NOTE : hook calling should be the first opcode
inline % 0A6h

  call code : %HOOK

  push [data : %CORE_GC_TABLE + gc_et_current]

  mov  edx, esp 
  push ebp
  push edx
  push ecx

  mov  [data : %CORE_GC_TABLE + gc_et_current], esp
  
end

// ; address label (ecx - offset)
inline % 0A7h

  call code : %HOOK
  mov  edx, ecx

end

// ; calli
inline % 0A8h

  mov  esi, [ebx + __arg1]
  call esi

end

// ; ifcount
// ; - partial opcode
inline % 0AFh

  mov  ecx, 0FFFFFh
  mov  eax, [ebx-elSizeOffset]
  and  eax, ecx
  shr  eax, 2
  cmp  eax, edx

end

// ; movn
inline % 0B1h

  mov  edx, __arg1

end

// ; equalfi
inline % 0B3h

  mov  eax, [ebp+__arg1]
  xor  edx, edx
  cmp  eax, ebx
  setz dl

end

// ; pushai
inline % 0B4h

  push [ebx+__arg1]

end

// ; loadf
inline % 0B5h

  mov  edx, [ebp + __arg1]

end

// ; loadfi
inline % 0B7h

  mov  edx, [ebp + __arg1]

end

// ; dloadsi
inline % 0B8h

  mov  edx, [esp + __arg1]

end

// ; savef
inline % 0B9h

  mov  [ebp + __arg1], edx

end

// ; savesi
inline % 0BBh

  mov  [esp + __arg1], edx

end

// ; savefi
inline % 0BCh

  mov  eax, [ebp + __arg1]
  mov  [eax], edx

end

// ; pushf
inline % 0BDh

  lea  eax, [ebp + __arg1]
  push eax

end

// ; reserve
inline % 0BFh

  sub  esp, __arg1
  push ebp
  push 0
  mov  ebp, esp

end

// ; pushsip
inline % 0BEh

  lea  eax, [esp + __arg1]
  push eax

end

// ; seti
inline %0C0h

  mov  esi, ebx
  mov  eax, [esp]                   
  // calculate write-barrier address
  sub  esi, [data : %CORE_GC_TABLE + gc_start]
  mov  ecx, [data : %CORE_GC_TABLE + gc_header]
  shr  esi, page_size_order
  mov  [ebx + __arg1], eax
  mov  byte ptr [esi + ecx], 1  

end

// ; storesi
inline % 0C3h

  mov  [esp+__arg1], ebx

end

// ; storefi
inline % 0C4h

  mov  [ebp+__arg1], ebx

end

// ; naddf
inline % 0C5h

  mov  ecx, [ebx]
  add  [ebp+__arg1], ecx

end

// ; nmulf
inline % 0C6h

  mov  eax, [ebp+__arg1]
  imul dword ptr [ebx]
  mov  [ebp+__arg1], eax

end

// ; xsetr
inline % 0C7h
            
   mov  [ebx + edx * 4], __arg1

end

// ; nsubf
inline % 0C8h

  mov  ecx, dword ptr [ebx]
  sub  [ebp+__arg1], ecx

end

// ; ndivf
inline % 0C9h

  mov  eax, [ebp+__arg1]
  cdq
  idiv dword ptr [ebx]
  mov  [ebp+__arg1], eax

end

// ; loadi
inline % 0CAh

  mov  edx, [ebx + __arg1]

end

// ; savei
inline % 0CBh

  mov  [ebx + __arg1], edx

end

// ; xor
inline % 0CDh

  xor    edx, __arg1

end

// ; clonef
inline % 0CEh

  mov  ecx, [ebx - elSizeOffset]
  and  ecx, struct_mask_inv
  lea  esi, [ebp+__arg1]
  shr  ecx, 2
  mov  edi, ebx
  rep  movsd

end

// ; xload
inline % 0CFh

  mov  edx, [ebx + __arg1]

end

// ; alloci
inline %0D1h

  // ; generated in jit : sub  esp, __arg1*4
  mov  ecx, __arg1
  xor  eax, eax
  mov  edi, esp
  rep  stos

end

// ; xcreate
inline % 0D2h

  mov   eax, [esp]
  mov   edx, 0FFFFFh
  mov   ecx, [eax-elSizeOffset]
  and   edx, ecx
  mov   ecx, page_ceil
  add   ecx, edx
  and   ecx, page_mask 
 
  call  code : %GC_ALLOC

  mov   eax, [esp]
  mov   [ebx-elVMTOffset], __arg1
  mov   edx, 0FFFFFh
  mov   ecx, [eax-elSizeOffset]
  and   edx, ecx
  mov   [ebx-elSizeOffset], edx
  
end

// ; inc
inline %0D6h

  add  edx, __arg1

end

// ; coalescer
inline % 0D8h

  mov    eax, __arg1
  test   ebx, ebx
  cmovz  ebx, eax

end

// ; vjumprm
inline % 0DBh

  mov  ecx, __arg1
  mov  eax, [ebx - elVMTOffset]
  jmp  [eax + ecx * 8 + 4]

end


// ; xsaveai (__arg1 - index, __arg2 - n)
inline % 0DCh

  mov  eax, __arg2
  mov  [ebx + __arg1], eax

end

// ; copyai (__arg1 - index, __arg2 - n)
inline % 0DDh

  mov  ecx, __arg2	
  lea  esi, [ebx + __arg1]
  mov  edi, [esp]
  rep  movsd

end

inline % 01DDh

  lea  esi, [ebx + __arg1]
  mov  edi, [esp]
  mov  eax, [esi]
  mov  [edi], eax

end

inline % 02DDh

  lea  esi, [ebx + __arg1]
  mov  edi, [esp]
  mov  eax, [esi]
  mov  [edi], eax
  mov  ecx, [esi+4]
  mov  [edi+4], ecx

end

inline % 03DDh

  lea  esi, [ebx + __arg1]
  mov  edi, [esp]
  mov  eax, [esi]
  mov  [edi], eax
  mov  ecx, [esi+4]
  mov  [edi+4], ecx
  mov  eax, [esi+8]
  mov  [edi+8], eax

end

inline % 04DDh

  lea  esi, [ebx + __arg1]
  mov  edi, [esp]
  mov  eax, [esi]
  mov  [edi], eax
  mov  ecx, [esi+4]
  mov  [edi+4], ecx
  mov  eax, [esi+8]
  mov  [edi+8], eax
  mov  ecx, [esi+12]
  mov  [edi+12], ecx

end

// ; move
inline % 0DEh

  lea esi, [ebx+__arg1]
  mov ecx, __arg2
  mov edi, [esp]
  rep movsb

end

// ; move
inline % 01DEh

  lea esi, [ebx+__arg1]
  mov edi, [esp]
  mov eax, [esi]
  mov byte ptr [edi], al

end

inline % 02DEh

  lea esi, [ebx+__arg1]
  mov edi, [esp]
  mov eax, [esi]
  mov word ptr [edi], ax

end

inline % 03DEh

  lea esi, [ebx+__arg1]
  mov edi, [esp]
  mov eax, [esi]
  mov [edi], eax

end

inline % 04DEh

  lea esi, [ebx+__arg1]
  mov edi, [esp]
  mov eax, [esi]
  mov [edi], eax
  mov ecx, [esi+4]
  mov [edi+4], ecx

end

// ; moveto
inline % 0DFh

  lea edi, [ebx+__arg1]
  mov ecx, __arg2
  mov esi, [esp]
  rep movsb

end

inline % 01DFh

  mov esi, [esp]
  lea edi, [ebx+__arg1]
  mov eax, [esi] 
  mov byte ptr [edi], al

end

inline % 02DFh

  mov esi, [esp]
  lea edi, [ebx+__arg1]
  mov eax, [esi] 
  mov word ptr [edi], ax

end

inline % 03DFh

  mov esi, [esp]
  lea edi, [ebx+__arg1]
  mov eax, [esi] 
  mov [edi], eax

end

inline % 04DFh

  mov esi, [esp]
  lea edi, [ebx+__arg1]
  mov eax, [esi] 
  mov [edi], eax
  mov ecx, [esi+4] 
  mov [edi+4], ecx

end

// ; mtredirect (__arg3 - number of parameters, eax - points to the stack arg list)
inline % 0E8h

  mov  esi, __arg1
  push ebx
  xor  edx, edx
  mov  ebx, [esi] // ; message from overload list

labNextOverloadlist:
  shr  ebx, ACTION_ORDER
  mov  edi, rdata : % CORE_MESSAGE_TABLE
  mov  ebx, [edi + ebx * 8 + 4]
  mov  ecx, __arg3
  lea  ebx, [edi + ebx - 4]

labNextParam:
  sub  ecx, 1
  jnz  short labMatching

  mov  esi, __arg1
  pop  ebx
  mov  eax, [esi + edx * 8 + 4]
  mov  ecx, [ebx - elVMTOffset]
  mov  edx, [esi + edx * 8]
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

  mov  esi, __arg1
  add  edx, 1
  mov  ebx, [esi + edx * 8] // ; message from overload list
  and  ebx, ebx
  jnz  labNextOverloadlist

  pop  ebx

end

// ; xmtredirect (__arg3 - number of parameters, eax - points to the stack arg list)
inline % 0E9h

  mov  esi, __arg1
  push ebx
  xor  edx, edx
  mov  ebx, [esi] // ; message from overload list
          	
labNextOverloadlist:
  shr  ebx, ACTION_ORDER
  mov  edi, rdata : % CORE_MESSAGE_TABLE
  mov  ebx, [edi + ebx * 8 + 4]
  mov  ecx, __arg3
  lea  ebx, [edi + ebx - 4]

labNextParam:
  sub  ecx, 1
  jnz  short labMatching

  mov  esi, __arg1
  mov  ecx, edx
  pop  ebx
  mov  edx, [esi + ecx * 8]
  jmp  [esi + ecx * 8 + 4]

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

  mov  esi, __arg1
  add  edx, 1
  mov  ebx, [esi + edx * 8] // ; message from overload list
  and  ebx, ebx
  jnz  labNextOverloadlist

  pop  ebx

end

// ; mtredirect<1>
inline % 1E8h

  mov  ecx, __arg1
  xor  edx, edx
  mov  eax, [eax + 4]
  mov  ecx, [ecx] // ; message from overload list

  //; check nil
  mov   esi, rdata : %VOIDPTR + elObjectOffset
  test  eax, eax
  cmovz eax, esi

  mov  eax, [eax - elVMTOffset]

labNextOverloadlist:
  shr  ecx, ACTION_ORDER
  mov  edi, rdata : % CORE_MESSAGE_TABLE
  mov  ecx, [edi + ecx * 8 + 4]
  lea  ecx, [edi + ecx]

labMatching:
  mov  edi, eax
  mov  esi, [ecx]

labNextBaseClass:
  cmp  esi, edi
  jnz  short labContinue

  mov  esi, __arg1
  mov  eax, [esi + edx * 8 + 4]
  mov  ecx, [ebx - elVMTOffset]
  mov  edx, [esi + edx * 8]
  jmp  [ecx + eax * 8 + 4]

labContinue:
  mov  edi, [edi - elPackageOffset]
  and  edi, edi
  jnz  short labNextBaseClass

  mov  ecx, __arg1
  add  edx, 1
  mov  ecx, [ecx + edx * 8] // ; message from overload list
  and  ecx, ecx
  jnz  labNextOverloadlist

labEnd:

end

// ; xmtredirect<1>
inline % 1E9h

  mov  ecx, __arg1
  mov  eax, [eax + 4]
  xor  edx, edx
  mov  ecx, [ecx] // ; message from overload list

  //; check nil
  mov   esi, rdata : %VOIDPTR + elObjectOffset
  test  eax, eax
  cmovz eax, esi

  mov  eax, [eax - elVMTOffset]

labNextOverloadlist:
  shr  ecx, ACTION_ORDER
  mov  edi, rdata : % CORE_MESSAGE_TABLE
  mov  ecx, [edi + ecx * 8 + 4]
  lea  ecx, [edi + ecx]

labMatching:
  mov  edi, eax
  mov  esi, [ecx]

labNextBaseClass:
  cmp  esi, edi
  jnz  short labContinue

  mov  ecx, edx
  mov  esi, __arg1
  mov  edx, [esi + edx * 8]
  jmp  [esi + ecx * 8 + 4]

labContinue:
  mov  edi, [edi - elPackageOffset]
  and  edi, edi
  jnz  short labNextBaseClass

  mov  ecx, __arg1
  add  edx, 1
  mov  ecx, [ecx + edx * 8] // ; message from overload list
  and  ecx, ecx
  jnz  labNextOverloadlist

labEnd:

end

// ; mtredirect<2> (eax - refer to the stack)
inline % 2E8h 

  mov  ecx, __arg1
  xor  edx, edx
  mov  ecx, [ecx] // ; message from overload list

labNextOverloadlist:
  mov  edi, rdata : % CORE_MESSAGE_TABLE
  shr  ecx, ACTION_ORDER
  mov  ecx, [edi + ecx * 8 + 4]
  lea  ecx, [edi + ecx]

labMatching:
  mov  edi, [eax+4]

  //; check nil
  mov   esi, rdata : %VOIDPTR + elObjectOffset
  test  edi, edi
  cmovz edi, esi

  mov  edi, [edi-elVMTOffset]
  mov  esi, [ecx]

labNextBaseClass:
  cmp  esi, edi
  jnz  labContinue

  mov  edi, [eax+8]

  //; check nil
  mov   esi, rdata : %VOIDPTR + elObjectOffset
  test  edi, edi
  cmovz edi, esi

  mov  edi, [edi-elVMTOffset]
  mov  esi, [ecx + 4]

labNextBaseClass2:
  cmp  esi, edi
  jnz  short labContinue2

  mov  esi, __arg1
  mov  eax, [esi + edx * 8 + 4]
  mov  ecx, [ebx - elVMTOffset]
  mov  edx, [esi + edx * 8]
  jmp  [ecx + eax * 8 + 4]

labContinue2:
  mov  edi, [edi - elPackageOffset]
  and  edi, edi
  jnz  short labNextBaseClass2
  nop
  nop
  jmp short labNext

labContinue:
  mov  edi, [edi - elPackageOffset]
  and  edi, edi
  jnz  short labNextBaseClass

labNext:
  mov  ecx, __arg1
  add  edx, 1
  mov  ecx, [ecx + edx * 8] // ; message from overload list
  and  ecx, ecx
  jnz  labNextOverloadlist

end

// ; xmtredirect<2>  (eax - refer to the stack)
inline % 2E9h

// ecx -> eax
// ebx -> ecx

  mov  ecx, __arg1
  xor  edx, edx
  mov  ecx, [ecx] // ; message from overload list

labNextOverloadlist:
  mov  edi, rdata : % CORE_MESSAGE_TABLE
  shr  ecx, ACTION_ORDER
  mov  ecx, [edi + ecx * 8 + 4]
  lea  ecx, [edi + ecx]

labMatching:
  mov  edi, [eax+4]

  //; check nil
  mov   esi, rdata : %VOIDPTR + elObjectOffset
  test  edi, edi
  cmovz edi, esi

  mov  edi, [edi-elVMTOffset]
  mov  esi, [ecx]

labNextBaseClass:
  cmp  esi, edi
  jnz  labContinue

  mov  edi, [eax+8]

  //; check nil
  mov   esi, rdata : %VOIDPTR + elObjectOffset
  test  edi, edi
  cmovz edi, esi

  mov  edi, [edi-elVMTOffset]
  mov  esi, [ecx + 4]

labNextBaseClass2:
  cmp  esi, edi
  jnz  short labContinue2

  mov  esi, __arg1
  mov  ecx, edx
  mov  edx, [esi + ecx * 8]
  jmp  [esi + ecx * 8 + 4]

labContinue2:
  mov  edi, [edi - elPackageOffset]
  and  edi, edi
  jnz  short labNextBaseClass2
  nop
  nop
  jmp short labNext

labContinue:
  mov  edi, [edi - elPackageOffset]
  and  edi, edi
  jnz  short labNextBaseClass

labNext:
  mov  ecx, __arg1
  add  edx, 1
  mov  ecx, [ecx + edx * 8] // ; message from overload list
  and  ecx, ecx
  jnz  labNextOverloadlist

end

// ; mtredirect<12> (__arg3 - number of parameters, eax - points to the stack arg list)

inline % 0CE8h

  push ebx
  xor  edx, edx
  mov  ebx, eax
  xor  ecx, ecx

labCountParam:
  lea  ebx, [ebx+4]
  cmp  [ebx], -1
  lea  ecx, [ecx+1]
  jnz  short labCountParam

  mov  esi, __arg1
  push ecx
  mov  ebx, [esi] // ; message from overload list

labNextOverloadlist:
  mov  edi, rdata : % CORE_MESSAGE_TABLE
  shr  ebx, ACTION_ORDER
  mov  ecx, [esp]              // ; param count
  mov  ebx, [edi + ebx * 8 + 4]
  lea  ebx, [edi + ebx - 4]

labNextParam:
  // ; check if signature contains the next class ptr
  lea  esi, [ebx + 4]
  cmp [esi], 0
  cmovnz ebx, esi

  sub  ecx, 1
  jnz  short labMatching

  mov  esi, __arg1
  lea  esp, [esp + 4]
  pop  ebx
  mov  eax, [esi + edx * 8 + 4]
  mov  ecx, [ebx - elVMTOffset]
  mov  edx, [esi + edx * 8]
  jmp  [ecx + eax * 8 + 4]

labMatching:
  mov  esi, [esp]
  sub  esi, ecx
  mov  edi, [eax + esi * 4]

  //; check nil
  mov   esi, rdata : %VOIDPTR + elObjectOffset
  test  edi, edi
  cmovz edi, esi

  mov  edi, [edi - elVMTOffset]
  mov  esi, [ebx]

labNextBaseClass:
  cmp  esi, edi
  jz   labNextParam
  mov  edi, [edi - elPackageOffset]
  and  edi, edi
  jnz  short labNextBaseClass

  mov  esi, __arg1
  add  edx, 1
  mov  ebx, [esi + edx * 8] // ; message from overload list
  and  ebx, ebx
  jnz  labNextOverloadlist

  lea  esp, [esp + 4]
  pop  eax

end

// ; xmtredirect<12>

inline % 0CE9h

  push ebx
  xor  edx, edx
  mov  ebx, eax
  xor  ecx, ecx

labCountParam:
  lea  ebx, [ebx+4]
  cmp  [ebx], -1
  lea  ecx, [ecx+1]
  jnz  short labCountParam

  mov  esi, __arg1
  push ecx
  mov  ebx, [esi] // ; message from overload list

labNextOverloadlist:
  mov  edi, rdata : % CORE_MESSAGE_TABLE
  shr  ebx, ACTION_ORDER
  mov  ecx, [esp]              // ; param count
  mov  ebx, [edi + ebx * 8 + 4]
  lea  ebx, [edi + ebx - 4]

labNextParam:
  // ; check if signature contains the next class ptr
  lea  esi, [ebx + 4]
  cmp [esi], 0
  cmovnz ebx, esi

  sub  ecx, 1
  jnz  short labMatching

  mov  esi, __arg1
  mov  ecx, edx
  lea  esp, [esp + 4]
  pop  ebx
  mov  edx, [esi + ecx * 8]
  jmp  [esi + ecx * 8 + 4]

labMatching:
  mov  esi, [esp]
  sub  esi, ecx
  mov  edi, [eax + esi * 4]

  //; check nil
  mov   esi, rdata : %VOIDPTR + elObjectOffset
  test  edi, edi
  cmovz edi, esi

  mov  edi, [edi - elVMTOffset]
  mov  esi, [ebx]

labNextBaseClass:
  cmp  esi, edi
  jz   labNextParam
  mov  edi, [edi - elPackageOffset]
  and  edi, edi
  jnz  short labNextBaseClass

  mov  esi, __arg1
  add  edx, 1
  mov  ebx, [esi + edx * 8] // ; message from overload list
  and  ebx, ebx
  jnz  labNextOverloadlist

  lea  esp, [esp + 4]
  pop  eax

end

// ; readtof (__arg1 - index, __arg2 - n)
inline % 0E0h

  mov  ecx, __arg2	
  lea  edi, [ebp + __arg1]
  lea  esi, [ebx+edx]
  rep  movsd

end

// ; readtof (__arg1 - index, __arg2 - n)
inline % 1E0h

  mov  ecx, [ebx+edx]
  lea  edi, [ebp + __arg1]
  mov  dword ptr [edi], ecx
end

// ; readtof (__arg1 - index, __arg2 - n)
inline % 2E0h

  mov  ecx, [ebx+edx]
  lea  edi, [ebp + __arg1]
  mov  dword ptr [edi], ecx
  mov  eax, [ebx+edx+4]
  mov  dword ptr [edi+4], eax

end

// ; readtof (__arg1 - index, __arg2 - n)
inline % 3E0h

  mov  ecx, [ebx+edx]
  lea  edi, [ebp + __arg1]
  mov  dword ptr [edi], ecx
  mov  eax, [ebx+edx+4]
  mov  dword ptr [edi+4], eax
  mov  ecx, [ebx+edx+8]
  mov  dword ptr [edi+8], ecx

end

// ; readtof (__arg1 - index, __arg2 - n)
inline % 4E0h

  mov  ecx, [ebx+edx]
  lea  edi, [ebp + __arg1]
  mov  dword ptr [edi], ecx
  mov  eax, [ebx+edx+4]
  mov  dword ptr [edi+4], eax
  mov  ecx, [ebx+edx+8]
  mov  dword ptr [edi+8], ecx
  mov  eax, [ebx+edx+12]
  mov  dword ptr [edi+12], eax

end

// ; createn (__arg1 - item size)
inline % 0E1h

  mov  eax, [esp]
  mov  ecx, page_ceil
  mov  eax, [eax]
  mov  ebx, __arg1
  imul ebx
  add  ecx, eax
  and  ecx, page_mask 
 
  call code : %GC_ALLOC

  mov   eax, [esp]
  mov   ecx, struct_mask
  mov   eax, [eax]
  mov   esi, __arg1
  imul  esi
  or    ecx, eax
  mov   [ebx-elSizeOffset], ecx
  
end

// ; createn (__arg1 = 1)
inline % 1E1h

  mov  eax, [esp]
  mov  ecx, page_ceil
  add  ecx, [eax]
  and  ecx, page_mask 
 
  call code : %GC_ALLOC

  mov   eax, [esp]
  mov   ecx, struct_mask
  or    ecx, [eax]
  mov   [ebx-elSizeOffset], ecx
  
end

// ; createn (__arg1 = 2)
inline % 2E1h

  mov  eax, [esp]
  mov  ecx, page_ceil
  mov  eax, [eax]
  shl  eax, 1
  add  ecx, eax
  and  ecx, page_mask 
 
  call code : %GC_ALLOC

  mov   eax, [esp]
  mov   ecx, struct_mask
  mov   eax, [eax]
  shl  eax, 1
  or    ecx, eax
  mov   [ebx-elSizeOffset], ecx
  
end

// ; createn (__arg1 = 4)
inline % 3E1h

  mov  eax, [esp]
  mov  ecx, page_ceil
  mov  eax, [eax]
  shl  eax, 2
  add  ecx, eax
  and  ecx, page_mask 
 
  call code : %GC_ALLOC

  mov   eax, [esp]
  mov   ecx, struct_mask
  mov   eax, [eax]
  shl  eax, 2
  or    ecx, eax
  mov   [ebx-elSizeOffset], ecx
  
end

// ; createn (__arg1 = 8)
inline % 4E1h

  mov  eax, [esp]
  mov  ecx, page_ceil
  mov  eax, [eax]
  shl  eax, 3
  add  ecx, eax
  and  ecx, page_mask 
 
  call code : %GC_ALLOC

  mov   eax, [esp]
  mov   ecx, struct_mask
  mov   eax, [eax]
  shl  eax, 3
  or    ecx, eax
  mov   [ebx-elSizeOffset], ecx
  
end

// ; xsetfi (__arg1 - index, __arg2 - index)
inline % 0E2h

  mov  eax, [ebp + __arg1]
  mov  [ebx + __arg2], eax

end

// ; copytoai (__arg1 - index, __arg2 - n)
inline % 0E3h

  mov  ecx, __arg2	
  lea  edi, [ebx + __arg1]
  mov  esi, [esp]
  rep  movsd

end

inline % 01E3h

  mov  esi, [esp]
  lea  edi, [ebx + __arg1]
  mov  eax, [esi]
  mov  [edi], eax

end

inline % 02E3h

  mov  esi, [esp]
  lea  edi, [ebx + __arg1]
  mov  eax, [esi]
  mov  [edi], eax
  mov  ecx, [esi+4]
  mov  [edi+4], ecx

end

inline % 03E3h

  mov  esi, [esp]
  lea  edi, [ebx + __arg1]
  mov  eax, [esi]
  mov  [edi], eax
  mov  ecx, [esi+4]
  mov  [edi+4], ecx
  mov  eax, [esi+8]
  mov  [edi+8], eax

end

inline % 04E3h

  mov  esi, [esp]
  lea  edi, [ebx + __arg1]
  mov  eax, [esi]
  mov  [edi], eax
  mov  ecx, [esi+4]
  mov  [edi+4], ecx
  mov  eax, [esi+8]
  mov  [edi+8], eax
  mov  ecx, [esi+12]
  mov  [edi+12], ecx

end

// ; copytofi (__arg1 - index, __arg2 - n)
inline % 0E4h

  mov  ecx, __arg2	
  mov  edi, [ebp + __arg1]
  mov  esi, ebx
  rep  movsd

end

inline % 1E4h

  mov  edi, [ebp + __arg1]
  mov  eax, [ebx]
  mov  [edi], eax

end

inline % 2E4h

  mov  edi, [ebp + __arg1]
  mov  eax, [ebx]
  mov  ecx, [ebx+4]
  mov  [edi], eax
  mov  [edi+4], ecx

end

inline % 3E4h

  mov  edi, [ebp + __arg1]
  mov  eax, [ebx]
  mov  ecx, [ebx+4]
  mov  [edi], eax
  mov  esi, [ebx+8]
  mov  [edi+4], ecx
  mov  [edi+8], esi

end

inline % 4E4h

  mov  edi, [ebp + __arg1]
  mov  eax, [ebx]
  mov  ecx, [ebx+4]
  mov  [edi], eax
  mov  esi, [ebx+8]
  mov  [edi+4], ecx
  mov  eax, [ebx+12]
  mov  [edi+8], esi
  mov  [edi+12], eax

end

// ; copytof (__arg1 - index, __arg2 - n)
inline % 0E5h

  mov  ecx, __arg2	
  lea  edi, [ebp + __arg1]
  mov  esi, ebx
  rep  movsd

end

// ; copytof (__arg1 - index, __arg2 - n)
inline % 1E5h

  lea  edi, [ebp + __arg1]
  mov  eax, [ebx]
  mov  [edi], eax

end

// ; copytof (__arg1 - index, __arg2 - n)
inline % 2E5h

  lea  edi, [ebp + __arg1]
  mov  eax, [ebx]
  mov  [edi], eax
  mov  ecx, [ebx+4]
  mov  [edi+4], ecx

end

// ; copytof (__arg1 - index, __arg2 - n)
inline % 3E5h

  lea  edi, [ebp + __arg1]
  mov  eax, [ebx]
  mov  [edi], eax
  mov  ecx, [ebx+4]
  mov  [edi+4], ecx
  mov  eax, [ebx+8]
  mov  [edi+8], eax

end

// ; copytof (__arg1 - index, __arg2 - n)
inline % 4E5h

  lea  edi, [ebp + __arg1]
  mov  eax, [ebx]
  mov  [edi], eax
  mov  ecx, [ebx+4]
  mov  [edi+4], ecx
  mov  eax, [ebx+8]
  mov  [edi+8], eax
  mov  ecx, [ebx+12]
  mov  [edi+12], ecx

end

// ; copyfi (__arg1 - index, __arg2 - n)
inline % 0E6h

  mov  ecx, __arg2	
  mov  esi, [ebp + __arg1]
  mov  edi, ebx
  rep  movsd

end

inline % 01E6h

  mov  esi, [ebp + __arg1]
  mov  eax, [esi]
  mov  [ebx], eax

end

inline % 02E6h

  mov  esi, [ebp + __arg1]
  mov  eax, [esi]
  mov  [ebx], eax
  mov  ecx, [esi+4]
  mov  [ebx+4], ecx

end

inline % 03E6h

  mov  esi, [ebp + __arg1]
  mov  eax, [esi]
  mov  [ebx], eax
  mov  ecx, [esi+4]
  mov  [ebx+4], ecx
  mov  eax, [esi+8]
  mov  [ebx+8], eax

end

inline % 04E6h

  mov  esi, [ebp + __arg1]
  mov  eax, [esi]
  mov  [ebx], eax
  mov  ecx, [esi+4]
  mov  [ebx+4], ecx
  mov  eax, [esi+8]
  mov  [ebx+8], eax
  mov  ecx, [esi+12]
  mov  [ebx+8], ecx

end

// ; copyf (__arg1 - index, __arg2 - n)
inline % 0E7h

  mov  ecx, __arg2	
  lea  esi, [ebp + __arg1]
  mov  edi, ebx
  rep  movsd

end

inline % 01E7h

  lea  esi, [ebp + __arg1]
  mov  eax, [esi]
  mov  [ebx], eax

end

inline % 02E7h

  lea  esi, [ebp + __arg1]
  mov  eax, [esi]
  mov  [ebx], eax
  mov  ecx, [esi+4]
  mov  [ebx+4], ecx

end

inline % 03E7h

  lea  esi, [ebp + __arg1]
  mov  eax, [esi]
  mov  [ebx], eax
  mov  ecx, [esi+4]
  mov  [ebx+4], ecx
  mov  eax, [esi+8]
  mov  [ebx+8], eax

end

inline % 04E7h

  lea  esi, [ebp + __arg1]
  mov  eax, [esi]
  mov  [ebx], eax
  mov  ecx, [esi+4]
  mov  [ebx+4], ecx
  mov  eax, [esi+8]
  mov  [ebx+8], eax
  mov  ecx, [esi+12]
  mov  [ebx+12], ecx

end

// ; xrsavef (__arg1 - index, __arg2 - n)
inline % 0EDh

  push  __arg2
  fild  dword ptr [esp]
  lea   edi, [ebp+__arg1]
  fstp  qword ptr [edi]
  lea   esp, [esp+4]

end

// ; xaddf (__arg1 - index, __arg2 - n)
inline % 0EEh

  add dword ptr [ebp + __arg1], __arg2

end

// ; xsavef (__arg1 - index, __arg2 - n)
inline % 0EFh

  mov dword ptr [ebp + __arg1], __arg2

end

// ; new (__arg1 - size)
inline % 0F0h
	
  mov  ecx, __arg1
  call code : %GC_ALLOC

end

// ; fillri (__arg1 - count)
inline % 0F2h

  mov  edi, ebx
  mov  ecx, __arg1
  rep  stosd

end

// ; xselectr (eax - r1, __arg1 - r2)
inline % 0F3h

  test   ebx, ebx
  mov    ebx, __arg1
  cmovnz ebx, eax

end

// ; vcallrm
inline % 0F4h

  mov  ecx, __arg1
  mov  eax, [ebx - elVMTOffset]
  call [eax + ecx * 8 + 4]
  
end

// ; jumprm
inline % 0F5h

  cmp [ebx], ebx
  jmp __arg1

end

// ; selectr (ebx - r1, __arg1 - r2)
inline % 0F6h

  mov    ecx, __arg1
  test   edx, edx
  cmovnz ebx, ecx

end

// ; allocn (__arg1 - size)
inline % 0F8h
	
  mov  ecx, __arg1
  call code : %GC_ALLOCPERM

end

// ; xsavesi (__arg1 - index, __arg2 - n)
inline % 0F9h

  mov  eax, __arg2
  mov  [esp + __arg1], eax

end

// callrm (edx contains message, __arg1 contains vmtentry)
inline % 0FEh

   call code : __arg1

end

// ; callextr
inline % 0FFh

  call extern __arg1
  mov  edx, eax

end

// ; lcallextr
inline % 1FFh

  call extern __arg1
  mov  edi, edx
  mov  edx, eax

end
