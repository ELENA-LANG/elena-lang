#import system.

#symbol program =
[
    #var s := "hello".
    console write:s.
    console writeLine:" literal".
    
    #var s1 := s + " literal".
    console writeLine:s1.
].