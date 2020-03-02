/// showing the sequence of binary digits for given numbers

import system'routines;
import extensions;

// --- Program ---

public program()
{
    new int[]{5,50,9000}.forEach:(n)
    {
        console.printLine(n.toString(2))
    }
}