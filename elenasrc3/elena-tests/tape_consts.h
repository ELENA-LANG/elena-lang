#ifdef _M_IX86

constexpr auto B_NillableIntAssigning = "tape (open_frame ()assigning 1 ()nil ()assigning 2 ()int_literal 2 (value 2 ())assigning 3 ()local 1 ()close_frame ()exit ())reserved 4 ())";

#elif _M_X64

constexpr auto B_NillableIntAssigning = "tape (open_frame ()assigning 1 ()nil ()assigning 2 ()int_literal 2 (value 2 ())assigning 3 ()local 1 ()close_frame ()exit ())reserved 6 ())";

#endif
