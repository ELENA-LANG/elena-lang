#import system.
#import system'routines.

#symbol program = 
[  
    #var a1 := ("a","b","c").
    #var a2 := ("A","B","C").
    #var a3 := (1,2,3).
    
    #var i := Integer new:0.
    #loop (i < a1 length)?
    [
        console writeLine:(a1@i + a2@i + (a3@i) literal).
        i := i + 1.
    ].
    
    console readChar.    
].

// To build the program to use 'programUsingZip' symbol, just change
// the forward to use programUsingZip as entry point
// --
// 'program=loopma'programUsingZip
// --
#symbol programUsingZip =
[
    #var a1 := ("a","b","c").
    #var a2 := ("A","B","C").
    #var a3 := (1,2,3).
    #var zipped := (a1 zip: a2 &into:(:first:second) [ first + second ]) 
                       zip: a3 &into:(:first:second) [ first + (second literal)].
    
    zipped run &each: e 
        [ console writeLine:e. ].
        
    console readChar.            
].