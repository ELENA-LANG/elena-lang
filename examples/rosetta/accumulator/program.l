/*
A problem posed by Paul Graham is that of creating a function that takes a single 
(numeric) argument and which returns another function that is an accumulator. 
The returned accumulator function in turn also takes a single numeric argument, 
and returns the sum of all the numeric values passed in so far to that accumulator 
(including the initial value passed when the accumulator was created).
*/

/*
ELENA is OO language, so a function is in fact an object
*/

#import system.
#import system'dynamic.

#symbol Function = 
    (:x) [ this append:x ].

#class(extension)op
{
    #method Accumulator
        = Variable new:self mix &into:Function.
}

#symbol program =
[
    #var x := 1 Accumulator.
    
    x eval:5.
    
    #var y := 3 Accumulator.
    
    console write:(x eval:2.3r).
].
