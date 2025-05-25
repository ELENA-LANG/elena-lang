define CORE_SINGLE_CONTENT  2000Bh

define tt_stack_frame        0008h
define tt_stack_root         0014h

// ; system startup
inline %4CFh

  mov  [data : %CORE_SINGLE_CONTENT + tt_stack_root], esp

end
