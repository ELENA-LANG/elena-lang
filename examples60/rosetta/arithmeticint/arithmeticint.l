/// Get two integers from the user, and then output the sum, difference, product, 
/// integer quotient and remainder of those numbers. 

import system'math;
import extensions;

// --- Program ---

public program()
{
    var a := Console.loadLineTo(new Integer());
    var b := Console.loadLineTo(new Integer());
    
    Console.printLine(a," + ",b," = ",a + b);
    Console.printLine(a," - ",b," = ",a - b);
    Console.printLine(a," * ",b," = ",a * b);
    Console.printLine(a," / ",b," = ",a / b);   // truncates towards 0
    Console.printLine(a," % ",b," = ",a.mod(b)); // matches sign of first operand
    Console.printLine(a," ^ ",b," = ",a ^ b);
}
