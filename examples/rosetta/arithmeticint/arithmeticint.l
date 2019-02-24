/// Get two integers from the user, and then output the sum, difference, product, 
/// integer quotient and remainder of those numbers. 

import system'math;
import extensions;

// --- Program ---

public program()
{
    var a := console.loadLineTo(new Integer());
    var b := console.loadLineTo(new Integer());
    
    console.printLine(a," + ",b," = ",a + b);
    console.printLine(a," - ",b," = ",a - b);
    console.printLine(a," * ",b," = ",a * b);
    console.printLine(a," / ",b," = ",a / b);   // truncates towards 0
    console.printLine(a," % ",b," = ",a.mod:b); // matches sign of first operand
}