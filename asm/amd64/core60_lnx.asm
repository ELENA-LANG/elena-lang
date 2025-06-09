// ; --- Predefined References  --
define GC_ALLOC	            10002h
define VEH_HANDLER          10003h
define GC_COLLECT	    10004h
define PREPARE	            10006h

define SYSTEM_ENV           20002h
define CORE_GC_TABLE        20003h
define CORE_SINGLE_CONTENT  2000Bh

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

// ; --- Object header fields ---
define elSizeOffset          0004h
define elVMTOffset           0010h 
define elObjectOffset        0010h

// ; ==== System commands ===

// ; ==== Overridden Command Set ==
