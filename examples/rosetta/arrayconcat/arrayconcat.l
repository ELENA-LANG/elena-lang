/// concatenating two arrays in your language. 
#import system.

#import extensions.

#symbol program =
[
    #var a := (1,2,3).
    #var b := (4,5).

    console writeLine:"(":a:") + (":b:") = (":(a + b):")" readChar.
].