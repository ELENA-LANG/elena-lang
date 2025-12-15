import system'collections;
import system'routines;
import extensions;

Unit2mult = new Map<string, real>()
    .setAt("arshin",     0.7112r)
    .setAt("centimeter", 0.01r)     
    .setAt("diuym",      0.0254r)
    .setAt("fut",        0.3048r)
    .setAt("kilometer",  1000.0r)   
    .setAt("liniya",     0.00254r)
    .setAt("meter",      1.0r)
    .setAt("milia",      7467.6r)
    .setAt("piad",       0.1778r)
    .setAt("sazhen",     2.1336r)
    .setAt("tochka",     0.000254r) 
    .setAt("vershok",    0.04445r)
    .setAt("versta",     1066.8r);

public Program()
{
    if (Program_arguments.Length != 3)
        { Console.writeLine("need two arguments - number then units"); AbortException.raise() };
        
    real value := Program_arguments[1].toReal();    
    string unit := Program_arguments[2];
    if:not (Unit2mult.containsKey(unit))
    {
        Console.printLine("only following units are supported:",
            Unit2mult.selectBy::(x=>x.Item1).asEnumerable());
            
        AbortException.raise()
    };
    
    Console.printLine(value," ",unit," to:");

    Unit2mult.forEach::(u,mlt)
    {
        Console.printPaddingLeft(30, u, ":").printLine(value * Unit2mult[unit] / mlt)
    }    
}