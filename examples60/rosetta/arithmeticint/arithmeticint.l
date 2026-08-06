/// Get two integers from the user, and then output the sum, difference, product, 
/// integer quotient and remainder of those numbers. 

import system'math;
import extensions;

// --- Program ---

public Program()
{
    var a := Console.write("Enter the first integer:").loadLineTo(new Integer());
    var b := Console.write("Enter the second integer:").loadLineTo(new Integer());
    
    Console.printLine(a," + ",b," = ",a + b);
    Console.printLine(a," - ",b," = ",a - b);
    Console.printLine(a," * ",b," = ",a * b);
    Console.printLine(a," / ",b," = ",a / b);   // truncates towards 0
    Console.printLine(a," % ",b," = ",a.mod(b)); // matches sign of first operand
    Console.printLine(a," ^ ",b," = ",a ^ b);
}
