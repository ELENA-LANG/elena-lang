/// showing the sequence of binary digits for given numbers

import system'routines;
import extensions;

// --- Program ---

public Program()
{
    new int[]{5,50,9000}.forEach::(n)
    {
        Console.printLine(n.toString(2))
    }
}