import system'calendar;
import extensions;

const int MAX = 50000000;

public program()
{
    real pi := 0.0;
    for(int i := 0, i < MAX, i += 1)
    {
        pi += -1.0.power(i) / (i*2+1) * 4
    };

    console.printLine(pi);
}
