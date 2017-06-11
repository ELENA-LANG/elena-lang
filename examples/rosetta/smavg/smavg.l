import system'routines.
import system'collections.
import extensions.

class SMA
{
    object thePeriod.
    object theList.
    
    constructor new : aPeriod
    [
        thePeriod := aPeriod.
        theList := List new.
    ]
    
    append : aNumber
    [
        theList append:aNumber.

        var aCount := theList length.
        aCount =>
            0 [ ^0.0r ];
            ! [
                if (aCount > thePeriod)
                [
                    theList remove index:0.
                    
                    aCount := thePeriod
                ].
        
                var aSum := theList summarize:(Real new int:0).
                
                ^ aSum / aCount
            ]
    ]
}

// --- Program ---

program =
[
    var SMA3 := SMA new:3.
    var SMA5 := SMA new:5.

    1 to:5 doEach: (:i)
    [
        console print:"sma3 + " :i :" = ": (SMA3 append:i) paddingRight:30 with:$32.
        console printLine:"sma5 + " :i :" = ": (SMA5 append:i)
    ].

    5 to:1 doEach: (:i)
    [
        console print:"sma3 + " :i :" = ": (SMA3 append:i) paddingRight:30 with:$32.
        console printLine:"sma5 + " :i :" = ": (SMA5 append:i)
    ].
    
    console readChar.
].
