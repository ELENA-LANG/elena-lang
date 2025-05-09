import system'routines;
import extensions;
import extensions'routines;

const int M = 3;
const int N = 5; 

// --- Numbers ---

Numbers(n)
{
    ^ Array.allocate(n).populate::(int n => n)
}

// --- Program ---

public program()
{
    var numbers := Numbers(N);    
    Combinator.new(M, numbers).forEach::(row)
    {
        Console.printLine(row.toString())
    };
    
    Console.readChar()
}