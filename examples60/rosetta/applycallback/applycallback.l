/// Apply a callback to an array

import system'routines;

PrintSecondPower(n){ Console.writeLine(n * n) }

public program()
{
    new int[]{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}.forEach(PrintSecondPower)
}
