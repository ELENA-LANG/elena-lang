// Ackermann function
//          | n + 1,               if m = 0
// A(m,n) = | A (m-1, 1),          if n = 0
//          | A (m-1, A(m, n - 1)) if m > 0 and n > 0

// A( 0 , 4 ) =  5
// A( 1 , 4 ) =  6
// A( 2 , 4 ) =  11
// A( 3 , 4 ) =  125

import extensions;

// --- Ackermann function ---

ackermann(m,n)
{
    if(n < 0 || m < 0)
    {
        InvalidArgumentException.raise()
    };
    
    m =>
       0 { ^n + 1 }
       : {
              n => 
                0 { ^ackermann(m - 1,1) }
                : { ^ackermann(m - 1,ackermann(m,n-1)) }
           }
}

public program()
{
    for(int i:=0, i <= 3, i += 1)
    {
        for(int j := 0, j <= 5, j += 1)
        {
            console.printLine("A(",i,",",j,")=",ackermann(i,j))
        }
    };

    console.readChar()
}