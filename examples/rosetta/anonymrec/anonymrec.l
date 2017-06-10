/// Anonymous recursion

import extensions.

extension mathOp
{
    fib
    [
        if (self < 0)
            [ InvalidArgumentException new:"Must be non negative"; raise ].
            
        ^ control eval:self for: 
            (:n)
                [
                    if (n > 1)
                        [ ^ $closure eval:(n - 2) + $closure eval:(n - 1) ];
                        [ ^ n ]
                ].
    ]
}

program =
[
    -1 to:10 doEach: (:i)
    [
        console print:"fib(":i:")=".
        
        try (console printLine:(i fib))        
        {
            InvalidArgumentError: e
            [
                console printLine:"invalid"
            ]
        }.
    ].
    
    console readChar.
].
