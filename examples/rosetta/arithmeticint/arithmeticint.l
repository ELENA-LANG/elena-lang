/// Get two integers from the user, and then output the sum, difference, product, 
/// integer quotient and remainder of those numbers. 

import system'math.
import extensions.

// --- Program ---

program =
[
    var a := console readLineTo:(Integer new).
    var b := console readLineTo:(Integer new).
    
    console writeLine:a:" + ": b:" = ":(a + b).
    console writeLine:a:" - ": b:" = ":(a - b).
    console writeLine:a:" * ": b:" = ":(a * b).
    console writeLine:a:" / ": b:" = ":(a / b).   // truncates towards 0
    console writeLine:a:" % ":b:" = ":(a mod:b). // matches sign of first operand
].