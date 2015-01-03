#define system.
#define system'calendar.
#define extensions.

// --- Power ---

// x ^ y

#symbol Power = (&int:x &int:y)
[
    #var aResult := Real new &int:1.

    control run &int:1 &int:y &forEvery: (&index:n) [ aResult *= x ] .

    ^ aResult.
].

#symbol(const) MAX = 10000.

#symbol Program =
[
    #var aStart := now.

    #var pi := Real new.
    
    control run &int:0 &int:MAX &forEvery: (&index:i)
    [
        pi += (Power eval &int:(-1) &int:i / ((2*i)+1)*4).
    ].
    
    #var anEnd := now.

    console writeLine:pi.

    #var aDiff := anEnd - aStart.
    console write:"Time elapsed in msec:" writeLine:(aDiff milliseconds).

    console readChar.
].
