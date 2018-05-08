import system'io.
import system'routines.
import system'collections.
import extensions.
import extensions'routines.
import extensions'text.

// --- Record ---
class Record
{
    object theName.
    object thePrice.
    
    constructor new(object aPrice, object aName)
    [
        theName := aName.
        thePrice := aPrice.
    ]

    Name = theName.
    
    Price = thePrice.

    T<literal> literal
    [
        ^ StringWriter new; print("Name:",theName," Price:",thePrice).
    ]
}

// --- Adapter ---

class Adapter
{
    object theBM.
    
    constructor new
    [
        theBM := StringBookmark new.
    ]

    write : aLiteral
    [
        ifnot (0 == theBM length)
           [ theBM append:";". ].
        
        theBM append:aLiteral.
    ]

    read : aVariable
    [
        if(0 == theBM length)
            [ ^false ];
            [
                theBM find:";".
                
                theBM Marked; convertTo:aVariable.
                
                theBM mark.
                
                ^ true
            ]
    ]
}

// --- Program --- 

public program = 
[
    (forwards'program'arguments length == 1)?
        [  console printLine("Please provide the path to the file to view"). AbortException new; raise. ].

    var aList := List new.
    var anAdapter := Adapter new.

    File new(forwards'program'arguments[1]); forEachLine(:aLine)
    [
        anAdapter write:aLine.

        var aName := String new.
        var aPrice := Real new.
                        
        while (anAdapter read:aName)
        [
            anAdapter read:aPrice.
        
            aList append(Record new(aPrice value, aName value)).
        ].
    ].

    aList forEach:printingLn.

    console readChar
].
