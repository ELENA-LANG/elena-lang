// Ackermann function
//          | n + 1,               if m = 0
// A(m,n) = | A (m-1, 1),          if n = 0
//          | A (m-1, A(m, n - 1)) if m > 0 and n > 0

// A( 0 , 4 ) =  5
// A( 1 , 4 ) =  6
// A( 2 , 4 ) =  11
// A( 3 , 4 ) =  125

#define system.

// --- Ackermann function ---

#symbol ackermann = &&:m:n
[
    m =>
       0 ? [ n + 1 ]
       > 0 ? [
              n => 0 ? [ $self:(m - 1):1 ] 
                       > 0 ? [ $self:(m - 1):($self:m:(n-1)) ]   
           ]
].

#symbol program =
[
    control from:0 &to:3 &do: &&:i
    [
        control from:0 &to:5 &do: &&:j
        [
            console << "A(" << i << "," << j << ")=" << (ackermann:i:j).
            
            console writeLine.
        ].
    ].

    console readChar.
].