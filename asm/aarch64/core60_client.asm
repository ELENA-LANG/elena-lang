// ; --- Predefined References  --
define GC_ALLOC	             10002h
define VEH_HANDLER           10003h
define GC_COLLECT	     10004h
define GC_ALLOCPERM	     10005h
define PREPARE	             10006h
define THREAD_WAIT          10007h

define CORE_TOC              20001h
define SYSTEM_ENV            20002h
define CORE_GC_TABLE         20003h
define CORE_MATH_TABLE       20004h
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
define tt_stack_root         0028h

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

// ; system startup
inline %4CFh

  mov     x12, sp

  movz    x14,  data_ptr32lo : %CORE_SINGLE_CONTENT
  movk    x14,  data_ptr32hi : %CORE_SINGLE_CONTENT, lsl #16
  add     x14, x14, # tt_stack_root
  str     x12, [x14]

end
