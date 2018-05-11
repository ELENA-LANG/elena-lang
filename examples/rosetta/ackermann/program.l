// Ackermann function
//          | n + 1,               if m = 0
// A(m,n) = | A (m-1, 1),          if n = 0
//          | A (m-1, A(m, n - 1)) if m > 0 and n > 0

// A( 0 , 4 ) =  5
// A( 1 , 4 ) =  6
// A( 2 , 4 ) =  11
// A( 3 , 4 ) =  125

import extensions.

// --- Ackermann function ---

ackermann = (:m:n)
[
    if((n < 0)||(m < 0))
    [
        InvalidArgumentException new; raise
    ].
    
    m =>
       0 [ ^n + 1 ];
       ! [
              n => 
                0 [ ^ackermann(m - 1,1) ];
                ! [ ^ackermann(m - 1,ackermann(m,n-1)) ]
           ]
].

public program =
[
    0 to:3 do(:i)
    [
        0 to:5 do(:j)
        [
            console printLine("A(",i,",",j,")=",ackermann(i,j))
        ]
    ].

    console readChar.
].