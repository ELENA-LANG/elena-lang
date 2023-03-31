import system'routines;
import extensions;

// --- Program ---

public program()
{
    var s :="hello";
    
    if(s.startingWith:"hel")
    {
        console.printLine(s," starts with hel")
    };
    
    if(s.endingWith:"llo")
    {
        console.printLine(s," ends with llo")
    };

    if(s.containing:"el")
    {
        console.printLine(s," contains el")
    };
    
    console.readChar()
}