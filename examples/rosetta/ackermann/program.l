// Ackermann function
//          | n + 1,               if m = 0
// A(m,n) = | A (m-1, 1),          if n = 0
//          | A (m-1, A(m, n - 1)) if m > 0 and n > 0

// A( 0 , 4 ) =  5
// A( 1 , 4 ) =  6
// A( 2 , 4 ) =  11
// A( 3 , 4 ) =  125

#import extensions.

// --- Ackermann function ---

ackermann = (:m:n)
[
    m =>
       0 [ n + 1 ];
       > 0 [
              n => 
                0 [ ackermann eval:(m - 1):1 ];
                > 0 [ ackermann eval:(m - 1):(ackermann eval:m:(n-1)) ]
           ]
].

program =
[
    0 to:3 &doEach: (:i)
    [
        0 to:5 &doEach: (:j)
        [
            console writeLine:"A(":i:",":j:")=":(ackermann eval:i:j).
        ].
    ].

    console readChar.
].