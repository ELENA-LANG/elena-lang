//import system'math.
import system'calendar;
import extensions;

const int MAX = 50000000;

public program()
{
    var started := now;

    real pi := 0.0;
    for(int i := 0, i < MAX, i += 1)
    {
        pi += -1.0.power(i) / (i*2+1) * 4
    };

    var ended := now;

    console.printLine(pi);

    var diff := ended - started;
    console.printLine("Time elapsed in msec:",diff.Milliseconds);

    console.readChar()
}
