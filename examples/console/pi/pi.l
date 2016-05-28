#import system.
#import system'math.
#import system'calendar.
#import extensions.

#symbol(const,int) MAX = 1000000.

#symbol program =
[
    #var aStart := now.

    #var(vreal) pi := Real new.
    
    control run &int:0 &int:MAX &every: (&int:i)
    [
        //pi += -1.0r power &int:i / (2*i+1) * 4.
        pi += -1.0r power &int:i divide &int:(2*i+1) multiply &int:4.
    ].

    #var anEnd := now.

    console writeLine:pi.

    #var aDiff := anEnd - aStart.
    console writeLine:"Time elapsed in msec:":(aDiff milliseconds).

    console readChar.
].
