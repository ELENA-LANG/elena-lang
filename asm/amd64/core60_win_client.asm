define CORE_SINGLE_CONTENT  2000Bh

define tt_stack_frame        0010h
define tt_stack_root         0028h

// ; system startup
inline %4CFh

  mov  [data : %CORE_SINGLE_CONTENT + tt_stack_root], rsp

end
