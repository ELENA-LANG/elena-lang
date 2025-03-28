import system'math;
import extensions;
 
public program()
{
    Console.printLine("Radians:");
    Console.printLine("sin(π/3) = ",(Pi_value/3).sin());
    Console.printLine("cos(π/3) = ",(Pi_value/3).cos());
    Console.printLine("tan(π/3) = ",(Pi_value/3).tan());
    Console.printLine("arcsin(1/2) = ",0.5r.arcsin());
    Console.printLine("arccos(1/2) = ",0.5r.arccos());
    Console.printLine("arctan(1/2) = ",0.5r.arctan());
    Console.printLine();
 
    Console.printLine("Degrees:");
    Console.printLine("sin(60º) = ",60.0r.Radian.sin());
    Console.printLine("cos(60º) = ",60.0r.Radian.cos());
    Console.printLine("tan(60º) = ",60.0r.Radian.tan());
    Console.printLine("arcsin(1/2) = ",0.5r.arcsin().Degree,"º");
    Console.printLine("arccos(1/2) = ",0.5r.arccos().Degree,"º");
    Console.printLine("arctan(1/2) = ",0.5r.arctan().Degree,"º");
 
    Console.readChar()
}