#define system.
#define system'calendar.

// --- Power ---

// x ^ y

#symbol Power = (:x:y)
[
    #var aResult := Real new::1.

    control forrange &int:1 &int:y &do: (&int:n) [ aResult *= x ] .

    ^ aResult.    
].

#symbol MAX = 10000.

#symbol Program =
[
    #var aStart := now.

    #var pi := Real new::0.

    control forrange &int:0 &int:MAX &do: (&int:i)
    [
        pi += (Power eval:(-1):i / ((2*i)+1)*4).
    ].
    
    #var anEnd := now.
        
    console writeLine:pi.
        
    #var aDiff := anEnd - aStart.
    console write:"Time elapsed in msec:" writeLine:(aDiff Milliseconds).

    console readChar.
].
