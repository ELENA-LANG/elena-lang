// ; ==== System commands ===

// ; ==== Overridden Command Set ==

// ; --- Predefined References  --
define INVOKER         10001h

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
