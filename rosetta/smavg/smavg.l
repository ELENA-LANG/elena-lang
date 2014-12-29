#define system.
#define system'routines.
#define system'collections.
#define extensions.

#class SMA
{
    #field thePeriod.
    #field theList.
    
    #constructor new : aPeriod
    [
        thePeriod := aPeriod.
        theList := List new.
    ]
    
    #method append : aNumber
    [
        theList += aNumber.

        #var aCount := theList length.
        ^ aCount =>
            0 ?  [ 0.0r ] 
            ! [
                (aCount > thePeriod)?
                [
                    theList remove &index:0.
                    
                    aCount := thePeriod.
                ].
        
                #var aSum := Summing new:(Real new &int:0) run &enumerator::theList.
                
                ^ aSum / aCount.
            ].
    ]
}

// --- Program ---

#symbol program =
[
    #var SMA3 := SMA new:3.
    #var SMA5 := SMA new:5.

    control run &int:1 &int:5 &forEvery: (&index:i)
    [
        consoleEx writeLine:"sma3 + " :i :" = ": (SMA3 += i).
        consoleEx writeLine:"sma5 + " :i :" = ": (SMA5 += i).
    ].

    control run &int:5 &int:1 &forEvery: (&index:i)
    [
        consoleEx writeLine:"sma3 + " :i :" = ": (SMA3 += i).
        consoleEx writeLine:"sma5 + " :i :" = ": (SMA5 += i).
    ].
].
