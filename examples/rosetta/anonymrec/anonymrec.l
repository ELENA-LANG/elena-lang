/// Anonymous recursion

#import extensions.

extension mathOp
{
    fib
    [
        if (self < 0)
            [ InvalidArgumentException new &message:"Must be non negative" raise. ].
            
        ^ control eval:self &for: 
            (:n)
                [
                    if (n > 1)
                        [ ^ $closure eval:(n - 2) + $closure eval:(n - 1) ];
                        [ ^ n ].
                ].
    ]
}

program =
[
    -1 to:10 &doEach: (:i)
    [
        console writeLiteral:"fib(":i:")=".
        
        try (console writeLine:(i fib))        
        {
            InvalidArgumentError: e
            [
                console writeLine:"invalid".
            ]
        }.
    ].
    
    console readChar.
].
