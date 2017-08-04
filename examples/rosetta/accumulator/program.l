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

import system'dynamic.

Function = 
    (:x)(closure append:x).

extension op
{
    accumulator of:func
        = Variable new:self; mix into:func.
}

program =
[
    var x := 1 accumulator of:Function.
    
    x eval:5.
    
    var y := 3 accumulator of:Function.
    
    console write(x eval:2.3r).
].
