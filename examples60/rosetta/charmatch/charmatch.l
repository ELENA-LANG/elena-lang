import system'routines;
import extensions;

// --- Program ---

public program()
{
    var s :="hello";
    
    if(s.startingWith("hel"))
    {
        Console.printLine(s," starts with hel")
    };
    
    if(s.endingWith("llo"))
    {
        Console.printLine(s," ends with llo")
    };

    if(s.containing("el"))
    {
        Console.printLine(s," contains el")
    };
    
    Console.readChar()
}