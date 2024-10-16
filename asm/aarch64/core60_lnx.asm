// ; ==== System commands ===

// ; ==== Overridden Command Set ==

// ; --- Predefined References  --
define VEH_HANDLER     10003h

define CORE_ET_TABLE   2000Bh

// ; ==== System commands ===

// VEH_HANDLER() 
procedure % VEH_HANDLER

  mov     x17, x0
  mov     x0, x9
  mov     x9, x17

  movz    x20,  data_ptr32lo : %CORE_ET_TABLE
  movk    x20,  data_ptr32hi : %CORE_ET_TABLE, lsl #16

  ldr     x17, [x20]
  br      x17

end
