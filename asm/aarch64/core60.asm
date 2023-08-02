// ; --- Predefined References  --
define INVOKER               10001h
define GC_ALLOC	             10002h
define VEH_HANDLER           10003h
define GC_COLLECT	     10004h
define GC_ALLOCPERM	     10005h
define PREPARE	            10006h

define CORE_TOC              20001h
define SYSTEM_ENV            20002h
define CORE_GC_TABLE         20003h
define CORE_SINGLE_CONTENT   2000Bh
define VOID           	     2000Dh
define VOIDPTR               2000Eh
define CORE_THREAD_TABLE     2000Fh

define ACTION_ORDER              9
define ARG_MASK               01Fh
define ARG_ACTION_MASK        1DFh

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
define gc_yg_start           0010h
define gc_yg_current         0018h
define gc_yg_end             0020h
define gc_mg_start           0038h
define gc_mg_current         0040h
define gc_end                0048h
define gc_mg_wbar            0050h
define gc_perm_start         0058h 
define gc_perm_end           0060h 
define gc_perm_current       0068h 

define et_current            0008h
define tt_stack_frame        0010h

define es_prev_struct        0000h
define es_catch_addr         0008h
define es_catch_level        0010h
define es_catch_frame        0018h

// ; --- Page Size ----
define page_size_order          5h
define page_ceil               2Fh
define page_mask        0FFFFFFE0h
define struct_mask_inv     7FFFFFh

define struct_mask       40000000h
define struct_mask_lo        0000h
define struct_mask_hi        4000h

// ; --- System Core Preloaded Routines --

structure % CORE_TOC

  dq 0         // ; address of import section

end
 
structure % CORE_SINGLE_CONTENT

  dq 0 // ; critical_handler       ; +x00   - pointer to ELENA critical exception handler
  dq 0 // ; et_current             ; +x08   - pointer to the current exception struct
  dq 0 // ; tt_stack_frame         ; +x10   - pointer to the stack frame

end
 
structure % CORE_THREAD_TABLE

  // ; dummy for STA

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

  dq 0 // ; gc_perm_start         : +58h 
  dq 0 // ; gc_perm_end           : +60h 
  dq 0 // ; gc_perm_current       : +68h 

  dq 0 // ; gc_lock               : +70h 
  dq 0 // ; reserved              : +78h 

end

// ; NOTE : the table is tailed with GCMGSize,GCYGSize and MaxThread fields
structure %SYSTEM_ENV

  dq 0
  dq data : %CORE_GC_TABLE
  dq data : %CORE_SINGLE_CONTENT
  dq 0
  dq code : %INVOKER
  dq code : %VEH_HANDLER
  // ; dd GCMGSize
  // ; dd GCYGSize
  // ; dd ThreadCounter

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
  // ; save registers
  sub     x11, x11, x15

  stp     x0,  x1, [sp, #-16]! 
  stp     x29, x30, [sp, #-16]! 
  mov     x29, sp              // ; set frame pointer

  // ; lock frame
  movz    x14,  data_ptr32lo : %CORE_SINGLE_CONTENT
  movk    x14,  data_ptr32hi : %CORE_SINGLE_CONTENT, lsl #16
  add     x14, x14, # tt_stack_frame

  mov     x12, sp
  str     x12, [x14]

  stp     x11, x11, [sp, #-16]! 

  // ; create set of roots
  mov     x29, sp
  mov     x18, 0
  stp     x18, x18, [sp, #-16]! 

  // ;   save static roots
  movz    x17, rdata_ptr32lo : %SYSTEM_ENV
  movk    x17, rdata_ptr32hi : %SYSTEM_ENV, lsl #16

  movz    x19, stat_ptr32lo : #0
  movk    x19, stat_ptr32hi : #0, lsl #16
  ldr     x18, [x17]
  lsl     x18, x18, #3
  stp     x18, x19, [sp, #-16]! 

  // ;   collect frames
  ldr     x19, [x14]
  mov     x18, x19

labYGNextFrame:
  mov     x17, x19
  ldr     x19, [x17]
  cmp     x19, #0
  bne     labYGNextFrame

  mov     x20, x18
  sub     x18, x17, x18
  stp     x18, x20, [sp, #-16]! 

  ldr     x19, [x17, #8]!
  cmp     x19, #0
  mov     x18, x19
  bne     labYGNextFrame

  mov     x20, sp
  str     x20, [x29]

  add     x19, x29, #8
  ldr     x1, [x19]
  mov     x0, sp

  // ; restore frame to correctly display a call stack
  stp     x29, x29, [sp, #-16]! 

  ldr     x29, [x29]

  // ; call GC routine
  movz    x16,  import_ptr32lo : "$rt.CollectGCLA"
  movk    x16,  import_ptr32hi : "$rt.CollectGCLA", lsl #16

  ldr     x17, [x16]
  blr     x17

  mov     x10, x0

//;  ldp     x19, x29, [sp, #-16]! 
  ldp     x19, x29, [sp], #16
  add     x29, x29, #16
  mov     sp, x29
  ldp     x29, x30, [sp], #16
  ldp     x0,  x1, [sp], #16

  ret     x30

end

// ; --- GC_COLLECT ---
// ; in: ecx - fullmode (0, 1)
inline % GC_COLLECT

  stp     x0,  x1, [sp, #-16]! 
  stp     x29, x30, [sp, #-16]! 
  mov     x29, sp              // ; set frame pointer

  // ; lock frame
  movz    x14,  data_ptr32lo : %CORE_SINGLE_CONTENT
  movk    x14,  data_ptr32hi : %CORE_SINGLE_CONTENT, lsl #16
  add     x14, x14, # tt_stack_frame

  mov     x12, sp
  str     x12, [x14]

  stp     x11, x11, [sp, #-16]! 

  // ; create set of roots
  mov     x29, sp
  mov     x18, 0
  stp     x18, x18, [sp, #-16]! 

  // ;   save static roots
  movz    x17, rdata_ptr32lo : %SYSTEM_ENV
  movk    x17, rdata_ptr32hi : %SYSTEM_ENV, lsl #16

  movz    x19, stat_ptr32lo : #0
  movk    x19, stat_ptr32hi : #0, lsl #16
  ldr     x18, [x17]
  lsl     x18, x18, #3
  stp     x18, x19, [sp, #-16]! 

  // ;   collect frames
  ldr     x19, [x14]
  mov     x18, x19

labYGNextFrame:
  mov     x17, x19
  ldr     x19, [x17]
  cmp     x19, #0
  bne     labYGNextFrame

  mov     x20, x18
  sub     x18, x17, x18
  stp     x18, x20, [sp, #-16]! 

  ldr     x19, [x17, #8]!
  cmp     x19, #0
  mov     x18, x19
  bne     labYGNextFrame

  mov     x20, sp
  str     x20, [x29]

  add     x19, x29, #8
  ldr     x1, [x19]
  mov     x0, sp

  // ; restore frame to correctly display a call stack
  stp     x29, x29, [sp, #-16]! 

  ldr     x29, [x29]

  // ; call GC routine
  movz    x16,  import_ptr32lo : "$rt.CollectGCLA"
  movk    x16,  import_ptr32hi : "$rt.CollectGCLA", lsl #16

  ldr     x17, [x16]
  blr     x17

  mov     x10, x0

//;  ldp     x19, x29, [sp, #-16]! 
  ldp     x19, x29, [sp], #16
  add     x29, x29, #16
  mov     sp, x29
  ldp     x29, x30, [sp], #16
  ldp     x0,  x1, [sp], #16

  ret     x30

end

// --- GC_ALLOCPERM ---
procedure %GC_ALLOCPERM

  movz    x12,  data_ptr32lo : %CORE_GC_TABLE
  movk    x12,  data_ptr32hi : %CORE_GC_TABLE, lsl #16
  add     x13, x12, gc_perm_current
  ldr     x15, [x13]
  add     x14, x12, gc_perm_end
  ldr     x14, [x14]
  add     x11, x11, x15
  cmp     x11, x14
  bge     labYGCollect
  str     x11, [x13]
  add     x10, x15, elObjectOffset
  ret     x30

labYGCollect:
  // ; save registers
  sub     x11, x11, x15

  stp     x0,  x1, [sp, #-16]! 
  stp     x29, x30, [sp, #-16]! 
  mov     x29, sp              // ; set frame pointer

  // ; lock frame
  movz    x14,  data_ptr32lo : %CORE_SINGLE_CONTENT
  movk    x14,  data_ptr32hi : %CORE_SINGLE_CONTENT, lsl #16
  add     x14, x14, # tt_stack_frame

  mov     x12, sp
  str     x12, [x14]

  stp     x11, x11, [sp, #-16]! 

  mov     x20, sp
  str     x20, [x29]

  add     x19, x29, #8
  ldr     x1, [x19]
  mov     x0, sp

  // ; restore frame to correctly display a call stack
  stp     x29, x29, [sp, #-16]! 

  ldr     x29, [x29]

  // ; call GC routine
  movz    x16,  import_ptr32lo : "$rt.CollectPermGCLA"
  movk    x16,  import_ptr32hi : "$rt.CollectPermGCLA", lsl #16

  ldr     x17, [x16]
  blr     x17

  mov     x10, x0

//;  ldp     x19, x29, [sp, #-16]! 
  ldp     x19, x29, [sp], #16
  add     x29, x29, #16
  mov     sp, x29
  ldp     x29, x30, [sp], #16
  ldp     x0,  x1, [sp], #16

  ret     x30

end

procedure %PREPARE

  stp     x29, x30, [sp, #-16]! 

  // ; call GC routine
  movz    x16,  import_ptr32lo : "$rt.PrepareLA"
  movk    x16,  import_ptr32hi : "$rt.PrepareLA", lsl #16

  mov     x0, x12

  ldr     x17, [x16]
  blr     x17

  ldp     x29, x30, [sp], #16

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

  movz    x14,  data_ptr32lo : %CORE_SINGLE_CONTENT
  movk    x14,  data_ptr32hi : %CORE_SINGLE_CONTENT, lsl #16

  ldr     x14, [x14, # et_current]!
  ldr     x17, [x14, # es_catch_addr]!

  br      x17

end

// ; unhook
inline %0Bh

  movz    x14,  data_ptr32lo : %CORE_SINGLE_CONTENT
  movk    x14,  data_ptr32hi : %CORE_SINGLE_CONTENT, lsl #16

  add     x14, x14, # et_current
  ldr     x13, [x14]

  ldr     x15, [x13]
  ldr     x16, [x13, #8]!
  ldr     x17, [x13, #8]!
  ldr     x29, [x13, #8]!

  str     x15, [x14]

  mov     sp, x17

end

// ; loadv
inline % 0Ch

  //;and     x9, x9, ARG_MASK
  mov     x16, ARG_MASK
  and     x9, x9, x16

  movz    x16,  ~ARG_MASK
  movk    x16,  #0FFFFh, lsl #16

  ldrsw   x14, [x10]
  and     x14, x14, x16

  orr     x9, x9, x14

end

// ; xcmp
inline %0Dh

  ldrsw   x14, [x10]
  cmp     x9, x14

end

// ; bload
inline %0Eh

  ldrb    w9, [x10]

end

// ; wload
inline %0Fh

  ldrsw   x9, [x10]
  sxth    x9, x9

end

// ; exclude
inline % 10h

  mov      x18, 0

  stp      x18, x29, [sp, #-16]! 

  movz    x14,  data_ptr32lo : %CORE_SINGLE_CONTENT
  movk    x14,  data_ptr32hi : %CORE_SINGLE_CONTENT, lsl #16
  add     x14, x14, # tt_stack_frame

end

// ; include
inline % 11h

  add     sp, sp, 10h          // ; free stack

end

// ; assign
inline %12h

  lsl     x11, x9, 3
  add     x11, x11, x10

  // calculate write-barrier address
  movz    x12, data_ptr32lo : %CORE_GC_TABLE
  movk    x12, data_ptr32hi : %CORE_GC_TABLE, lsl #16

  add     x13, x12, gc_start
  add     x15, x12, gc_header
  ldr     x14, [x13]
  sub     x14, x10, x14
  ldr     x15, [x15]

  lsr     x14, x14, page_size_order
  add     x14, x15, x14
  mov     x12, 1
  strb    w12, [x14]

  str     x0, [x11]

end

// ; movfrm
inline %13h

  mov  x9, x29

end

// ; loads
inline % 14h

  ldr     x22, [x10]
  movz    x24,  mdata_ptr32lo : #0               //;--
  movk    x24,  mdata_ptr32hi : #0, lsl #16
  lsr     x22, x22, # ACTION_ORDER
  add     x22, x22, x24 
  ldr     x9, [x22]

end

// ; mlen
inline % 15h

  and   x9, x9, ARG_MASK

end

// ; dalloc
inline % 16h

  lsl     x12, x14, #3

  add     x12, x12, #8    // ; rounding to 10h

  lsr     x12, x12, #4
  lsl     x12, x12, #4

  sub     sp, sp,  x12   // ; allocate stack
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

// ; dtrans
inline %18h

  mov     x11, x9
  mov     x12, x0
  mov     x13, x10

labLoop:
  cmp     x11, 0
  beq     labEnd
  sub     x11, x11, 1
  ldr     x14, [x12], #8
  str     x14, [x13], #8
  b       labLoop

labEnd:

end

// ; xassign
inline %19h

  lsl     x14, x9, 3
  add     x14, x14, x10
  str     x0, [x14]

end

// ; lload
inline %1Ah

  ldr     x9, [x10]

end

// ; convl
inline %1Bh

  sxtw    x9, w9

end

// ; xlcmp
inline % 1Ch

  ldr     x14, [x10]
  cmp     x14, x9

end

// ; xload
inline %1Dh

  add     x12, x10, __arg12_1
  ldrsw   x9, [x12]

end

// ; xlload
inline %1Eh

  add     x12, x10, __arg12_1
  ldr     x9, [x12]

end

// ; coalesce
inline % 20h

  cmp     x10, #0
  csel    x10, x0, x10, eq

end

// ; not
inline % 21h

   mvn    x9, x9

end

// ; neg
inline % 22h

   mov    x17, 0
   sub    x9, x17, x9

end

// ; bread
inline %23h

  add     x18, x0, x9
  ldrsb   x17, [x18]
  str     x17, [x10]

end

// ; lsave
inline %24h

  str    x9, [x10]

end

// ; fsave
inline %25h

  scvtf   d17, x9
  str     d17, [x10]

end

// ; wread
inline %26h

  lsl     x18, x9, #1
  add     x18, x0, x18
  ldrsw   x17, [x18]
  str     x17, [x10]

end

// ; xjump
inline %027h

  br      x10

end

// ; bcopy
inline %28h

  ldrsb   x17, [x0]
  str     x17, [x10]

end

// ; wcopy
inline %29h

  ldrsw   x17, [x0]
  str     x17, [x10]

end

// ; xpeekeq
inline %02Ah

  csel    x10, x0, x10, eq

end

// ; xget
inline %02Eh

  lsl     x14, x9, #3
  add     x17, x10, x14
  ldr     x10, [x17]

end

// ; xcall
inline %02Fh

  blr     x10

end

// ; fadd
inline %070h

  ldr     x19, [x0]
  ldr     d17, [x10]
  scvtf   d18, x19
  fadd    d17, d17, d18  
  str     d17, [x10]

end

// ; fsub
inline %071h

  ldr     x19, [x0]
  ldr     d17, [x10]
  scvtf   d18, x19
  fsub    d17, d17, d18  
  str     d17, [x10]

end

// ; fmul
inline %072h

  ldr     x19, [x0]
  ldr     d17, [x10]
  scvtf   d18, x19
  fmul    d17, d17, d18  
  str     d17, [x10]

end

// ; fdiv
inline %073h

  ldr     x19, [x0]
  ldr     d17, [x10]
  scvtf   d18, x19
  fdiv    d17, d17, d18  
  str     d17, [x10]

end

// ; fabsdp
inline %78h

  add     x19, x29, __arg12_1
  ldr     d18, [x0]
  fabs    d18, d18
  str     d18, [x19]

end

// ; fsqrtdp
inline %79h

  add     x19, x29, __arg12_1
  ldr     d18, [x0]
  fsqrt   d18, d18
  str     d18, [x19]

end

// ; fexpdp
inline %07Ah

//; static double expo(double n) {
//;     int a = 0, b = n > 0;
//;     double c = 1, d = 1, e = 1;
//;     for (b || (n = -n); e + .00001 < (e += (d *= n) / (c *= ++a)););
//;     // approximately 15 iterations
//;     return b ? e : 1 / e;

  mov     x15, #0
  fmov    d14, #"1E-4" // ; diff = 0.00001

  ldr     d7, [x0]     // ; n

  fcmp    d7, #0
  blt     labSkip
  fneg    d7, d7
  mov     x15, #1

labSkip:
  mov     x4, #0       // ; a = 0

  fmov    d3, #"1E0"   // ; e = 1
  fmov    d5, d3       // ; c = 1
  fmov    d6, d3       // ; d = 1

  // ; e = d3  a = x4, c = d5, d = d6 ; n = d7

labNext:
  // ; e + .00001 < (e += (d *= n) / (c *= ++a))
  fmov    d13, d3

  add     x4, x4, #1     // ; ++a
  fmov    d4, x4 
  frintx  d4, d4

  fmul    d5, d5, d4     // ; c *= (++a)
  fmul    d6, d6, d7     // ; d *= n
  fdiv    d12, d6, d5    // ; (d *= n) / (c *= ++a)
  fadd    d3, d3, d12    // ; e += (d *= n) / (c *= ++a)

  fsub    d13, d13, d3
  fcmp    d13, d14
  bgt     labNext

  cmp     x15, #0
  fmov    d16, #"1E0"
  bne     labSkip2
  fdiv    d3, d16, d3

labSkip2:
  add     x19, x29, __arg12_1
  str     d3, [x19]

end

// ; fln
inline %07Bh
end

// ; fsin
inline %07Ch
end

// ; fcos
inline %07Dh
end

// ; farchtan
inline %07Eh
end

// ; fpi
inline %07Fh
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

// ; setr -1
inline %680h

  mov     x10, #0
  sub     x10, x10, #1

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

// ; xswapsi
inline %86h

  mov     x13, x0
  add     x12, sp, __arg12_1
  ldr     x0, [x12]
  str     x13, [x12]

end

// ; xswapsi 0
inline %186h

end

// ; xswapsi 1
inline %286h

  mov     x13, x0
  mov     x0, x1
  mov     x1, x13

end

// ; swapsi
inline %87h

  mov     x13, x10
  add     x12, sp, __arg12_1
  ldr     x10, [x12]
  str     x13, [x12]

end

// ; swapsi 0
inline %187h

  mov     x13, x10
  mov     x10, x0
  mov     x0, x13

end

// ; swapsi 1
inline %287h

  mov     x13, x10
  mov     x10, x1
  mov     x1, x13

end

// ; movm
inline %88h

  movz    x9,  __arg32lo_1
  movk    x9,  __arg32hi_1, lsl #16

end

// ; movn
inline %89h

  mov    x9,  __n16_1

end

// ; loaddp
inline %8Ah

  add     x11, x29, __arg12_1
  ldrsw   x9,  [x11]

end 

// ; xcmpdp
inline %8Bh

  add     x11, x29, __arg12_1
  ldrsw   x14, [x11]
  cmp     x9, x14

end 

// ; subn
inline %8Ch

  mov    x11,  __n16_1
  sub    x9, x9, x11

end

// ; addn
inline %8Dh

  mov    x11,  __n16_1
  add    x9, x9, x11

end

// ; setfi
// ; NOTE : it is presumed that arg1 < 0 (it is inverted in jitcompiler)
inline %08Eh

  sub     x10, x29, -__arg12_1

end 

// ; setfi
// ; NOTE : it is presumed that arg1 > 0 (it is inverted in jitcompiler)
inline %58Eh

  add     x10, x29, __arg12_1

end 

// ; create r
inline %08Fh

  ldr     w19, [x0]
  lsl     x19, x19, #3

  add     x19, x19, page_ceil
  and     x11, x19, page_mask

  movz    x17,  code_ptr32lo : %GC_ALLOC
  movk    x17,  code_ptr32hi : %GC_ALLOC, lsl #16
  blr     x17

  ldr     w19, [x0]
  lsl     x18, x19, #3

  movz    x19,  __ptr32lo_1
  movk    x19,  __ptr32hi_1, lsl #16
  sub     x20, x10, elVMTOffset
  str     x19, [x20]
  str     w18, [x20, #12]!

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

// ; cmpn n
inline %97h

  mov     x18, __n16_1   // ; temporally
  cmp     x9, x18

end

// ; nconfdp
inline %098h

  add      x19, x29, __arg12_1
  ldr      d0, [x10]
  frintx   d0, d0
  fcvtzs   x18, d0
  str      x18, [x19]

end

// ; ftruncdp
inline %099h

  add     x19, x29, __arg12_1
  ldr     d18, [x0]
  frintz  d18, d18
  str     d18, [x19]

end

// ; dcopy
inline %9Ah

  mov     x11, __n16_1
  mul     x11, x11, x9
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

// ; orn
inline %9Bh

  mov     x11, __n16_1   // ; temporally
  orr     x9, x9, x11

end

// ; muln
inline %9Ch

  mov     x11, __n16_1   // ; temporally
  mul     x9, x9, x11

end

// ; xadddp
inline %9Dh

  add     x11, x29, __arg12_1
  ldrsw   x12,  [x11]
  add     x9, x9, x12

end 

// ; xsetfi
// ; NOTE : it is presumed that arg1 < 0 (it is inverted in jitcompiler)
inline %09Eh

  lsl     x14, x9, #3
  sub     x10, x29, -__arg12_1
  sub     x10, x10, x14

end 

// ; xsetfi
// ; NOTE : it is presumed that arg1 > 0 (it is inverted in jitcompiler)
inline %59Eh

  lsl     x14, x9, #3
  add     x10, x29, __arg12_1
  add     x10, x10, x14

end 
// ; frounddp
inline %9Fh

  add     x19, x29, __arg12_1
  ldr     d18, [x0]
  frintn  d18, d18
  str     d18, [x19]

end

// ; saveddp
inline %0A0h

  add     x11, x29, __arg12_1
  str     w9, [x11]        

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

// ; assigni
inline %0A6h

  add     x11, x10, __arg12_1

  // calculate write-barrier address
  movz    x12, data_ptr32lo : %CORE_GC_TABLE
  movk    x12, data_ptr32hi : %CORE_GC_TABLE, lsl #16

  add     x13, x12, gc_start
  add     x15, x12, gc_header
  ldr     x14, [x13]
  sub     x14, x10, x14
  ldr     x15, [x15]

  lsr     x14, x14, page_size_order
  add     x14, x15, x14
  mov     x12, 1
  strb    w12, [x14]

  str     x0, [x11]

end

// ; xrefreshsi i
inline %0A7h

end 

// ; xrefreshsi 0
inline %1A7h

  ldr     x0, [sp]

end 

// ; xrefreshsi 1
inline %2A7h

  add     x11, sp, 8
  ldr     x1, [x11]

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

// ; lsavedp
inline %0AAh

  add     x11, x29, __arg12_1
  str     x9, [x11]        

end

// ; lsavesi
inline %0ABh

  add     x11, sp, __arg12_1
  str     x9, [x11]

end 

// ; lsavesi 0
inline %1ABh

  mov     x0, x9

end 

// ; lsavesi 1
inline %2ABh

  mov     x1, x9

end 

// ; lloaddp
inline %0ACh

  add     x11, x29, __arg12_1
  ldr     x9,  [x11]

end 

// ; xfillr
inline % 0ADh

  ldr     w11, [x0]
  lsl     x11, x11, #3

  movz    x12,  __ptr32lo_1
  movk    x12,  __ptr32hi_1, lsl #16
  mov     x13, x10

labLoop:
  cmp     x11, 0
  beq     labEnd
  sub     x11, x11, 8
  str     x12, [x13], #8
  b       labLoop

labEnd:

end

// ; xfillr 0
inline % 1ADh

  ldr     w11, [x0]
  lsl     x11, x11, #3

  movz    x12, #0
  mov     x13, x10

labLoop:
  cmp     x11, 0
  beq     labEnd
  sub     x11, x11, 8
  str     x12, [x13], #8
  b       labLoop

labEnd:

end

// ; xstorei
inline % 0AEh

  add     x11, x10, __arg12_1
  ldr     x0, [x11]

end

// ; xassignsp
inline % 0AFh

  add     x10, sp, __arg12_1

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

// ; jumpvi
inline %0B5h

  sub     x14, x10, elVMTOffset              
  ldr     x17, [x14]
  add     x17, x17, __arg12_1
  ldr     x17, [x17]
  br      x17

end

// ; xredirect
inline % 0B6h //; (r15 - object, r14 - message)

  mov     x20, x9
  sub     x14, x10, elVMTOffset              
  ldr     x11, [x14]              //; edi
  mov     x12, #0                 //; ecx
  sub     x15, x11, elVMTSizeOffset
  ldr     x13, [x15]              //; esi

  movz    x14,  __arg32lo_1
  movk    x14,  __arg32hi_1, lsl #16
  and     x9, x9, ARG_ACTION_MASK
  movz    x16,  ~ARG_MASK
  movk    x16,  #0FFFFh, lsl #16
  and     x14, x14, x16
  orr     x9, x9, x14

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
  mov     x9, x20
  add     x14, x14, #8 
  ldr     x15, [x14] 
  br      x15

labEnd:
  mov     x9, x20
                               
end

// ; cmpr
inline %0C0h

  movz    x11,  __ptr32lo_1
  movk    x11,  __ptr32hi_1, lsl #16
  cmp     x10, x11

end 

// ; cmpr 0
inline %1C0h

  mov     x11, #0
  cmp     x10, x11

end 

// ; cmpr -1
inline %6C0h

  mov     x11, #0
  sub     x11, x11, #1
  cmp     x10, x11

end 

// ; fcmpn 8
inline %0C1h

  ldr     d17, [x0]
  ldr     d18, [x10]

  fcmp    d17, d18

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
inline %4C2h

  ldr     x17, [x0]
  ldr     x18, [x10]

  cmp     x17, x18

end

// ; tstflg
inline %0C3h

  sub     x14, x10, elVMTOffset              
  ldr     x14, [x14]              
  sub     x14, x14, elVMTFlagOffset
  ldr     x14, [x14]              

  movz    x11,  __n16lo_1
  movk    x11,  __n16hi_1, lsl #16

  tst     x14, x11

end

// ; tstn
inline %0C4h

  movz    x11,  __n16lo_1
  movk    x11,  __n16hi_1, lsl #16

  tst     x9, x11

end

// ; tstm
inline % 0C5h

  movz    x16,  __arg32lo_1
  movk    x16,  __arg32hi_1, lsl #16

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
  cmp     x16, x15
  beq     labFound
  add     x14, x14, #16
  ble     labSplit
  mov     x11, x14
  sub     x13, x13, x12
  b       labSplit

labFound:
  mov     x13, #1

labEnd:
  cmp     x13, #1
                               
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

  mov     x11, x1
  cmp     x10, x11

end 

// ; xloadargsi
inline %0CDh

  add     x11, sp, __arg12_1
  ldr     x9, [x11]

end 

// ; xloadargsi 0
inline %1CDh

  mov     x9, x0

end 

// ; xloadargsi 1
inline %2CDh

  mov     x9, x1

end 

// ; xcreate r
inline %0CEh

  ldr     w19, [x0]
  lsl     x19, x19, #3

  add     x19, x19, page_ceil
  and     x11, x19, page_mask

  movz    x17,  code_ptr32lo : %GC_ALLOCPERM
  movk    x17,  code_ptr32hi : %GC_ALLOCPERM, lsl #16
  blr     x17

  ldr     w19, [x0]
  lsl     x18, x19, #3

  movz    x19,  __ptr32lo_1
  movk    x19,  __ptr32hi_1, lsl #16
  sub     x20, x10, elVMTOffset
  str     x19, [x20]
  str     w18, [x20, #12]!

end

// ; system
inline %0CFh

end


// ; system 4
inline %4CFh

  mov     x12, sp

  movz    x17,  code_ptr32lo : %PREPARE
  movk    x17,  code_ptr32hi : %PREPARE, lsl #16
  blr     x17

end

// ; faddndp
inline %0D0h

  add     x19, x29, __arg12_1

  ldr     d17, [x0]
  ldr     d18, [x19]

  fadd    d17, d17, d18  

  str     d17, [x19]

end

// ; fsubndp
inline %0D1h

  add     x19, x29, __arg12_1

  ldr     d17, [x0]
  ldr     d18, [x19]

  fsub    d17, d18, d17  

  str     d17, [x19]

end

// ; fmulndp
inline %0D2h

  add     x19, x29, __arg12_1

  ldr     d17, [x0]
  ldr     d18, [x19]

  fmul    d17, d17, d18  

  str     d17, [x19]

end

// ; fdivndp
inline %0D3h

  add     x19, x29, __arg12_1

  ldr     d17, [x0]
  ldr     d18, [x19]

  fdiv    d18, d18, d17    // ; sp[0] / temp

  str     d18, [x19]

end

// ; udivndp
inline %0D4h

  add     x19, x29, __arg12_1

  ldr     w17, [x0]
  ldr     w18, [x19]

  udiv    x18, x18, x17    // ; sp[0] / temp

  str     w18, [x19]

end

// ; selgrrr
inline %0D7h

  movz    x11,  __ptr32lo_1
  movz    x12,  __ptr32lo_2
  movk    x11,  __ptr32hi_1, lsl #16
  movk    x12,  __ptr32hi_2, lsl #16

  csel    x10, x11, x12, gt

end

// ; ianddpn
inline %0D8h

  add     x19, x29, __arg12_1

  ldrsw   x17, [x0]
  ldrsw   x18, [x19]

  and     x17, x17, x18  

  str     w17, [x19]

end

// ; ianddpn
inline %1D8h

  add     x19, x29, __arg12_1

  ldrsb   x17, [x0]
  ldrsb   x18, [x19]

  and     x17, x17, x18  

  strb    w17, [x19]

end

// ; ianddpn
inline %2D8h

  add     x19, x29, __arg12_1

  ldrsh   x17, [x0]
  ldrsh   x18, [x19]

  and     x17, x17, x18  

  strh    w17, [x19]

end

// ; ianddpn
inline %4D8h

  add     x19, x29, __arg12_1

  ldr     x17, [x0]
  ldr     x18, [x19]

  and     x17, x17, x18  

  str     x17, [x19]

end

// ; iordpn
inline %0D9h

  add     x19, x29, __arg12_1

  ldrsw   x17, [x0]
  ldrsw   x18, [x19]

  orr     x17, x17, x18  

  str     w17, [x19]

end

// ; iordpn
inline %1D9h

  add     x19, x29, __arg12_1

  ldrsb   x17, [x0]
  ldrsb   x18, [x19]

  orr     x17, x17, x18  

  strb    w17, [x19]

end

// ; iordpn
inline %2D9h

  add     x19, x29, __arg12_1

  ldrsh   x17, [x0]
  ldrsh   x18, [x19]

  orr     x17, x17, x18  

  strh    w17, [x19]

end

// ; iordpn
inline %4D9h

  add     x19, x29, __arg12_1

  ldr     x17, [x0]
  ldr     x18, [x19]

  orr     x17, x17, x18  

  str     x17, [x19]

end

// ; ixordpn
inline %0DAh

  add     x19, x29, __arg12_1

  ldrsw   x17, [x0]
  ldrsw   x18, [x19]

  eor     x17, x17, x18  

  str     w17, [x19]

end

// ; ixordpn
inline %1DAh

  add     x19, x29, __arg12_1

  ldrsb   x17, [x0]
  ldrsb   x18, [x19]

  eor     x17, x17, x18  

  strb    w17, [x19]

end

// ; ixordpn
inline %2DAh

  add     x19, x29, __arg12_1

  ldrsh   x17, [x0]
  ldrsh   x18, [x19]

  eor     x17, x17, x18  

  strh    w17, [x19]

end

// ; ixordpn
inline %4DAh

  add     x19, x29, __arg12_1

  ldr     x17, [x0]
  ldr     x18, [x19]

  eor     x17, x17, x18  

  str     x17, [x19]

end

// ; inotdpn
inline %0DBh

  add     x19, x29, __arg12_1

  ldrsw   x18, [x0]

  mvn     x17, x18  

  str     w17, [x19]

end

// ; inotdpn
inline %1DBh

  add     x19, x29, __arg12_1

  ldrsw   x18, [x0]

  mvn     x17, x18  

  strb    w17, [x19]

end

// ; inotdpn
inline %2DBh

  add     x19, x29, __arg12_1

  ldrsw   x18, [x0]

  mvn     x17, x18  

  strh    w17, [x19]

end

// ; inotdpn
inline %4DBh

  add     x19, x29, __arg12_1

  ldrsw   x18, [x0]

  mvn     x17, x18  

  str     x17, [x19]

end

// ; ishldpn
inline %0DCh

  add     x19, x29, __arg12_1

  ldrsw   x17, [x0]
  ldrsw   x18, [x19]

  lsl     x18, x18, x17

  str     w18, [x19]

end

// ; ishldpn
inline %1DCh

  add     x19, x29, __arg12_1

  ldrsw   x17, [x0]
  ldrsw   x18, [x19]

  lsl     x18, x18, x17

  strb    w18, [x19]

end

// ; ishldpn
inline %2DCh

  add     x19, x29, __arg12_1

  ldrsw   x17, [x0]
  ldrsw   x18, [x19]

  lsl     x18, x18, x17

  strh    w18, [x19]

end

// ; ishldpn
inline %4DCh

  add     x19, x29, __arg12_1

  ldrsw   x17, [x0]
  ldr     x18, [x19]

  lsl     x18, x18, x17

  str     x18, [x19]

end

// ; ishrdpn
inline %0DDh

  add     x19, x29, __arg12_1

  ldrsw   x17, [x0]
  ldrsw   x18, [x19]

  lsr     x18, x18, x17

  str     w18, [x19]

end

// ; ishrdpn
inline %1DDh

  add     x19, x29, __arg12_1

  ldrsw   x17, [x0]
  ldrsw   x18, [x19]

  lsr     x18, x18, x17

  strb    w18, [x19]

end

// ; ishrdpn
inline %2DDh

  add     x19, x29, __arg12_1

  ldrsw   x17, [x0]
  ldrsw   x18, [x19]

  lsr     x18, x18, x17

  strh    w18, [x19]

end

// ; ishrdpn
inline %4DDh

  add     x19, x29, __arg12_1

  ldrsw   x17, [x0]
  ldr     x18, [x19]

  lsr     x18, x18, x17

  str     x18, [x19]

end

// ; selultrr
inline %0DFh

  ldrsw   x17, [x0]
  ldrsw   x18, [x10]
  cmp     x17, x18

  movz    x11,  __ptr32lo_1
  movz    x12,  __ptr32lo_2
  movk    x11,  __ptr32hi_1, lsl #16
  movk    x12,  __ptr32hi_2, lsl #16

  csel    x10, x11, x12, cc

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

// ; copydpn dpn, 1
inline %1E0h

  add     x13, x29, __arg12_1
  ldr     x14, [x0]
  strb    w14, [x13]

end

// ; copydpn dpn, 2
inline %2E0h

  add     x13, x29, __arg12_1
  ldr     x14, [x0]
  strh    w14, [x13]

end

// ; copydpn dpn, 4
inline %3E0h

  add     x13, x29, __arg12_1
  ldr     x14, [x0]
  str     w14, [x13]

end

// ; copydpn dpn, 8
inline %4E0h

  add     x13, x29, __arg12_1
  ldr     x14, [x0]
  str     x14, [x13]

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
  sxth    x18, x18
  str     w18, [x19]

end

// ; nsavedpn (n > 0FFFFh)
inline %01E5h

  add     x19, x29, __arg12_1
  movz    x18,  __n16_2
  movk    x18,  __n16hi_2, lsl #16
  str     w18, [x19]

end

// ; xhookdpr
inline %0E6h

  add     x13, x29, __arg12_1

  movz    x14,  data_ptr32lo : %CORE_SINGLE_CONTENT
  movk    x14,  data_ptr32hi : %CORE_SINGLE_CONTENT, lsl #16
  mov     x18, x13

  movz    x16,  __ptr32lo_2
  movk    x16,  __ptr32hi_2, lsl #16
  add     x14, x14, # et_current

  mov     x17, sp
  ldr     x15, [x14]

  str     x15, [x13]
  str     x16, [x13, #8]!
  str     x17, [x13, #8]!
  str     x29, [x13, #8]!
  str     x18, [x14]

end

// ; xnewnr i, r
inline %0E7h

  add     x10, x10, elObjectOffset
  movz    x18, __n16lo_1
  movk    x18, __n16hi_1, lsl #16
  movz    x19,  __ptr32lo_2
  movk    x19,  __ptr32hi_2, lsl #16
  sub     x20, x10, elVMTOffset
  str     x19, [x20]
  str     w18, [x20, #12]!

end

// ; nsavedpn
inline %0E8h

  add     x19, x29, __arg12_1
  mov     x18, __n16_2
  ldr     w20, [x19]
  add     x20, x20, x18
  str     w20, [x19]

end

// ; dcopydpn
inline %0E9h

  mov     x11, __n16_2
  mov     x12, x0
  mul     x11, x11, x9
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

// ; xwriteon
inline %0EAh

  mov     x11, __n16_2
  mov     x14, __n16_1
  mov     x13, x0
  add     x12, x10, x14

labLoop:
  cmp     x11, 0
  beq     labEnd
  sub     x11, x11, 1
  ldrb    w14, [x12], #1
  strb    w14, [x13], #1
  b       labLoop

labEnd:

end

// ; xcopyon
inline %0EBh

  mov     x11, __n16_2
  mov     x14, __n16_1
  mov     x12, x0
  add     x13, x10, x14

labLoop:
  cmp     x11, 0
  beq     labEnd
  sub     x11, x11, 1
  ldrb    w14, [x12], #1
  strb    w14, [x13], #1
  b       labLoop

labEnd:

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
// ; NOTE : arg2 < 0
inline %0F3h

  sub     x12, x29, -__arg12_2
  add     x13, sp, __arg12_1

  ldr     x11, [x12]
  str     x11, [x13]

end

// ; movsifi sp:0, fp:i2
// ; NOTE : arg2 < 0
inline %1F3h

  sub     x12, x29, -__arg12_2
  ldr     x0, [x12]

end

// ; movsifi sp:1, fp:i2
// ; NOTE : arg2 < 0
inline %2F3h

  sub     x12, x29, -__arg12_2
  ldr     x1, [x12]

end

// ; movsifi
// ; NOTE : arg2 > 0
inline %5F3h

  add     x12, x29, __arg12_2
  add     x13, sp, __arg12_1

  ldr     x11, [x12]
  str     x11, [x13]

end

// ; movsifi sp:0, fp:i2
// ; NOTE : arg2 > 0
inline %6F3h

  add     x12, x29, __arg12_2
  ldr     x0, [x12]

end

// ; movsifi sp:1, fp:i2
// ; NOTE : arg2 > 0
inline %7F3h

  add     x12, x29, __arg12_2
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
  movz    x18, __n16lo_1
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
  add     x19, x19, page_ceil
  and     x11, x19, page_mask

  movz    x17,  code_ptr32lo : %GC_ALLOC
  movk    x17,  code_ptr32hi : %GC_ALLOC, lsl #16
  blr     x17

  ldr     w19, [x0]
  movz    x18, __n16_1
  mul     x18, x19, x18

  // ; adding mask
  movz    x19, struct_mask_lo
  movk    x19, struct_mask_hi, lsl #16
  orr     x18, x18, x19

  movz    x19,  __ptr32lo_2
  movk    x19,  __ptr32hi_2, lsl #16
  sub     x20, x10, elVMTOffset
  str     x19, [x20]
  str     w18, [x20, #12]!

end

// ; fillir
inline %0F8h

  movz    x12,  __ptr32lo_2
  movk    x12,  __ptr32hi_2, lsl #16

  mov     x11, __arg12_1
  mov     x13, x10

labLoop:
  cmp     x11, 0
  beq     labEnd
  sub     x11, x11, 1
  str     x12, [x13], #8
  b       labLoop

labEnd:

end

// ; fill i,0
inline %1F8h

  mov     x11, __arg12_1
  mov     x12, 0
  mov     x13, x10

labLoop:
  cmp     x11, 0
  beq     labEnd
  sub     x11, x11, 1
  str     x12, [x13], #8
  b       labLoop

labEnd:

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
  str     x0, [sp]
//;  lea  rax, [rsp + __n_2]
  add     x17, sp, __n12_2
//;  mov  [rsp+16], r11                     // ; saving arg1
  str     x1, [sp, #8]

  sub     x17, x17, #8                      // ; HOTFIX : caller address is not in the stack

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
  movz    x20,  rdata_ptr32lo : %VOIDPTR
  movk    x20,  rdata_ptr32hi : %VOIDPTR, lsl #16
  add     x20, x20, elObjectOffset

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

// ; xdispatchmr
// ; NOTE : __arg32_1 - message; __n_1 - arg count; __ptr32_2 - list, __n_2 - argument list offset
inline % 5FAh

//;  mov  [rsp+8], r10                      // ; saving arg0
  str     x0, [sp]
//;  lea  rax, [rsp + __n_2]
  add     x17, sp, __n12_2
//;  mov  [rsp+16], r11                     // ; saving arg1
  str     x1, [sp, #8]

  sub     x17, x17, #8                      // ; HOTFIX : caller address is not in the stack

//;  mov  rsi, __ptr64_2
  movz    x21,  __ptr32lo_2
  movk    x21,  __ptr32hi_2, lsl #16

//;  xor  edx, edx
  mov     x25, #0
  mov     x11, #0

  // ; count the number of args
  mov     x22, x17
  mov     x23, #0
  sub     x23, x23, #1                        // ; define the terminator
labCountParam:
  add     x22, x22, #8
  ldr     x24, [x22]
  add     x16, x16, 1
  cmp     x24, x23
  bne     labCountParam
  mov     x17, x11

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
  mov     x16, x17

//;  lea  rbx, [r13 - 8]
  sub     x22, x23, #8

labNextParam:
//;  sub  ecx, 1
  sub     x16, x16, #1

//;  jnz  short labMatching
  cmp     x16, x17
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
  add     x20, x26, #8
  ldr     x18, [x20]
  cmp     x18, #0
  csel    x26, x26, x20, eq

//;  mov  rdi, [rax + rcx * 8]
  lsl     x19, x16, #3
  add     x18, x19, x17 
  ldr     x18, [x18]

  //; check nil
//;  mov   rsi, rdata : %VOIDPTR + elObjectOffset
  movz    x20,  rdata_ptr32lo : %VOIDPTR
  movk    x20,  rdata_ptr32hi : %VOIDPTR, lsl #16
  add     x20, x20, elObjectOffset

//;  test  rdi, rdi                                              
  cmp     x18, #0

//;  cmovz rdi, rsi
  csel   x18, x20, x18, eq

//;  mov  rdi, [rdi - elVMTOffset]
  sub     x18, x18, elVMTOffset
  ldr     x18, [x18, #0]

//;  mov  rsi, [rbx + rcx * 8]
  ldr     x20, [x26]

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

  str     x0, [sp]                          // ; saving arg0
//;  lea  rax, [rsp + __n_2]
  add     x17, sp, __n12_2
//;  mov  [rsp+16], r11                     // ; saving arg0
  str     x1, [sp, #8]

  sub     x17, x17, #8                      // ; HOTFIX : caller address is not in the stack

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

  lsl     x23, x25, #4
  add     x25, x21, x23
  ldr     x9,  [x25]
  ldr     x23, [x25, #8] 

  sub     x16, x10, elVMTOffset
  ldr     x16, [x16, #0]

//;  jmp  [rcx + rax + 8]       // rax - 0
  add     x20, x16, x23

  ldr     x17, [x20, #8]
  br      x17

labMatching:
//;  mov  rdi, [rax + rcx * 8]
  lsl     x19, x16, #3
  add     x18, x19, x17 
  ldr     x18, [x18]

  //; check nil
//;  mov   rsi, rdata : %VOIDPTR + elObjectOffset
  movz    x20,  rdata_ptr32lo : %VOIDPTR
  movk    x20,  rdata_ptr32hi : %VOIDPTR, lsl #16
  add     x20, x20, elObjectOffset

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
inline % 5FBh

  str     x0, [sp]                          // ; saving arg0
//;  lea  rax, [rsp + __n_2]
  add     x17, sp, __n12_2
//;  mov  [rsp+16], r11                     // ; saving arg0
  str     x1, [sp, #8]

  sub     x17, x17, #8                      // ; HOTFIX : caller address is not in the stack

//;  mov  rsi, __ptr64_2
  movz    x21,  __ptr32lo_2
  movk    x21,  __ptr32hi_2, lsl #16

//;  xor  edx, edx
  mov     x25, #0
  mov     x11, #0

  // ; count the number of args
  mov     x22, x17
  mov     x23, #0
  sub     x23, x23, #1                        // ; define the terminator
labCountParam:
  add     x22, x22, #8
  ldr     x24, [x22]
  add     x16, x16, 1
  cmp     x24, x23
  bne     labCountParam
  mov     x17, x11

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
  add     x16, x16, #1

//;  jnz  short labMatching
  cmp     x16, x17
  bne     labMatching

//;  mov  r9, __ptr64_2  - r21

  lsl     x23, x25, #4
  add     x25, x21, x23
  ldr     x9,  [x25]
  ldr     x23, [x25, #8] 

  sub     x16, x10, elVMTOffset
  ldr     x16, [x16, #0]

//;  jmp  [rcx + rax + 8]       // rax - 0
  add     x20, x16, x23

  ldr     x17, [x20, #8]
  br      x17

labMatching:
  add     x20, x26, #8
  ldr     x18, [x20]
  cmp     x18, #0
  csel    x26, x26, x20, eq

//;  mov  rdi, [rax + rcx * 8]
  lsl     x19, x16, #3
  add     x18, x19, x17 
  ldr     x18, [x18]

  //; check nil
//;  mov   rsi, rdata : %VOIDPTR + elObjectOffset
  movz    x20,  rdata_ptr32lo : %VOIDPTR
  movk    x20,  rdata_ptr32hi : %VOIDPTR, lsl #16
  add     x20, x20, elObjectOffset

//;  test  rdi, rdi                                              
  cmp     x18, #0

//;  cmovz rdi, rsi
  csel   x18, x20, x18, eq

//;  mov  rdi, [rdi - elVMTOffset]
  sub     x18, x18, elVMTOffset
  ldr     x18, [x18, #0]

//;  mov  rsi, [rbx + rcx * 8]
  ldr     x20, [x26]

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

  mov     x18, __arg16_1
  sub     x14, x10, elVMTOffset              
  ldr     x17, [x14]
  add     x17, x17, x18
  ldr     x17, [x17, #8]
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
  mov     x9, x0

end
