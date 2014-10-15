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

#define system.
#define system'dynamic.

#symbol Function = 
    (:x) [ self append:x ].

#symbol Accumulator = (:anInitialValue)
    [ Extension(Function, Variable new:anInitialValue) ].

#symbol Program =
[
    #var x := Accumulator:1.
    
    x:5.
    
    #var y := Accumulator:3.
    
    console write:(x:2.3r).
].
