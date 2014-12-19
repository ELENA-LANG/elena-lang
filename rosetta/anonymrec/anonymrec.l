#define system.

#symbol fibo = (:n)
[
    n < 0
        ? [ #throw InvalidArgumentException new &message:"Must be non negative". ].
 
    ^ { eval:n [ ^ (n > 1) ? [ ($self:(n - 2)) + ($self:(n - 1)) ] ! [ n ]. ] }:n.
].

#symbol program =
[
    control runRange &int:-1 &int:10 &forEvery: (&index:i)
    [
        console << "fib(" << i << ")=".
        
        console writeLine:(fibo:i) | if &InvalidArgumentError: e
            [
                console writeLine:"invalid".
            ].
    ].
    
    console readChar.
].
