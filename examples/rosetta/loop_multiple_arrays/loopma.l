// Loop over multiple arrays (or lists or tuples or whatever they're called in your language) and display the <i>th element of each. 

import system'routines.
import extensions.

program = 
[  
    var a1 := ("a","b","c").
    var a2 := ("A","B","C").
    var a3 := (1,2,3).
    
    0 till(a1 length) do(:i)
    [
        console printLine(a1[i], a2[i], a3[i]).
    ].
    
    console readChar.    
].

// To build the program to use 'programUsingZip' symbol, just change
// the forward to use programUsingZip as entry point
// --
// 'program=loopma'programUsingZip
// --
programUsingZip =
[
    var a1 := ("a","b","c").
    var a2 := ("A","B","C").
    var a3 := (1,2,3).
    var zipped := a1 zip:a2 by(:first:second)( first + second );
                       zip:a3 by(:first:second)( first + second literal ).
    
    zipped forEach(:e)
        [ console writeLine:e. ].
        
    console readChar.            
].