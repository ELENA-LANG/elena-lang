#define system.

#symbol fibo = &&:n
[
    n < 0
        ? [ #throw InvalidArgumentException new:"Must be non negative". ].
 
    ^ &&type'int:n [ (n > 1) ? [ $self eval &type'int:(n - 2) + $self eval &type'int:(n - 1) ] ! [ n ] ] eval &type'int::n.
].

#symbol program =
[
    control from:-1 &to:10 &do: &&:i
    [
        console << "fib(" << i << ")=".
        
        console writeLine:(fibo:i) | onInvalidArgumentError: &&:e
            [
                console writeLine:"invalid".
            ].
    ].
    
    console readChar.
].
