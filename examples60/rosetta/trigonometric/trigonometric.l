import system'math;
import extensions;
 
public program()
{
    console.printLine("Radians:");
    console.printLine("sin(π/3) = ",(Pi_value/3).sin());
    console.printLine("cos(π/3) = ",(Pi_value/3).cos());
    console.printLine("tan(π/3) = ",(Pi_value/3).tan());
    console.printLine("arcsin(1/2) = ",0.5r.arcsin());
    console.printLine("arccos(1/2) = ",0.5r.arccos());
    console.printLine("arctan(1/2) = ",0.5r.arctan());
    console.printLine();
 
    console.printLine("Degrees:");
    console.printLine("sin(60º) = ",60.0r.Radian.sin());
    console.printLine("cos(60º) = ",60.0r.Radian.cos());
    console.printLine("tan(60º) = ",60.0r.Radian.tan());
    console.printLine("arcsin(1/2) = ",0.5r.arcsin().Degree,"º");
    console.printLine("arccos(1/2) = ",0.5r.arccos().Degree,"º");
    console.printLine("arctan(1/2) = ",0.5r.arctan().Degree,"º");
 
    console.readChar()
}