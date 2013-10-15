#define system.

// --- Power ---

// x ^ y

#symbol Power = &&:x:y
[
    #var aResult := Real new::1.

    control from:1 &to:y &do: &&:n [ aResult *= x ] .

    ^ aResult.    
].

#symbol MAX = 10000.

#symbol Program =
[
    #var aStart := now.

    #var pi := Real new::0.

    control from:0 &to:MAX &do: &&:i
    [
        pi += (Power eval:(-1):i / ((2*i)+1)*4).
    ].
    
    #var anEnd := now.
        
    console writeLine:pi.
        
    #var aDiff := anEnd - aStart.
    console write:"Time elapsed in msec:" writeLine:(aDiff Milliseconds).

    console readChar.
].
