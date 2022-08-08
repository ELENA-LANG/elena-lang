// ; ==== System commands ===

// ; ==== Overridden Command Set ==

// ; --- Predefined References  --
define INVOKER         10001h
define VEH_HANDLER     10003h

define CORE_ET_TABLE   2000Bh

// ; ==== System commands ===

// INVOKER(function, arg)
procedure % INVOKER

  // ; x0 - function
  // ; x1 - arg

  stp     x19, x20, [sp, #-16]! 
  stp     x21, x22, [sp, #-16]! 
  stp     x23, x24, [sp, #-16]! 
  stp     x25, x26, [sp, #-16]! 
  stp     x27, x28, [sp, #-16]! 
  stp     x29, x30, [sp, #-16]! 

  mov     x8, x0
  mov     x0, x1

  blr     x8

  ldp     x29, x30, [sp], #16 
  ldp     x27, x28, [sp], #16 
  ldp     x25, x26, [sp], #16 
  ldp     x23, x24, [sp], #16 
  ldp     x21, x22, [sp], #16 
  ldp     x19, x20, [sp], #16 

  ret     x30

end

// VEH_HANDLER() 
procedure % VEH_HANDLER

  stp     x29, x30, [sp, #-16]! 
  mov     x29, sp

  movz    x20,  data_ptr32lo : %CORE_ET_TABLE
  movk    x20,  data_ptr32hi : %CORE_ET_TABLE, lsl #16

  ldr     x17, [x20]
  br      x17

end
