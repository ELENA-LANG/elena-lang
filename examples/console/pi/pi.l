//import system'math.
import system'calendar.
import extensions.

const int MAX = 1000000.

public program =
[
    var aStart := now.

    ref<real> pi := 0.0r.
    
    0 to:MAX do(:i)<int>
    [
        //pi += -1.0r power &int:i / (2*i+1) * 4.
        pi append(-1.0r power(i); divide(2*i+1); multiply(4))
    ].

    var anEnd := now.

    console writeLine:pi.

//    var aDiff := anEnd - aStart.
//    console printLine("Time elapsed in msec:",aDiff milliseconds).

    console readChar
].
