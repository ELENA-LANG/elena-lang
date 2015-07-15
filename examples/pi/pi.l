#define system.
#define system'math.
#define system'calendar.
#define extensions.

#symbol(const) MAX = 1000000.

#symbol Program =
[
    #var aStart := now.

    #var pi := Real new.
    
    control run &int:0 &int:MAX &every: (&int:i)
    [
        pi += -1.0r power &int:i / (2*i+1) * 4.
    ].

    #var anEnd := now.

    console writeLine:pi.

    #var aDiff := anEnd - aStart.
    console writeLine:"Time elapsed in msec:":(aDiff milliseconds).

    console readChar.
].
