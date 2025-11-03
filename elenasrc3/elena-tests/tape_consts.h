#ifdef _M_IX86

constexpr auto B_NillableIntAssigning = "tape (open_frame ()assigning 1 ()nil ()assigning 2 ()int_literal 2 (value 2 ())assigning 3 ()local 1 ()close_frame ()exit ())reserved 4 ())";
constexpr auto B_IntermediateVar = "tape (open_frame ()assigning 1 ()symbol_call_op 2 ()assigning 2 ()local 2 ()saving_stack ()argument ()call_op 2049 ()final_op -4 (stack_index 3 ()tape ()tape (local 2 ()saving_stack ()argument ()call_op 2561 ()))local 1 ()close_frame ()exit ())reserved 4 ()reserved_n 16 ())";

#elif _M_X64

constexpr auto B_NillableIntAssigning = "tape (open_frame ()assigning 1 ()nil ()assigning 2 ()int_literal 2 (value 2 ())assigning 3 ()local 1 ()close_frame ()exit ())reserved 6 ())";
constexpr auto B_IntermediateVar = "tape (open_frame ()assigning 1 ()symbol_call_op 2 ()assigning 2 ()local 2 ()saving_stack ()argument ()call_op 2049 ()final_op -8 (stack_index 3 ()tape ()tape (local 2 ()saving_stack ()argument ()call_op 2561 ()))local 1 ()close_frame ()exit ())reserved 6 ()reserved_n 32 ())";

#endif
