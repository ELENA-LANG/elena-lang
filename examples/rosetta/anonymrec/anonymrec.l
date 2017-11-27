/// Anonymous recursion

import extensions.

extension mathOp
{
    fib
    [
        if (self < 0)
            [ InvalidArgumentException new:"Must be non negative"; raise ].
            
        ^ control do:self with
            (:n)
                [
                    if (n > 1)
                        [ ^ @self(n - 2) + @self(n - 1) ];
                        [ ^ n ]
                ].
    ]
}

program =
[
    -1 to:10 do(:i)
    [
        console print("fib(",i,")=").
        
        try (console printLine(i fib))        
        {
            on(Exception e)
            [
                console printLine:"invalid"
            ]
        }.
    ].
    
    console readChar.
].
