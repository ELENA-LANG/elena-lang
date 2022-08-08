// ; --- Predefined References  --
define INVOKER               10001h
define GC_ALLOC	             10002h
define VEH_HANDLER           10003h

define CORE_TOC              20001h
define SYSTEM_ENV            20002h
define CORE_GC_TABLE         20003h
define CORE_ET_TABLE         2000Bh
define VOID           	     2000Dh
define VOIDPTR               2000Eh

define ACTION_ORDER              9

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

define et_current            0008h

define es_prev_struct        0000h
define es_catch_addr         0008h
define es_catch_level        0010h
define es_catch_frame        0018h

// ; --- Page Size ----
define page_ceil               2Fh
define page_mask        0FFFFFFE0h
define struct_mask_inv     7FFFFFh

// ; --- System Core Preloaded Routines --

structure % CORE_TOC

  dq 0         // ; address of import section

end
 
structure % CORE_ET_TABLE

  dq 0 // ; critical_handler       ; +x00   - pointer to ELENA critical exception handler
  dq 0 // ; et_current             ; +x08   - pointer to the current exception struct

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

  dq 0
  dq data : %CORE_GC_TABLE
  dq data : %CORE_EH_TABLE
  dq code : %INVOKER
  dq code : %VEH_HANDLER
  // ; dd GCMGSize
  // ; dd GCYGSize

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
// ; in: x11 - size ; out: x10 - created object
inline % GC_ALLOC

  movz    x12,  data_ptr32lo : %CORE_GC_TABLE
  movk    x12,  data_ptr32hi : %CORE_GC_TABLE, lsl #16
  add     x13, x12, gc_yg_current
  ldr     x15, [x13]
  add     x14, x12, gc_yg_end
  ldr     x14, [x14]
  add     x11, x11, x15
  cmp     x11, x14
  bge     labYGCollect
  str     x11, [x13]
  add     x10, x15, elObjectOffset
  ret     x30

labYGCollect:
  mov     x10, #0                 //; ecx
  ret     x30

end
 
// ; ==== Command Set ==

// ; redirect
inline % 03h //; (r15 - object, r14 - message)

  sub     x14, x10, elVMTOffset              
  ldr     x11, [x14]              //; edi
  mov     x12, #0                 //; ecx
  sub     x15, x11, elVMTSizeOffset
  ldr     x13, [x15]              //; esi

labSplit:
  cmp     x13, #0 
  beq     labEnd

labStart:
  tst     x13, #1
  lsr     x13, x13, #1
  cset    x12, eq

  lsl     x14, x13, #4
  add     x14, x14, x11 

  ldr     x15, [x14]         //; edx
  cmp     x9, x15
  beq     labFound
  add     x14, x14, #16
  ble     labSplit
  mov     x11, x14
  sub     x13, x13, x12
  b       labSplit

labFound:
  add     x14, x14, #8 
  ldr     x15, [x14] 
  br      x15

labEnd:
                               
end

// ; quit
inline %4

  ret x30

end

// ; movenv
inline %5

  movz    x9,  rdata_ptr32lo : %SYSTEM_ENV
  movk    x9,  rdata_ptr32hi : %SYSTEM_ENV, lsl #16

end

// ; load
inline %6

  ldrsw   x9, [x10]

end

// ; len
inline %7

  sub     x11, x10, elSizeOffset
  ldr     x9, [x11]
  and     x9, x9, struct_mask_inv
  lsr     x9, x9, #3

end

// ; class
inline %8

  sub     x14, x10, elVMTOffset              
  ldr     x10, [x14]              //; edi

end

// ; save
inline %9

  str    x9, [x10]

end

// ; throw
inline %0Ah

  movz    x14,  data_ptr32lo : %CORE_ET_TABLE
  movk    x14,  data_ptr32hi : %CORE_ET_TABLE, lsl #16

  ldr     x14, [x14, et_current]!
  ldr     x17, [x14, es_catch_addr]!

  br      x17

end

// ; setr
inline %80h

  movz    x10,  __ptr32lo_1
  movk    x10,  __ptr32hi_1, lsl #16

end 

// ; setr 0
inline %180h

  mov     x10, #0

end 

// ; setdp
inline %81h

  add     x10, x29, __arg12_1

end 

// ; nlen n
inline %82h

  mov     x18, __n16_1

  sub     x11, x10, elSizeOffset
  ldr     w9, [x11]
  and     x9, x9, struct_mask_inv
  sdiv    x17, x17, x18  

end

// ; nlen 1
inline %182h

  sub     x11, x10, elSizeOffset
  ldr     w9, [x11]
  and     x9, x9, struct_mask_inv

end

// ; nlen 2
inline %282h

  sub     x11, x10, elSizeOffset
  ldr     w9, [x11]
  and     x9, x9, struct_mask_inv
  lsr     x9, x9, #1

end

// ; nlen 4
inline %382h

  sub     x11, x10, elSizeOffset
  ldr     w9, [x11]
  and     x9, x9, struct_mask_inv
  lsr     x9, x9, #2

end

// ; nlen 8
inline %482h

  sub     x11, x10, elSizeOffset
  ldr     w9, [x11]
  and     x9, x9, struct_mask_inv
  lsr     x9, x9, #3

end

// ; xassigni
inline %83h

  add     x11, x10, __arg12_1
  str     x0, [x11]

end

// ; peekr
inline %84h

  movz    x11,  __ptr32lo_1
  movk    x11,  __ptr32hi_1, lsl #16
  ldr     x10, [x11]

end 

// ; storer
inline %85h

  movz    x11,  __ptr32lo_1
  movk    x11,  __ptr32hi_1, lsl #16
  str     x10, [x11]

end 

// ; movm
inline %88h

  movz    x9,  __arg32lo_1
  movk    x9,  __arg32hi_1, lsl #16

end

// ; copy
inline %90h

  mov     x11, __n16_1
  mov     x12, x0
  mov     x13, x10

labLoop:
  cmp     x11, 0
  beq     labEnd
  sub     x11, x11, 1
  ldrb    w14, [x12], #1
  strb    w14, [x13], #1
  b       labLoop

labEnd:

end

// ; closen
inline %91h

  add     x29, x29, __n12_1
  mov     sp, x29
  ldp     x29, x30, [sp], #16
  
end

// ; closen 0
inline %191h

  mov     sp, x29
  ldp     x29, x30, [sp], #16
  
end

// ; alloci
// ; NOTE : __arg1 should be aligned to 16
inline %92h

  sub     sp, sp,  __arg12_1   // ; allocate stack
  mov     x11, __arg12_1
  mov     x12, 0
  mov     x13, sp

labLoop:
  cmp     x11, 0
  beq     labEnd
  sub     x11, x11, 8
  str     x12, [x13], #8
  b       labLoop

labEnd:

end

// ; alloci 0
inline %192h

end

// ; alloci 1
inline %292h

  mov     x12, #0
  str     x12, [sp, #-8]!

end

// ; alloci 2
inline %392h

  mov     x12, #0
  stp     x12, x12, [sp, #-16]!

end

// ; alloci 3
inline %492h

  mov     x12, #0
  stp     x12, x12, [sp, #-16]!
  str     x12, [sp, #-8]!

end

// ; alloci 4
inline %592h

  stp     x12,x12, [sp, #-16]!
  stp     x12,x12, [sp, #-16]!

end

// ; freei
// ; NOTE : __arg1 should be aligned to 16
inline %93h

  add     sp, sp, __arg12_1

end

// ; andn
inline %94h

//;  and     x9, x9, __n13_1
  mov     x19, __n16_1   // ; temporally
  and     x9, x9, x19

end

// ; readn
inline %95h

  mov     x11, __n16_1
  mov     x13, x10
  mul     x14, x9, x11
  add     x12, x0, x14

labLoop:
  cmp     x11, 0
  beq     labEnd
  sub     x11, x11, 1
  ldrb    w14, [x12], #1
  strb    w14, [x13], #1
  b       labLoop

labEnd:

end

// ; writen
inline %96h

  mov     x11, __n16_1
  mov     x13, x10
  mul     x14, x9, x11
  add     x12, x0, x14

labLoop:
  cmp     x11, 0
  beq     labEnd
  sub     x11, x11, 1
  ldrb    w14, [x13], #1
  strb    w14, [x12], #1
  b       labLoop

labEnd:

end

// ; saveddisp
inline %0A0h

  add     x11, x29, __arg12_1
  str     x9, [x11]        

end

// ; storefi
// ; NOTE : it is presumed that arg1 < 0 (it is inverted in jitcompiler)
inline %0A1h

  sub     x11, x29, -__arg12_1
  str     x10, [x11]

end

// ; storefi
// ; NOTE : it is presumed that arg1 > 0 (it is inverted in jitcompiler)
inline %05A1h

  add     x11, x29, __arg12_1
  str     x10, [x11]

end

// ; savesi
inline %0A2h

  add     x11, sp, __arg12_1
  str     x9, [x11]

end 

// ; savesi 0
inline %1A2h

  mov     x0, x9

end 

// ; savesi 1
inline %2A2h

  mov     x1, x9

end 

// ; storesi
inline %0A3h

  add     x11, sp, __arg12_1
  str     x10, [x11]

end 

// ; storesi 0
inline %1A3h

  mov     x0, x10

end 

// ; storesi 1
inline %2A3h

  mov     x1, x10

end 

// ; xflushsi i
inline %0A4h

end 

// ; xflushsi 0
inline %1A4h

  str     x0, [sp]

end 

// ; xflushsi 1
inline %2A4h

  add     x11, sp, 8
  str     x1, [x11]

end 

// ; geti
inline %0A5h

  add     x11, x10, __arg12_1
  ldr     x10, [x11]

end

// ; geti
// ; NOTE : it is presumed that arg1 < 0 (it is inverted in jitcompiler)
inline %5A5h

  sub     x11, x10, -__arg12_1
  ldr     x10, [x11]

end

// ; peekfi
// ; NOTE : it is presumed that arg1 < 0 (it is inverted in jitcompiler)
inline %0A8h

  sub     x11, x29, -__arg12_1
  ldr     x10, [x11]

end 

// ; peekfi
// ; NOTE : it is presumed that arg1 > 0 (it is inverted in jitcompiler)
inline %5A8h

  add     x11, x29, __arg12_1
  ldr     x10, [x11]

end 

// ; peeksi
inline %0A9h

  add     x11, sp, __arg12_1
  ldr     x10, [x11]

end 

// ; peeksi 0
inline %1A9h

  mov     x10, x0

end 

// ; peeksi 1
inline %2A9h

  mov     x10, x1

end 

// ; callr
inline %0B0h

  movz    x17,  __ptr32lo_1
  movk    x17,  __ptr32hi_1, lsl #16
  blr     x17

end

// ; callvi
inline %0B1h

  sub     x14, x10, elVMTOffset              
  ldr     x17, [x14]
  add     x17, x17, __arg12_1
  ldr     x17, [x17]
  blr     x17

end

// ; cmpr
inline %0C0h

  movz    x11,  __ptr32lo_1
  movk    x11,  __ptr32hi_1, lsl #16
  cmp     x10, x11

end 

// ; cmpr
inline %1C0h

  mov     x11, #0
  cmp     x10, x11

end 

// ; icmpn 4
inline %0C2h

  ldrsw   x17, [x0]
  ldrsw   x18, [x10]

  cmp     x17, x18

end

// ; icmpn 1
inline %1C2h

  ldrsb   x17, [x0]
  ldrsb   x18, [x10]

  cmp     x17, x18

end

// ; icmpn 2
inline %2C2h

  ldrsh   x17, [x0]
  ldrsh   x18, [x10]

  cmp     x17, x18

end

// ; icmpdpn 8
inline %4EFh

  ldr     x17, [x0]
  ldr     x18, [x10]

  cmp     x17, x18

end

// ; cmpfi
// ; NOTE : it is presumed that arg1 < 0 (it is inverted in jitcompiler)
inline %0C8h

  sub     x11, x29, -__arg12_1
  ldr     x11, [x11]
  cmp     x10, x11

end 

// ; cmpfi
// ; NOTE : it is presumed that arg1 > 0 (it is inverted in jitcompiler)
inline %5C8h

  add     x11, x29, __arg12_1
  ldr     x11, [x11]
  cmp     x10, x11

end 

// ; cmpsi
inline %0C9h

  add     x11, sp, __arg12_1
  ldr     x11, [x11]
  cmp     x10, x11

end 

// ; cmpsi 0
inline %1C9h

  mov     x11, x0
  cmp     x10, x11

end 

// ; cmpsi 1
inline %2C9h

  mov     x10, x1
  cmp     x10, x11

end 

// ; copydpn
inline %0E0h

  mov     x11, __n16_2
  mov     x12, x0
  add     x13, x29, __arg12_1

labLoop:
  cmp     x11, 0
  beq     labEnd
  sub     x11, x11, 1
  ldrb    w14, [x12], #1
  strb    w14, [x13], #1
  b       labLoop

labEnd:

end

// ; iaddndp
inline %0E1h

  add     x19, x29, __arg12_1

  ldrsw   x17, [x0]
  ldrsw   x18, [x19]

  add     x17, x17, x18  

  str     w17, [x19]

end

// ; iaddndp
inline %1E1h

  add     x19, x29, __arg12_1

  ldrsb   x17, [x0]
  ldrsb   x18, [x19]

  add     x17, x17, x18  

  strb    w17, [x19]

end

// ; iaddndp
inline %2E1h

  add     x19, x29, __arg12_1

  ldrsh   x17, [x0]
  ldrsh   x18, [x19]

  add     x17, x17, x18  

  strh    w17, [x19]

end

// ; iaddndp
inline %4E1h

  add     x19, x29, __arg12_1

  ldr     x17, [x0]
  ldr     x18, [x19]

  add     x17, x17, x18  

  str     x17, [x19]

end

// ; isubndp
inline %0E2h

  add     x19, x29, __arg12_1

  ldr     w17, [x0]
  ldr     w18, [x19]

  sub     x17, x18, x17  

  str     w17, [x19]

end

// ; isubndp
inline %1E2h

  add     x19, x29, __arg12_1

  ldrsb   x17, [x0]
  ldrsb   x18, [x19]

  sub     x17, x17, x18  

  strb    w17, [x19]

end

// ; isubndp
inline %2E2h

  add     x19, x29, __arg12_1

  ldrsw   x17, [x0]
  ldrsw   x18, [x19]

  sub     x17, x17, x18  

  str     w17, [x19]

end

// ; isubndp
inline %4E2h

  add     x19, x29, __arg12_1

  ldr     x17, [x0]
  ldr     x18, [x19]

  sub     x17, x17, x18  

  str     x17, [x19]

end

// ; imulndp
inline %0E3h

  add     x19, x29, __arg12_1

  ldrsw   x17, [x0]
  ldrsw   x18, [x19]

  mul     x17, x17, x18  

  str     w17, [x19]

end

// ; imulndp
inline %1E3h

  add     x19, x29, __arg12_1

  ldrsb   x17, [x0]
  ldrsb   x18, [x19]

  mul     x17, x17, x18  

  strb    w17, [x19]

end

// ; imulndp
inline %2E3h

  add     x19, x29, __arg12_1

  ldrsb   x17, [x0]
  ldrsb   x18, [x19]

  mul     x17, x17, x18  

  strh    w17, [x19]

end

// ; imulndp
inline %4E3h

  add     x19, x29, __arg12_1

  ldr     x17, [x0]
  ldr     x18, [x19]

  mul     x17, x17, x18  

  str     x17, [x19]

end

// ; idivndp
inline %0E4h

  add     x19, x29, __arg12_1

  ldrsw   x17, [x0]
  ldrsw   x18, [x19]

  sdiv    x18, x18, x17    // ; sp[0] / temp

  str     w18, [x19]

end

// ; idivndp
inline %1E4h

  add     x19, x29, __arg12_1

  ldrsb   x17, [x0]
  ldrsb   x18, [x19]

  sdiv    x18, x18, x17  

  strb    w18, [x19]

end

// ; idivndp
inline %2E4h

  add     x19, x29, __arg12_1

  ldrsb   x17, [x0]
  ldrsb   x18, [x19]

  sdiv    x18, x18, x17  

  strh    w18, [x19]

end

// ; idivndp
inline %4E4h

  add     x19, x29, __arg12_1

  ldr     x17, [x0]
  ldr     x18, [x19]

  sdiv    x18, x18, x17  

  str     x18, [x19]

end

// ; nsavedpn
inline %0E5h

  add     x19, x29, __arg12_1
  mov     x18, __n16_2
  str     w18, [x19]

end

/ ; xhookdpr
inline %0E6h

  add     x13, x29, __arg12_1

  movz    x14,  data_ptr32lo : %CORE_ET_TABLE
  movk    x14,  data_ptr32hi : %CORE_ET_TABLE, lsl #16

  ldr     x15, [x14, et_current]!

  movz    x16,  __ptr32lo_2
  movk    x16,  __ptr32hi_2, lsl #16

  str     x15, [x13]
  str     x16, [x13, #8]!
  str     x29, [x13, #8]!
  str     sp, [x13, #8]!

end

// ; vjumpmr
inline %0ECh

  sub     x14, x10, elVMTOffset              
  ldr     x17, [x14]
  add     x17, x17, __arg12_1
  ldr     x17, [x17]
  br      x17

end

// ; jumpmr
inline %0EDh

  movz    x17,  __ptr32lo_2
  movk    x17,  __ptr32hi_2, lsl #16
  br      x17

end

// ; seleqrr
inline %0EEh

  movz    x11,  __ptr32lo_1
  movz    x12,  __ptr32lo_2
  movk    x11,  __ptr32hi_1, lsl #16
  movk    x12,  __ptr32hi_2, lsl #16

  csel    x10, x11, x12, eq

end

// ; selltrr
inline %0EFh

  movz    x11,  __ptr32lo_1
  movz    x12,  __ptr32lo_2
  movk    x11,  __ptr32hi_1, lsl #16
  movk    x12,  __ptr32hi_2, lsl #16

  csel    x10, x11, x12, lt

end

// ; openin
inline %0F0h

  stp     x29, x30, [sp, #-16]! 
  mov     x29, sp

  sub     sp, sp, __n12_2 // ; allocate raw stack

  mov     x11, #0
  stp     x11, x29, [sp, #-16]! 

  mov     x29, sp              // ; set frame pointer

  sub     sp, sp,  __arg12_1   // ; allocate stack
  mov     x11, __arg16_1
  mov     x12, #0
  mov     x13, sp

labLoop:
  cmp     x11, #0
  beq     labEnd
  sub     x11, x11, #8
  str     x12, [x13], #8
  b       labLoop

labEnd:

end 

// ; openin 0, 0
inline %1F0h

   stp     x29, x30, [sp, #-16]! 
   mov     x29, sp              // ; set frame pointer

end 

// ; openin 1, 0
inline %2F0h

   mov     x11, 0
   stp     x29, x30, [sp, #-16]! 
   mov     x29, sp              // ; set frame pointer
   stp     x11, x11, [sp, #-16]! 

end 

// ; openin 2, 0
inline %3F0h

   mov     x11, 0
   stp     x29, x30, [sp, #-16]! 
   mov     x29, sp              // ; set frame pointer
   stp     x11, x11, [sp, #-16]! 

end 

// ; openin 3, 0
inline %4F0h

  mov     x11, 0
  stp     x29, x30, [sp, #-16]! 
  mov     x29, sp              // ; set frame pointer
  stp     x11, x11, [sp, #-16]! 
  stp     x11, x11, [sp, #-16]! 

end 

// ; openin 0, n
inline %5F0h

  stp     x29, x30, [sp, #-16]! 
  mov     x29, sp

  sub     sp, sp, __n12_2 // ; allocate raw stack

  mov     x11, #0
  stp     x11, x29, [sp, #-16]! 

  mov     x29, sp              // ; set frame pointer

end 

// ; openin i, 0
inline %6F0h

  stp     x29, x30, [sp, #-16]! 
  mov     x29, sp

  sub     sp, sp,  __arg12_1   // ; allocate stack
  mov     x11, __arg16_1
  mov     x12, #0
  mov     x13, sp

labLoop:
  cmp     x11, #0
  beq     labEnd
  sub     x11, x11, #8
  str     x12, [x13], #8
  b       labLoop

labEnd:

end 

// ; xstoresir
inline %0F1h

  movz    x11,  __ptr32lo_2
  movk    x11,  __ptr32hi_2, lsl #16

  add     x12, sp, __arg12_1
  str     x11, [x12]

end

// ; xstoresir :0, ...
inline %1F1h

  movz    x0,  __ptr32lo_2
  movk    x0,  __ptr32hi_2, lsl #16

end

// ; xstoresir :1, ...
inline %2F1h

  movz    x1,  __ptr32lo_2
  movk    x1,  __ptr32hi_2, lsl #16

end

// ; xstoresir :0, 0
inline %6F1h

  mov     x0, #0

end

// ; xstoresir :1, 0
inline %7F1h

  mov     x1, #0

end

// ; openheaderin
inline %0F2h

  stp     x29, x30, [sp, #-16]! 
  mov     x29, sp

  sub     sp, sp, __n12_2 // ; allocate raw stack

  mov     x11, #0
  stp     x11, x29, [sp, #-16]! 

  mov     x29, sp              // ; set frame pointer

  sub     sp, sp,  __arg12_1   // ; allocate stack
  mov     x11, __arg16_1
  mov     x12, #0
  mov     x13, sp

labLoop:
  cmp     x11, #0
  beq     labEnd
  sub     x11, x11, #8
  str     x12, [x13], #8
  b       labLoop

labEnd:

end 

// ; openheaderin 0, 0
inline %1F2h

   stp     x29, x30, [sp, #-16]! 
   mov     x29, sp              // ; set frame pointer

end 

// ; openheaderin 1, 0
inline %2F2h

   mov     x11, #0
   stp     x29, x30, [sp, #-16]! 
   mov     x29, sp              // ; set frame pointer
   stp     x11, x11, [sp, #-16]! 

end 

// ; openheaderin 2, 0
inline %3F2h

   mov     x11, #0
   stp     x29, x30, [sp, #-16]! 
   mov     x29, sp              // ; set frame pointer
   stp     x11, x11, [sp, #-16]! 

end 

// ; openheaderin 3, 0
inline %4F2h

   mov     x11, #0
   stp     x29, x30, [sp, #-16]! 
   mov     x29, sp              // ; set frame pointer
   stp     x11, x11, [sp, #-16]! 
   stp     x11, x11, [sp, #-16]! 

end 

// ; openheaderin 0, n
inline %5F2h

  stp     x29, x30, [sp, #-16]! 
  mov     x29, sp

  sub     sp, sp, __n12_2 // ; allocate raw stack

  mov     x11, #0
  stp     x11, x29, [sp, #-16]! 

  mov     x29, sp              // ; set frame pointer

end 

// ; openheaderin i, 0
inline %6F2h

  stp     x29, x30, [sp, #-16]! 
  mov     x29, sp

  sub     sp, sp,  __arg12_1   // ; allocate stack
  mov     x11, __arg16_1
  mov     x12, #0
  mov     x13, sp

labLoop:
  cmp     x11, #0
  beq     labEnd
  sub     x11, x11, #8
  str     x12, [x13], #8
  b       labLoop

labEnd:

end 

// ; movsifi
inline %0F3h

  add     x12, x29, __arg12_1
  add     x13, sp, __arg12_2

  ldr     x11, [x12]
  str     x11, [x13]

end

// ; movsifi sp:0, fp:i2
inline %1F3h

  add     x12, x29, __arg12_1
  ldr     x0, [x12]

end

// ; movsifi sp:1, fp:i2
inline %2F3h

  add     x12, x29, __arg12_1
  ldr     x1, [x12]

end

// ; newir i, r
inline %0F4h

  mov     x11, __arg16_1
  movz    x17,  code_ptr32lo : %GC_ALLOC
  movk    x17,  code_ptr32hi : %GC_ALLOC, lsl #16
  blr     x17
  mov     x18, __n16_1
  movz    x19,  __ptr32lo_2
  movk    x19,  __ptr32hi_2, lsl #16
  sub     x20, x10, elVMTOffset
  str     x19, [x20]
  str     w18, [x20, #12]!

end

// ; newnr i, r
inline %0F5h

  mov     x11, __arg16_1
  movz    x17,  code_ptr32lo : %GC_ALLOC
  movk    x17,  code_ptr32hi : %GC_ALLOC, lsl #16
  blr     x17
  movz    x18, __n16_1
  movk    x18, __n16hi_1, lsl #16
  movz    x19,  __ptr32lo_2
  movk    x19,  __ptr32hi_2, lsl #16
  sub     x20, x10, elVMTOffset
  str     x19, [x20]
  str     w18, [x20, #12]!

end


// ; xmovsisi
inline %0F6h
 
  add     x12, sp, __arg12_1
  add     x13, sp, __arg12_2

  ldr     x11, [x12]
  str     x11, [x13]

end

// ; xmovsisi 1, n
inline %1F6h

  add     x13, sp, __arg12_2

  ldr     x0, [x13]

end

// ; xmovsisi n, 1
inline %2F6h

  add     x13, sp, __arg12_1

  str     x0, [x13]

end

// ; xmovsisi 2, n
inline %3F6h

  add     x13, sp, __arg12_2

  ldr     x1, [x13]

end

// ; xmovsisi n, 1
inline %4F6h

  add     x13, sp, __arg12_1

  str     x1, [x13]

end

// ; xmovsisi 1, 2
inline %5F6h

  mov     x0, x1

end

// ; xmovsisi 2, 1
inline %6F6h

  mov     x1, x0

end

// ; createnr n,r
inline %0F7h

  ldr     w19, [x0]
  movz    x18, __n16_1
  mul     x19, x19, x18
  add     x19, x18, page_ceil
  and     x11, x19, page_mask

  movz    x17,  code_ptr32lo : %GC_ALLOC
  movk    x17,  code_ptr32hi : %GC_ALLOC, lsl #16
  blr     x17

  ldr     w19, [x0]
  movz    x18, __n16_1
  mul     x18, x19, x18

  // ; adding mask

  movz    x19,  __ptr32lo_2
  movk    x19,  __ptr32hi_2, lsl #16
  sub     x20, x10, elVMTOffset
  str     x19, [x20]
  str     w18, [x20, #12]!

end

// ; xstorefir
inline %0F9h

  movz    x11,  __ptr32lo_2
  movk    x11,  __ptr32hi_2, lsl #16
  add     x12, x29, __arg12_1
  str     x11, [x12]

end

// ; xdispatchmr
// ; NOTE : __arg32_1 - message; __n_1 - arg count; __ptr32_2 - list, __n_2 - argument list offset
inline % 0FAh

//;  mov  [rsp+8], r10                      // ; saving arg0
  str     x0, [sp, #8]
//;  lea  rax, [rsp + __n_2]
  add     x17, sp, __n12_2
//;  mov  [rsp+16], r11                     // ; saving arg0
  str     x1, [sp, #16]

//;  mov  rsi, __ptr64_2
  movz    x21,  __ptr32lo_2
  movk    x21,  __ptr32hi_2, lsl #16

//;  xor  edx, edx
  mov     x25, #0
//;  mov  rbx, [rsi] // ; message from overload list
  ldr     x22, [x21, #0]

labNextOverloadlist:
//;  mov  r9, mdata : %0
  movz    x24,  mdata_ptr32lo : #0               //;--
  movk    x24,  mdata_ptr32hi : #0, lsl #16

//;  shr  ebx, ACTION_ORDER
  lsr     x22, x22, # ACTION_ORDER
//;  lea  r13, [rbx*8]
  lsl     x23, x22, #4

//;  mov  r13, [r9 + r13 * 2 + 8]
  add     x23, x23, x24
  ldr     x23, [x23, #8]

//;  mov  ecx, __n_1
  mov     x16, __n16_1

//;  lea  rbx, [r13 - 8]
  sub     x22, x23, #8

labNextParam:
//;  sub  ecx, 1
  sub     x16, x16, #1

//;  jnz  short labMatching
  cmp     x16, #0 
  bne     labMatching

//;  mov  r9, __ptr64_2  - r21

//;  mov  rax, [r9 + rdx * 16 + 8]
//;  mov  rdx, [r9 + rdx * 16]
//;  jmp  rax

  lsl     x23, x25, #4
  add     x23, x21, x23

  ldr     x9, [x23], #8
  ldr     x17, [x23]

  br      x17

labMatching:
//;  mov  rdi, [rax + rcx * 8]
  lsl     x19, x16, #3
  add     x18, x19, x17 
  ldr     x18, [x18]

  //; check nil
//;  mov   rsi, rdata : %VOIDPTR + elObjectOffset
  movz    x20,  rdata_ptr32hi : %VOIDPTR
  movk    x20,  rdata_ptr32hi : %VOIDPTR, lsl #16

//;  test  rdi, rdi                                              
  cmp     x18, #0

//;  cmovz rdi, rsi
  csel   x18, x20, x18, eq

//;  mov  rdi, [rdi - elVMTOffset]
  sub     x18, x18, elVMTOffset
  ldr     x18, [x18, #0]

//;  mov  rsi, [rbx + rcx * 8]
  add     x20,  x22, x19
  ldr     x20, [x20]

labNextBaseClass:
//;  cmp  rsi, rdi
  cmp     x20, x18

//;  jz   labNextParam
  beq     labNextParam 
//;  mov  rdi, [rdi - elPackageOffset]
  sub     x18, x18, elPackageOffset
  ldr     x18, [x18]

//;  and  rdi, rdi
  cmp     x18, #0
//;  jnz  short labNextBaseClass
  bne     labNextBaseClass

//;  add  rdx, 1
  add     x25, x25, #1
//;  mov  r13, __ptr32_2
  mov     x23, x21

//;  lea  r9, [rdx * 8]
  lsl     x24, x25, 4

//;  mov  rbx, [r13 + r9 * 2] // ; message from overload list
  add     x22, x24, x23
  ldr     x22, [x22]

//;  and  rbx, rbx
  cmp     x22, 0
//;  jnz  labNextOverloadlist
  bne     labNextOverloadlist

end

// ; dispatchmr
// ; NOTE : __arg32_1 - message; __n_1 - arg count; __ptr32_2 - list, __n_2 - argument list offset
inline % 0FBh

//;  mov  [rsp+8], r10                      // ; saving arg0
  str     x0, [sp, #8]
//;  lea  rax, [rsp + __n_2]
  add     x17, sp, __n12_2
//;  mov  [rsp+16], r11                     // ; saving arg0
  str     x1, [sp, #16]

//;  mov  rsi, __ptr64_2
  movz    x21,  __ptr32lo_2
  movk    x21,  __ptr32hi_2, lsl #16

//;  xor  edx, edx
  mov     x25, #0
//;  mov  rbx, [rsi] // ; message from overload list
  ldr     x22, [x21, #0]

labNextOverloadlist:
//;  mov  r9, mdata : %0
  movz    x24,  mdata_ptr32lo : #0               //;--
  movk    x24,  mdata_ptr32hi : #0, lsl #16

//;  shr  ebx, ACTION_ORDER
  lsr     x22, x22, # ACTION_ORDER
//;  lea  r13, [rbx*8]
  lsl     x23, x22, #4

//;  mov  r13, [r9 + r13 * 2 + 8]
  add     x23, x23, x24
  ldr     x23, [x23, #8]

//;  mov  ecx, __n_1
  mov     x16, __n16_1

//;  lea  rbx, [r13 - 8]
  sub     x22, x23, #8

labNextParam:
//;  sub  ecx, 1
  sub     x16, x16, #1

//;  jnz  short labMatching
  cmp     x16, #0 
  bne     labMatching

//;  mov  r9, __ptr64_2  - r21

//;  mov  r13, [r9 + rdx * 16 + 8] 
  lsl     x23, x25, #4
  add     x25, x21, x23
  ldr     x23, [x25, #8] 

//;  mov  rcx, [rbx - elVMTOffset]
  sub     x16, x10, elVMTOffset
  ldr     x16, [x16, #0]

//;  lea  rax, [r13 * 16]
  lsl     x17, x23, #4

//;  mov  rdx, [r9 + r13 * 2]        // c02
  lsl     x23, x23, #1
  add     x14, x21, x23 
  ldr     x9, [x14, #0]
//;  jmp  [rcx + rax + 8]       // rax - 0
  add     x20, x16, x17

  ldr     x17, [x20, #8]
  br      x17

labMatching:
//;  mov  rdi, [rax + rcx * 8]
  lsl     x19, x16, #3
  add     x18, x19, x17 
  ldr     x18, [x18]

  //; check nil
//;  mov   rsi, rdata : %VOIDPTR + elObjectOffset
  movz    x20,  rdata_ptr32hi : %VOIDPTR
  movk    x20,  rdata_ptr32hi : %VOIDPTR, lsl #16

//;  test  rdi, rdi                                              
  cmp     x18, #0

//;  cmovz rdi, rsi
  csel   x18, x20, x18, eq

//;  mov  rdi, [rdi - elVMTOffset]
  sub     x18, x18, elVMTOffset
  ldr     x18, [x18, #0]

//;  mov  rsi, [rbx + rcx * 8]
  add     x20,  x22, x19
  ldr     x20, [x20]

labNextBaseClass:
//;  cmp  rsi, rdi
  cmp     x20, x18

//;  jz   labNextParam
  beq     labNextParam 
//;  mov  rdi, [rdi - elPackageOffset]
  sub     x18, x18, elPackageOffset
  ldr     x18, [x18]

//;  and  rdi, rdi
  cmp     x18, #0
//;  jnz  short labNextBaseClass
  bne     labNextBaseClass

//;  add  rdx, 1
  add     x25, x25, #1
//;  mov  r13, __ptr32_2
  mov     x23, x21

//;  lea  r9, [rdx * 8]
  lsl     x24, x25, 4

//;  mov  rbx, [r13 + r9 * 2] // ; message from overload list
  add     x22, x24, x23
  ldr     x22, [x22]

//;  and  rbx, rbx
  cmp     x22, 0
//;  jnz  labNextOverloadlist
  bne     labNextOverloadlist

end

// ; vcallmr
inline %0FCh

  sub     x14, x10, elVMTOffset              
  ldr     x17, [x14]
  add     x17, x17, __arg12_1
  ldr     x17, [x17]
  blr     x17

end

// ; callmr
inline %0FDh

  movz    x17,  __ptr32lo_2
  movk    x17,  __ptr32hi_2, lsl #16
  blr     x17

end

// ; callext
inline %0FEh

  add     x15, sp, #16
  ldr     x2, [x15]  
  ldr     x3, [x15, #8]  

  movz    x16,  __ptr32lo_1
  movk    x16,  __ptr32hi_1, lsl #16
  ldr     x17, [x16]
  blr     x17

end
