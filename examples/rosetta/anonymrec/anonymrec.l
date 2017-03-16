/// Anonymous recursion

#import extensions.

extension mathOp
{
    fib
    [
        (self < 0)
            ? [ InvalidArgumentException new &message:"Must be non negative" raise. ].
            
        ^ control eval:self &for: 
            (:n)
                [
                    (n > 1) ? [ this eval:(n - 2) + this eval:(n - 1) ] ! [ n ]
                ].
    ]
}

program =
[
    -1 to:10 &doEach: (:i)
    [
        console writeLiteral:"fib(":i:")=".
        
        console writeLine:(i fib) | if &InvalidArgumentError: e
            [
                console writeLine:"invalid".
            ].
    ].
    
    console readChar.
].
