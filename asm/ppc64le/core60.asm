
// ; --- Predefined References  --
define INVOKER              10001h
define GC_ALLOC	            10002h

define CORE_TOC             20001h
define SYSTEM_ENV           20002h
define CORE_GC_TABLE        20003h
define VOID           	    2000Dh
define VOIDPTR              2000Eh

define ACTION_ORDER              9

// ; TOC TABLE OFFSETS
define toc_import            0000h
define toc_rdata             0008h
define toc_mdata             0010h
define toc_code              0018h
define toc_gctable           0020h
define toc_alloc             0028h

// ; --- Object header fields ---
define elSizeOffset          0004h
define elVMTOffset           0010h 
define elObjectOffset        0010h

// ; --- VMT header fields ---
define elVMTSizeOffset       0008h
define elVMTFlagOffset       0018h
define elPackageOffset       0020h

// ; --- GC TABLE OFFSETS ---
define gc_header             0000h
define gc_start              0008h
define gc_yg_current         0010h
define gc_yg_current         0018h
define gc_yg_end             0020h
define gc_mg_start           0038h
define gc_mg_current         0040h
define gc_end                0048h
define gc_mg_wbar            0050h

define struct_mask_inv     7FFFFFh

// ; --- System Core Preloaded Routines --

structure % CORE_TOC

  dq import : 0         // ; address of import section
  dq rdata  : 0         // ; address of rdata section
  dq mdata  : 0         // ; address of rdata section
  dq code   : 0         // ; address of code section
  dq data   : %CORE_GC_TABLE
  dq code   : %GC_ALLOC // ; address of alloc function

end
 
structure %CORE_GC_TABLE

  dq 0 // ; gc_header             : +00h
  dq 0 // ; gc_start              : +08h
  dq 0 // ; gc_yg_start           : +10h
  dq 0 // ; gc_yg_current         : +18h
  dq 0 // ; gc_yg_end             : +20h
  dq 0 // ; gc_shadow             : +28h
  dq 0 // ; gc_shadow_end         : +30h
  dq 0 // ; gc_mg_start           : +38h
  dq 0 // ; gc_mg_current         : +40h
  dq 0 // ; gc_end                : +48h
  dq 0 // ; gc_mg_wbar            : +50h

end
 
// ; NOTE : the table is tailed with GCMGSize,GCYGSize and MaxThread fields
structure %SYSTEM_ENV

  dq data : %CORE_GC_TABLE
  dq code : %INVOKER

end

structure %VOID

  dq 0
  dq 0  // ; a reference to the super class class
  dq 0
  dq 0  
  dq 0

end

structure %VOIDPTR

  dq rdata : %VOID + elPackageOffset
  dq 0
  dq 0

end

// ; --- GC_ALLOC ---
// ; in: r18 - size ; out: r15 - created object
inline % GC_ALLOC

  ld      r19, toc_gctable(r2)
  ld      r17, gc_yg_current(r19) 
  ld      r16, gc_yg_end(r19) 
  add     r18, r18, r17 
  cmp     r18, r16
  bge     labYGCollect
  std     r18, gc_yg_current(r19) 
  addi    r15, r17, elObjectOffset
  blr

labYGCollect:
  xor  r15, r15, r15  // !! temporal stub
  blr

end

// ; ==== Command Set ==

// ; redirect
inline % 03h //; (r15 - object, r14 - message)

  ld      r16, -elVMTOffset(r15)      //; edi
  xor     r17, r17, r17               //; ecx 
  ld      r7, -elVMTSizeOffset(r16)   //; esi
  li      r19, 1

labSplit:
  cmpwi   r7, 0
  beq     labEnd

labStart:
  andi.   r0, r7, 1
  srdi    r7, r7, 1
  iseleq  r21, r19, r17                  //; ecx

  sldi    r22, r7, 4
  add     r22, r22, r16                  //; edx

  ld      r23, 0(r22)
  cmp     r14, r23
  beq     labFound
  addi    r16, r22, 16  
  blt     labSplit
  subf    r7, r21, r7
  b       labSplit
labFound:
  ld      r23, 8(r22) 
  mtctr   r23
  bctr

labEnd:
                               
end

// ; quit
inline %4

  blr

end

// ; movenv
inline %5

  ld      r14, toc_rdata(r2)
  addis   r14, r14, rdata_disp32hi : %SYSTEM_ENV
  addi    r14, r14, rdata_disp32lo : %SYSTEM_ENV

end

// ; load
inline %6

  ld      r14, 0(r15)

end

// ; len
inline %7

  li      r16, struct_mask_inv
  ld      r14, elSizeOffset(r15)
  and     r14, r14, r16
  srdi    r14, r14, 3

end

// ; setr
inline %80h

  ld      r15, toc_rdata(r2)
  addis   r15, r15, __disp32hi_1 
  addi    r15, r15, __disp32lo_1 

end 

// ; setr 0
inline %180h

  li      r15, 0

end 

// ; setdp
inline %81h

  addi    r15, r31, __arg16_1

end 

// ; nlen n
inline %82h

  li      r18, __n16_1
  li      r16, struct_mask_inv
  ld      r14, elSizeOffset(r15)
  and     r14, r14, r16 
  divw    r14, r14, r18  

end

// ; nlen 1
inline %182h

  li      r16, struct_mask_inv
  ld      r14, elSizeOffset(r15)
  and     r14, r14, r16

end

// ; nlen 2
inline %282h

  li      r16, struct_mask_inv
  ld      r14, elSizeOffset(r15)
  and     r14, r14, r16
  srdi    r14, r14, 1

end

// ; nlen 4
inline %382h

  li      r16, struct_mask_inv
  ld      r14, elSizeOffset(r15)
  and     r14, r14, r16
  srdi    r14, r14, 2

end

// ; nlen 8
inline %482h

  li      r16, struct_mask_inv
  ld      r14, elSizeOffset(r15)
  and     r14, r14, r16
  srdi    r14, r14, 3

end

// ; movm
inline %88h

  lis     r14, __arg32hi_1
  addi    r14, r14, __arg32lo_1

end

// ; copy
inline %90h

  li      r16, __n16_1
  mr      r19, r3
  mr      r18, r15

labLoop:
  cmpwi   r16,0
  beq     labEnd
  ld      r17, 0(r19)
  addi    r16, r16, -8
  std     r17, 0(r18)
  addi    r18, r18, 8
  addi    r19, r19, 8
  b       labLoop

labEnd:

end

// ; closen
inline %91h

  addi    r31, r31, __arg16_1  // ; skip unframed stack
  mr      r1, r31              // ; restore stack pointer

  ld      r31, 00h(r1)         // ; restore frame pointer
  ld      r0,  08h(r1)         // ; restore  return address

  mtlr    r0
  addi    r1, r1, 10h          // ; free stack
  
end

// ; closen 0
inline %191h

  mr      r1, r31              // ; restore stack pointer

  ld      r31, 00h(r1)         // ; restore frame pointer
  ld      r0,  08h(r1)         // ; restore  return address

  mtlr    r0
  addi    r1, r1, 10h          // ; free stack
  
end

// ; alloci
inline %92h

  addi    r1, r1,  -__arg16_1   // ; allocate stack
  li      r16, __arg16_1
  li      r17, 0
  mr      r18, r1 

labLoop:
  cmpwi   r16,0
  beq     labEnd
  addi    r16, r16, -8
  std     r17, 0(r18)
  addi    r18, r18, 8
  b       labLoop

labEnd:

end

// ; alloci 0
inline %192h


end

// ; alloci 1
inline %292h

  addi    r1, r1,  -__arg16_1   // ; allocate stack
  li      r17, 0
  std     r17, 0(r1)

end

// ; alloci 2
inline %392h

  addi    r1, r1,  -__arg16_1   // ; allocate stack
  li      r17, 0
  std     r17, 8(r1)
  std     r17, 0(r1)

end

// ; alloci 3
inline %492h

  addi    r1, r1,  -__arg16_1   // ; allocate stack
  li      r17, 0
  std     r17, 0(r1)
  std     r17, 8(r1)
  std     r17, 16(r1)

end

// ; alloci 4
inline %592h

  addi    r1, r1,  -__arg16_1   // ; allocate stack
  li      r17, 0
  std     r17, 0(r1)
  std     r17, 8(r1)
  std     r17, 16(r1)
  std     r17, 24(r1)

end

// ; freei
inline %93h

  addi    r1, r1, __arg16_1     // ; free stack

end

// ; saveddisp
inline %0A0h

  std     r14, __arg16_1(r31)  // ; save frame pointer

end

// ; storefp
inline %0A1h

  std     r15, __arg16_1(r31)  

end

// ; savesi
inline %0A2h

  std     r14, __arg16_1(r1)  

end 

// ; savesi 0
inline %1A2h

  mr      r3, r14

end 

// ; savesi 1
inline %2A2h

  mr      r4, r14

end 

// ; storesi
inline %0A3h

  std     r15, __arg16_1(r1)  

end 

// ; storesi 0
inline %1A3h

  mr      r3, r15

end 

// ; storesi 1
inline %2A3h

  mr      r4, r15

end 

// ; xflushsi i
inline %0A4h

end 

// ; xflushsi 0
inline %1A4h

  std     r3, 0(r1)  

end 

// ; xflushsi 1
inline %2A4h

  std     r4, 8(r1)

end 

// ; peekfi
inline %0A8h

  ld       r15, __arg16_1(r31)

end 

// ; peeksi
inline %0A9h

  ld      r15, __arg16_1(r1)  

end 

// ; peeksi 0
inline %1A9h

  mr      r15, r3

end 

// ; peeksi 1
inline %2A9h

  mr      r15, r4

end 

// ; callr
inline %0B0h

  ld       r12, toc_code(r2)
  addis    r12, r12, __disp32hi_1 
  addi     r12, r12, __disp32lo_1
  mtctr    r12            // ; put code address into ctr
  bctrl                   // ; and call it

end

// ; callvi (ecx - offset to VMT entry)
inline % 0B1h

  ld       r16, -elVMTOffset(r15)     
  ld       r17, __arg16_1(r16)
  mtctr    r17            // ; put code address into ctr
  bctrl                   // ; and call it

end

// ; copydpn
inline %0E0h

  li      r16, __n16_2
  addi    r18, r31, __arg16_1
  mr      r19, r3

labLoop:
  cmpwi   r16,0
  beq     labEnd
  ld      r17, 0(r19)
  addi    r16, r16, -8
  std     r17, 0(r18)
  addi    r18, r18, 8
  addi    r19, r19, 8
  b       labLoop

labEnd:

end

// ; iaddndp
inline %0E1h

  addi    r19, r31, __arg16_1

  lwz      r17, 0(r3)
  lwz      r18, 0(r19)

  add     r17, r17, r18  

  stw     r17, 0(r19)

end

// ; iaddndp
inline %1E1h

  addi    r19, r31, __arg16_1

  lbz     r17, 0(r3)
  lbz     r18, 0(r19)

  add     r17, r17, r18  

  stb     r17, 0(r19)

end

// ; iaddndp
inline %2E1h

  addi    r19, r31, __arg16_1

  lhz     r17, 0(r3)
  lhz     r18, 0(r19)

  add     r17, r17, r18  

  sth     r17, 0(r19)

end

// ; iaddndp
inline %4E1h

  addi    r19, r31, __arg16_1

  ld      r17, 0(r3)
  ld      r18, 0(r19)

  add     r17, r17, r18  

  std     r17, 0(r19)

end

// ; isubndp
inline %0E2h

  addi    r19, r31, __arg16_1

  lwz     r17, 0(r3)
  lwz     r18, 0(r19)

  add     r17, r17, r18  

  stw     r17, 0(r19)

end

// ; isubndp
inline %1E2h

  addi    r19, r31, __arg16_1

  lbz     r17, 0(r3)
  lbz     r18, 0(r19)

  add     r17, r17, r18  

  stb     r17, 0(r19)

end

// ; isubndp
inline %2E2h

  addi    r19, r31, __arg16_1

  lhz     r17, 0(r3)
  lhz     r18, 0(r19)

  add     r17, r17, r18  

  sth     r17, 0(r19)

end

// ; isubndp
inline %4E2h

  addi    r19, r31, __arg16_1

  ld      r17, 0(r3)
  ld      r18, 0(r19)

  add     r17, r17, r18  

  std     r17, 0(r19)

end

// ; imulndp
inline %0E3h

  addi    r19, r31, __arg16_1

  lwz     r17, 0(r3)
  lwz     r18, 0(r19)

  mullw   r17, r17, r18  

  stw     r17, 0(r19)

end

// ; imulndp
inline %1E3h

  addi    r19, r31, __arg16_1

  lbz     r17, 0(r3)
  lbz     r18, 0(r19)

  mullw   r17, r17, r18  

  stb     r17, 0(r19)

end

// ; imulndp
inline %2E3h

  addi    r19, r31, __arg16_1

  lhz     r17, 0(r3)
  lhz     r18, 0(r19)

  mullw   r17, r17, r18  

  sth     r17, 0(r19)

end

// ; imulndp
inline %4E3h

  addi    r19, r31, __arg16_1

  ld      r17, 0(r3)
  ld      r18, 0(r19)

  mulld   r17, r17, r18  

  std     r17, 0(r19)

end

// ; idivndp
inline %0E4h

  addi    r19, r31, __arg16_1

  lwz     r17, 0(r3)
  lwz     r18, 0(r19)

  divw    r17, r17, r18  

  stw     r17, 0(r19)

end

// ; idivndp
inline %1E4h

  addi    r19, r31, __arg16_1

  lbz     r17, 0(r3)
  lbz     r18, 0(r19)

  divw    r17, r17, r18  

  stb     r17, 0(r19)

end

// ; idivndp
inline %2E4h

  addi    r19, r31, __arg16_1

  lhz     r17, 0(r3)
  lhz     r18, 0(r19)

  divw    r17, r17, r18  

  sth     r17, 0(r19)

end

// ; idivndp
inline %4E4h

  addi    r19, r31, __arg16_1

  ld      r17, 0(r3)
  ld      r18, 0(r19)

  divd    r17, r17, r18  

  std     r17, 0(r19)

end

// ; openin
inline %0F0h

  mflr    r0
  std     r31, -10h(r1)  // ; save frame pointer
  std     r0,  -08h(r1)  // ; save return address

  addi    r1, r1, -16    // ; allocate raw stack
  mr      r31, r1        // ; set frame pointer

  addi    r1, r1, -__n16_2 // ; allocate raw stack

  li      r16, 0
  std     r31, -08h(r1)  // ; save frame pointer
  std     r16, -10h(r1)  // ; save dummy
  addi    r1, r1, -10h   // ; allocate stack
  mr      r31, r1        // ; set frame pointer

  addi    r1, r1,  -__arg16_1   // ; allocate stack
  li      r16, __arg16_1
  li      r17, 0
  mr      r18, r1 

labLoop:
  cmpwi   r16,0
  beq     labEnd
  addi    r16, r16, -8
  std     r17, 0(r18)
  addi    r18, r18, 8
  b       labLoop

labEnd:

end 

// ; openin 0, 0
inline %1F0h

  mflr    r0
  std     r31, -10h(r1)  // ; save frame pointer
  std     r0,  -08h(r1)  // ; save return address

  addi    r1, r1, -10h   // ; allocate stack
  mr      r31, r1        // ; set frame pointer

end 

// ; openin 1, 0
inline %2F0h

  mflr    r0
  std     r31, -10h(r1)  // ; save frame pointer
  li      r16, 0
  std     r0,  -08h(r1)  // ; save return address
  addi    r31, r1, -10h  // ; set frame pointer
  std     r16, -18h(r1)  // ; save return address
  std     r16, -20h(r1)  // ; save return address

  addi    r1, r1, -20h   // ; allocate stack

end 

// ; openin 2, 0
inline %3F0h

  mflr    r0
  std     r31, -10h(r1)  // ; save frame pointer
  li      r16, 0
  std     r0,  -08h(r1)  // ; save return address
  addi    r31, r1, -10h  // ; set frame pointer
  std     r16, -18h(r1)  // ; save return address
  std     r16, -20h(r1)  // ; save return address

  addi    r1, r1, -20h   // ; allocate stack

end 

// ; openin 3, 0
inline %4F0h

  mflr    r0
  std     r31, -10h(r1)  // ; save frame pointer
  li      r16, 0
  std     r0,  -08h(r1)  // ; save return address
  addi    r31, r1, -10h  // ; set frame pointer
  std     r16, -18h(r1)  // ; save return address
  std     r16, -20h(r1)  // ; save return address
  std     r16, -28h(r1)  // ; save return address
  std     r16, -30h(r1)  // ; save return address

  addi    r1, r1, -30h   // ; allocate stack

end 

// ; openin 0, n
inline %5F0h

  mflr    r0
  std     r31, -10h(r1)  // ; save frame pointer
  std     r0,  -08h(r1)  // ; save return address

  mr      r31, r1        // ; set frame pointer
  addi    r1, r1, -__n16_2 // ; allocate raw stack

  li      r16, 0
  std     r31, -10h(r1)  // ; save frame pointer
  std     r0,  -08h(r16) // ; save dummy

  addi    r1, r1, -10h    // ; allocate stack

end 

// ; openin i, 0
inline %6F0h

  mflr    r0
  std     r31, -10h(r1)  // ; save frame pointer
  std     r0,  -08h(r1)  // ; save return address

  addi    r1, r1, -16    // ; allocate raw stack
  mr      r31, r1        // ; set frame pointer

  addi    r1, r1,  -__arg16_1   // ; allocate stack
  li      r16, __arg16_1
  li      r17, 0
  mr      r18, r1 

labLoop:
  cmpwi   r16,0
  beq     labEnd
  addi    r16, r16, -8
  std     r17, 0(r18)
  addi    r18, r18, 8
  b       labLoop

labEnd:

end 

// ; xstoresir
inline %0F1h

  ld      r16, toc_rdata(r2)
  addis   r16, r16, __disp32hi_2
  addi    r16, r16, __disp32lo_2

  std   r16, __arg16_1(r1)

end

// ; xstoresir :0, ...
inline %1F1h

  ld      r16, toc_rdata(r2)
  addis   r16, r16, __disp32hi_2
  addi    r16, r16, __disp32lo_2

  mr    r3, r16

end

// ; xstoresir :1, ...
inline %2F1h

  ld      r16, toc_rdata(r2)
  addis   r16, r16, __disp32hi_2
  addi    r16, r16, __disp32lo_2

  mr    r4, r16

end

// ; openheaderin
inline %0F2h

  // ; loading TOC pointer
  lis     r2, rdata32_hi : %CORE_TOC
  addi    r2, r2, rdata32_lo : %CORE_TOC

  mflr    r0
  std     r31, -10h(r1)  // ; save frame pointer
  std     r0,  -08h(r1)  // ; save return address

  addi    r1, r1, -16    // ; allocate raw stack
  mr      r31, r1        // ; set frame pointer

  addi    r1, r1, -__n16_2 // ; allocate raw stack

  li      r16, 0
  std     r31, -08h(r1)  // ; save frame pointer
  std     r16, -10h(r1)  // ; save dummy
  addi    r1, r1, -10h   // ; allocate stack
  mr      r31, r1        // ; set frame pointer

  addi    r1, r1,  -__arg16_1   // ; allocate stack
  li      r16, __arg16_1
  li      r17, 0
  mr      r18, r1 

labLoop:
  cmpwi   r16,0
  beq     labEnd
  addi    r16, r16, -8
  std     r17, 0(r18)
  addi    r18, r18, 8
  b       labLoop

labEnd:

end 

// ; openheaderin 0, 0
inline %1F2h

  // ; loading TOC pointer
  lis   r2, rdata32_hi : %CORE_TOC
  addi r2, r2, rdata32_lo : %CORE_TOC

  addi    r1, r1, -10h   // ; allocate stack
  mflr    r0
  std     r31, -10h(r1)  // ; save frame pointer
  std     r0,  -08h(r1)  // ; save return address

  mr      r31, r1        // ; set frame pointer

end 

// ; openheaderin 1, 0
inline %2F2h

  // ; loading TOC pointer
  lis   r2, rdata32_hi : %CORE_TOC
  addi r2, r2, rdata32_lo : %CORE_TOC

  addi    r1, r1, -20h   // ; allocate stack
  mflr    r0
  std     r31, -10h(r1)  // ; save frame pointer
  li      r16, 0
  addi    r31, r1, -10h  // ; set frame pointer
  std     r0,  -08h(r1)  // ; save return address
  std     r16, -18h(r1)  // ; save return address
  std     r16, -20h(r1)  // ; save return address

end 

// ; openheaderin 2, 0
inline %3F2h

  // ; loading TOC pointer
  lis   r2, rdata32_hi : %CORE_TOC
  addi r2, r2, rdata32_lo : %CORE_TOC

  addi    r1, r1, -20h   // ; allocate stack
  mflr    r0
  std     r31, -10h(r1)  // ; save frame pointer
  li      r16, 0
  addi    r31, r1, -10h  // ; set frame pointer
  std     r0,  -08h(r1)  // ; save return address
  std     r16, -18h(r1)  // ; save return address
  std     r16, -20h(r1)  // ; save return address

end 

// ; openheaderin 3, 0
inline %4F2h

  // ; loading TOC pointer
  lis   r2, rdata32_hi : %CORE_TOC
  addi r2, r2, rdata32_lo : %CORE_TOC

  addi    r1, r1, -30h   // ; allocate stack
  mflr    r0
  std     r31, -10h(r1)  // ; save frame pointer
  li      r16, 0
  addi    r31, r1, -10h  // ; set frame pointer
  std     r0,  -08h(r1)  // ; save return address
  std     r16, -18h(r1)  // ; save return address
  std     r16, -20h(r1)  // ; save return address
  std     r16, -28h(r1)  // ; save return address
  std     r16, -30h(r1)  // ; save return address

end 

// ; openheaderin 0, n
inline %5F2h

  // ; loading TOC pointer
  lis   r2, rdata32_hi : %CORE_TOC
  addi r2, r2, rdata32_lo : %CORE_TOC

  addi    r1, r1, -10h    // ; allocate stack
  mflr    r0
  std     r31, -10h(r1)  // ; save frame pointer
  std     r0,  -08h(r1)  // ; save return address

  mr      r31, r1        // ; set frame pointer
  addi    r1, r1, -__n16_2 // ; allocate raw stack

  li      r16, 0
  std     r31, -10h(r1)  // ; save frame pointer
  std     r0,  -08h(r16) // ; save dummy

end 

// ; openheaderin i, 0
inline %6F2h

  // ; loading TOC pointer
  lis     r2, rdata32_hi : %CORE_TOC
  addi    r2, r2, rdata32_lo : %CORE_TOC

  mflr    r0
  li      r16, 0
  std     r31, -08h(r1)  // ; save frame pointer
  std     r16, -10h(r1)  // ; save dummy
  addi    r1, r1, -10h   // ; allocate stack
  mr      r31, r1        // ; set frame pointer

  addi    r1, r1,  -__arg16_1   // ; allocate stack
  li      r16, __arg16_1
  li      r17, 0
  mr      r18, r1 

labLoop:
  cmpwi   r16,0
  beq     labEnd
  addi    r16, r16, -8
  std     r17, 0(r18)
  addi    r18, r18, 8
  b       labLoop

labEnd:

end 

// ; movsifi
inline %0F3h

  ld       r16, __arg16_2(r31)
  std      r16, __arg16_1(r1)

end

// ; movsifi sp:0, fp:i2
inline %1F3h

  ld       r3, __arg16_2(r31)

end

// ; movsifi sp:1, fp:i2
inline %2F3h

  ld       r4, __arg16_2(r31)

end

// ; newir i, r
inline %0F4h

  li      r18, __arg16_1
  ld      r12, toc_alloc(r2)
  mtctr   r12            
  bctrl                   

  li      r18, __n16_1
  ld      r17, toc_rdata(r2)
  addis   r17, r17, __disp32hi_2 
  addi    r17, r17, __disp32lo_2
  std     r18, -elSizeOffset(r15)
  std     r17, -elVMTOffset(r15)

end

// ; newnr n, r
inline %0F5h

  li      r18, __arg16_1
  ld      r12, toc_alloc(r2)
  mtctr   r12            
  bctrl                   

  li      r18, __n16_1
  addis   r18, r18, __n16hi_1

  ld      r17, toc_rdata(r2)
  addis   r17, r17, __disp32hi_2 
  addi    r17, r17, __disp32lo_2
  std     r18, -elSizeOffset(r15)
  std     r17, -elVMTOffset(r15)

end

// ; xmovsisi
inline %0F6h

  ld       r16, __arg16_2(r1)
  std      r16, __arg16_1(r1)

end

// ; xmovsisi 1, n
inline %1F6h

  ld       r3, __arg16_2(r1)

end

// ; xmovsisi n, 1
inline %2F6h

  std      r3, __arg16_1(r1)

end

// ; xmovsisi 2, n
inline %3F6h

  ld       r4, __arg16_2(r1)

end

// ; xmovsisi n, 1
inline %4F6h

  std      r4, __arg16_1(r1)

end


// ; xmovsisi 1, 2
inline %5F6h

  mr       r3, r4

end

// ; xmovsisi 2, 1
inline %6F6h

  mr       r4, r3

end

// ; xstorefir
inline %0F9h

  ld      r16, toc_rdata(r2)
  addis   r16, r16, __disp32hi_2
  addi    r16, r16, __disp32lo_2
  std     r16, __arg16_1(r1)

end

// ; dispatchmr
// ; NOTE : __arg32_1 - message; __n_1 - arg count; __ptr32_2 - list, __n_2 - argument list offset
inline % 0FBh

//;  mov  [rsp+8], r10                      // ; saving arg0
  std     r3, 8(r1)
//;  lea  rax, [rsp + __n_2]
  addi    r17, r1, __n16_2
//;  mov  [rsp+16], r11                     // ; saving arg0
  std     r4, 16(r1)

//;  mov  rsi, __ptr64_2
  ld      r21, toc_rdata(r2)
  addis   r21, r21, __disp32hi_2 
  addi    r21, r21, __disp32lo_2 

//;  xor  edx, edx
  li      r25, 0
//;  mov  rbx, [rsi] // ; message from overload list
  ld      r22, 0(r21)                           //;!!

labNextOverloadlist:
//;  mov  r9, mdata : %0
  ld      r24, toc_mdata(r2)

//;  shr  ebx, ACTION_ORDER
  srdi    r22, r22, ACTION_ORDER
//;  lea  r13, [rbx*8]
  sldi    r23, r22, 4

//;  mov  r13, [r9 + r13 * 2 + 8]
  add     r23, r23, r24
  ld      r23, 8(r23)
//;  mov  ecx, __n_1
  li      r16, __n16_1
//;  lea  rbx, [r13 - 8]
  addi    r22, r23, -8

labNextParam:
//;  sub  ecx, 1
  addi    r16, r16, -1
//;  jnz  short labMatching
  cmpwi   r16,0
  bne     labMatching

//;  mov  r9, __ptr64_2  - r21

//;  mov  r13, [r9 + rdx * 16 + 8] 
  sldi    r23, r25, 4  
  add     r25, r21, r23
  ld      r23, 8(r25)

//;  mov  rcx, [rbx - elVMTOffset]
  ld      r16, -elVMTOffset(r15)
//;  lea  rax, [r13 * 16]
  sldi    r17, r23, 4

//;  mov  rdx, [r9 + r13 * 2]        // c02
  sldi    r23, r23, 1
  add     r14, r21, r23
  ld      r14, 0(r14)                
//;  jmp  [rcx + rax + 8]       // rax - 0
  add     r20, r16, r17
  ld      r0, 8(r20)                
  mtctr   r0
  bctr

labMatching:
//;  mov  rdi, [rax + rcx * 8]
  sldi    r19, r16, 3
  add     r18, r19, r17
  ld      r18, 0(r18)

  //; check nil
//;  mov   rsi, rdata : %VOIDPTR + elObjectOffset
  ld      r20, toc_rdata(r2)
  addis   r20, r20, rdata_disp32hi : %VOIDPTR
  addi    r20, r20, rdata_disp32lo : %VOIDPTR
//;  test  rdi, rdi                                              
  cmpwi   r18,0

//;  cmovz rdi, rsi
  iseleq  r18, r20, r18

//;  mov  rdi, [rdi - elVMTOffset]
  ld      r18, -elVMTOffset(r18)
//;  mov  rsi, [rbx + rcx * 8]
  add     r20,  r22, r19
  ld      r20, 0(r20)

labNextBaseClass:
//;  cmp  rsi, rdi
  cmp     r20, r18
//;  jz   labNextParam
  beq     labNextParam 
//;  mov  rdi, [rdi - elPackageOffset]
  ld      r18, -elPackageOffset(r18)
//;  and  rdi, rdi
  cmpwi   r18, 0
//;  jnz  short labNextBaseClass
  bne     labNextBaseClass

//;  add  rdx, 1
  addi    r25, r25, 1
//;  mov  r13, __ptr32_2
  mr      r23, r21

//;  lea  r9, [rdx * 8]
  sldi    r24, r25, 4

//;  mov  rbx, [r13 + r9 * 2] // ; message from overload list
  add     r22, r24, r23
  ld      r22, 0(r22)

//;  and  rbx, rbx
  cmpwi   r22, 0
//;  jnz  labNextOverloadlist
  bne     labNextOverloadlist

end

// ; vcallmr
inline % 0FCh

  ld       r16, -elVMTOffset(r15)     
  ld       r17, __arg16_1(r16)
  mtctr    r17            // ; put code address into ctr
  bctrl                   // ; and call it

end

// ; callmr
inline %0FDh

  ld       r12, toc_code(r2)
  addis    r12, r12, __disp32hi_2 
  addi     r12, r12, __disp32lo_2
  mtctr    r12            // ; put code address into ctr
  bctrl                   // ; and call it

end

// ; callext
inline %0FEh

  // ; Put the parameters in r3 through r10,
  // ; and additional parameters go on the stack
  // ; after the home space (not shown here).

  ld      r5, 16(r1)

  ld      r12, toc_import(r2)
  addis   r12, r12, __disp32hi_1 
  ld      r12,__disp32lo_1(r12)

//   ; ld      r11, 00(r2)    // ; r11 points to import address table entry
//   ; ld      r11, 00(r11)   // ; r11 = point address table entry
//   ; ld      r12, 00(r11)   // ; r12 = address of code
//   ; ld      r2,  08h(r11)  // ; load table of contents for destination
  mtctr   r12            // ; put code address into ctr
  bctrl                  // ; and call it
  // lwz      r2, n(r1)     // ; restore our table of contents

end
