#import system'io.
#import system'routines.
#import system'collections.
#import extensions.
#import extensions'routines.
#import extensions'text.

// --- Record ---
class Record
{
    object theName.
    object thePrice.
    
    constructor new &Price:aPrice &Name:aName
    [
        theName := aName.
        thePrice := aPrice.
    ]

    Name = theName.
    
    Price = thePrice.

    literal
    [
        ^ StringWriter new writeLiteral:"Name:":theName:" Price:":thePrice literal.
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
           [ theBM += ";". ].
        
        theBM += aLiteral.
    ]

    read : aVariable
        = (0 == theBM length)
            ? [ false ]
            ! [
                theBM find:";".
                
                theBM Marked convert &to:aVariable.
                
                theBM mark.
                
                ^ true.
            ].
}

// --- Program --- 

program = 
[
    ('program'arguments length == 1)?
        [  console << "Please provide the path to the file to view". #throw AbortException new. ].

    var aList := List new.
    var anAdapter := Adapter new.

    'program'arguments@1 file_path run &eachLine: aLine
    [
        anAdapter write:aLine.

        var aName := String new.
        var aPrice := Real new.
                        
        while (anAdapter read:aName)
        [
            anAdapter read:aPrice.
        
            aList append:(Record new &Price:(aPrice get) &Name:(aName get)).
        ].
    ].

    aList run &each:printingLn.

    console readChar.
].
