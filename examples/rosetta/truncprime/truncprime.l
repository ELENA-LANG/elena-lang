#import system.
#import system'calendar.
#import extensions.

#symbol MAXN = 1000000.

#class(extension)mathOp
{
    #method is &prime
    [
        #var(int)n := self int.
        
        (n < 2) ? [ ^ false. ].        
        (n < 4) ? [ ^ true. ].        
        (n mod:2 == 0) ? [ ^ false. ].        
        (n < 9) ? [ ^ true. ].        
        (n mod:3 == 0) ? [ ^ false. ].
        
        #var(int)r := n sqrt.        
        #var(int)f := 5.
        #loop (f <= r)?
        [
            ((n mod:f == 0) || (n mod:(f + 2) == 0))
                ? [ ^ false. ].
                
            f := f + 6.
            
        ].
        
        ^ true.
    ]
    
    #method is &rightTruncatable
    [
        #var(int)n := self int.
        
        #loop (n != 0)?
        [
            (n is &prime)
                ! [ ^ false. ].
            
            n := n / 10.
        ].
        
        ^ true.
    ]

    #method is &leftTruncatable
    [
        #var(int)n := self int.
        #var(int)tens := 1.
        
        #loop (tens < n)
            ? [ tens := tens * 10. ].
            
        #loop (n != 0)?
        [
            (n is &prime)
                ! [ ^ false. ].

            tens := tens / 10.
            n := n - (n / tens * tens).
        ].
        
        ^ true.
    ]
}

#symbol program =
[
    #var aStart := now.
    
    #var n := MAXN.
    #var max_lt := 0.
    #var max_rt := 0.

    #loop ((max_lt == 0) || (max_rt == 0))?
    [
        (n literal indexOf:"0" == -1) ?
        [
            ((max_lt == 0) and:[ n is &leftTruncatable ])
                ? [
                    max_lt := n.
                ].
                
            ((max_rt == 0) and:[ n is &rightTruncatable ])
                ? [
                    max_rt := n.
                ].
        ].
                        
        n := n - 1.            
    ].

    #var anEnd := now.
    
    console writeLine:"Largest truncable left is ":max_lt.    
    console writeLine:"Largest truncable right is ":max_rt.
    
    #var aDiff := anEnd - aStart.
    console writeLine:"Time elapsed in msec:":(aDiff milliseconds).
    
    console readChar.
].