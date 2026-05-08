#ifdef _M_IX86

constexpr auto B_NillableIntAssigning = "tape (open_frame ()assigning 1 ()nil ()assigning 2 ()int_literal 2 (value 2 ())assigning 3 ()local 1 ()close_frame ()exit ())reserved 4 ())";
constexpr auto B_IntermediateVar = "tape (open_frame ()assigning 1 ()symbol_call_op 2 ()assigning 2 ()local 2 ()saving_stack ()argument ()call_op 2049 ()final_op -4 (stack_index 3 ()tape ()tape (local 2 ()saving_stack ()argument ()call_op 2561 ()))local 1 ()close_frame ()exit ())reserved 4 ()reserved_n 16 ())";

constexpr auto BuildTree_StructFieldAssigning_1 = "tape (open_frame ()assigning 1 () int_literal 2 (value 3 ()) copying -4 (size 4 ()) local_address -8 ()saving_stack ()local 1 ()copying_acc_field 4 (size 4 ())local_address -8 ()saving_stack ()local_address -4 ()saving_stack 1 ()intcondop 8 (operator_id 8() true_const 10() false_const 11 ()) assigning 2() local 2 () branchop 7 ( const_param 10 () tape (local_address -4 ()saving_stack ()local 1 ()copying_to_acc_field 4 (size 4 ())) ) local 1() close_frame () exit ()) reserved 4 () reserved_n 8 ()";
constexpr auto OptimizedTree_StructFieldAssigning_1 = "tape (open_frame ()assigning 1 ()saving_int -4 (size 4 ()value 3 ())local_address -8 ()saving_stack ()local 1 ()copying_acc_field 4 (size 4 ())local_address -8 ()saving_stack ()local_address -4 ()saving_stack 1 ()intbranchop 8 (const_param 10 ()tape (local_address -4 ()saving_stack ()local 1 ()copying_to_acc_field 4 (size 4 ())))local 1 ()close_frame () exit ()) reserved 4 () reserved_n 8 ()";

constexpr auto B_IntValueOperator = "tape (open_frame ()assigning 1 ()class_reference 3 ()direct_call_op 544 (type 7 ())inplacemark 3841 () copying -4 (size 4 ()) local_address -8 ()saving_stack ()local_address -4 ()copying_acc_field (size 4 ())local 1 ()close_frame () exit ()) reserved 2 () reserved_n 8 ())";
constexpr auto B_IntValueOperator2 = "tape (open_frame ()assigning 1 ()class_reference 3 ()direct_call_op 544 (type 7 ())inplacemark 3841 () copying -4 (size 4 ()) local_address -8 ()saving_stack 1 ()local_address -4 ()saving_stack ()argument ()direct_call_op 54146 (type 79 ())local_address -8 ()local 1 ()close_frame ()exit ())reserved 3 ()reserved_n 12 ()))";

#elif _M_X64

constexpr auto B_NillableIntAssigning = "tape (open_frame ()assigning 1 ()nil ()assigning 2 ()int_literal 2 (value 2 ())assigning 3 ()local 1 ()close_frame ()exit ())reserved 6 ())";
constexpr auto B_IntermediateVar = "tape (open_frame ()assigning 1 ()symbol_call_op 2 ()assigning 2 ()local 2 ()saving_stack ()argument ()call_op 2049 ()final_op -8 (stack_index 3 ()tape ()tape (local 2 ()saving_stack ()argument ()call_op 2561 ()))local 1 ()close_frame ()exit ())reserved 6 ()reserved_n 32 ())";

constexpr auto BuildTree_StructFieldAssigning_1 = "tape (open_frame ()assigning 1 () int_literal 2 (value 3 ()) copying -8 (size 4 ()) local_address -24 ()saving_stack ()local 1 ()copying_acc_field 4 (size 4 ())local_address -24 ()saving_stack ()local_address -8 ()saving_stack 1 ()intcondop 8 (operator_id 8() true_const 10() false_const 11 ()) assigning 2() local 2 () branchop 7 ( const_param 10 () tape (local_address -8 ()saving_stack ()local 1 ()copying_to_acc_field 4 (size 4 ())) ) local 1() close_frame () exit ()) reserved 4 () reserved_n 32 ()";
constexpr auto OptimizedTree_StructFieldAssigning_1 = "tape (open_frame ()assigning 1 ()saving_int -8 (size 4 ()value 3 ())local_address -24 ()saving_stack ()local 1 ()copying_acc_field 4 (size 4 ())local_address -24 ()saving_stack ()local_address -8 ()saving_stack 1 ()intbranchop 8 (const_param 10 ()tape (local_address -8 ()saving_stack ()local 1 ()copying_to_acc_field 4 (size 4 ())))local 1 ()close_frame () exit ()) reserved 4 () reserved_n 32 ()";

constexpr auto B_IntValueOperator = "tape (open_frame ()assigning 1 ()class_reference 3 ()direct_call_op 544 (type 7 ())inplacemark 3841 () copying -8 (size 8 ()) local_address -24 ()saving_stack ()local_address -8 ()copying_acc_field (size 4 ())local 1 ()close_frame () exit ()) reserved 4 () reserved_n 32 ())";
constexpr auto B_IntValueOperator2 = "tape (open_frame ()assigning 1 ()class_reference 3 ()direct_call_op 544 (type 7 ())inplacemark 3841 () copying -4 (size 4 ()) local_address -8 ()saving_stack 1 ()local_address -4 ()saving_stack ()argument ()direct_call_op 54146 (type 79 ())local_address -8 ()local 1 ()close_frame ()exit ())reserved 3 ()reserved_n 12 ()))";

#endif
