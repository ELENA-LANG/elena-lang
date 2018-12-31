//import system'math.
//import system'calendar;
import extensions;

const int MAX = 1000000;

public program()
{
//    var aStart := now.

    real pi := 0.0r;
    for(int i := 0, i < 1000000, i += 1)
    {
        pi += -1.0r.power(i) / (2*i+1) * 4
        //pi += -1.0r.power(i).divide(2*i+1); multiply(4)
    };

//    var anEnd := now.

    console.printLine(pi);

//    var aDiff := anEnd - aStart.
//    console printLine("Time elapsed in msec:",aDiff milliseconds).

    console.readChar()
}
