// Write a program to create an ordered collection A of functions of a real number. At least one function should be built-in and 
// at least one should be user-defined; try using the sine, cosine, and cubing functions. 
// Fill another collection B with the inverse of each function in A. 
// Implement function composition as in Functional Composition. Finally, demonstrate that the result of applying the composition 
// of each function in A and its inverse in B to a value, is the original value

import system'routines;
import system'math;
import extensions'routines;
import extensions'math;
 
extension op
{
    compose(f,g)
        = f(g(self));
}
 
public program()
{
   var fs := new::( mssgconst sin<mathOp>[0], mssgconst cos<mathOp>[0], (x => power(x, 3.0r)) );
   var gs := new::( mssgconst arcsin<mathOp>[0], mssgconst arccos<mathOp>[0], (x => power(x, 1.0r / 3)) );
 
   fs.zipBy(gs, (f,g => 0.5r.compose(f,g)))
     .forEach:printingLn
}