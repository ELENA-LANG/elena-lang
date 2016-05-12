///  a program to find the mean (arithmetic average) of a numeric vector. 
#import system.
#import extensions.

// --- Average ---

#class(extension)op
{
    #method average
    [
        #var aSum := Real new.
        #var aCount := Integer new:0.
        
        #var anEnumerator := self enumerator.
        
        #loop (anEnumerator next)?
        [
            aSum += anEnumerator get.
            aCount += 1.
        ].
        
        ^ aSum / aCount.
    ]
}
// --- Program ---

#symbol program =
[
    #var anArray := (1, 2, 3, 4, 5, 6, 7, 8).
    console writeLine:"Arithmetic mean of {":anArray:"} is ":(anArray average) readChar.
]. 