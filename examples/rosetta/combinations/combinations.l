import system'routines;
import extensions;
import extensions'routines;

const int M = 3;
const int N = 5; 

// --- Numbers ---

numbers(n)
{
    ^ Array.allocate(n).populate:(int n => n)
}

// --- Program ---

public program()
{
    var numbers := numbers(N);    
    Combinator.new(M, numbers).forEach:(row)
    {
        console.printLine(row.toString())
    };
    
    console.readChar()
}