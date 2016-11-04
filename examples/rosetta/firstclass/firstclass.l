// Write a program to create an ordered collection A of functions of a real number. At least one function should be built-in and 
// at least one should be user-defined; try using the sine, cosine, and cubing functions. 
// Fill another collection B with the inverse of each function in A. 
// Implement function composition as in Functional Composition. Finally, demonstrate that the result of applying the composition 
// of each function in A and its inverse in B to a value, is the original value

#import system.
#import system'routines.
#import system'math.
#import extensions'routines.
 
#class(extension)op
{
    #method compose : f : g
        = (self::g eval)::f eval.
}
 
#symbol program =
[
   #var fs := (%"mathOp.sin[0]", %"mathOp.cos[0]", ()[ this power:3.0r ]).
   #var gs := (%"mathOp.arcsin[0]", %"mathOp.arccos[0]", ()[ this power:(1.0r / 3) ]).
 
   fs zip:gs &into: (:f:g)[ 0.5r compose:f:g ]
        run &each:printingLn.
].