//import system'io.
//import system'routines.
import system'collections.
//import extensions.
//import extensions'routines.
//import extensions'text.
//
//// --- Record ---
//class Record
//{
//    object theName.
//    object thePrice.
//    
//    constructor new Price:aPrice Name:aName
//    [
//        theName := aName.
//        thePrice := aPrice.
//    ]
//
//    Name = theName.
//    
//    Price = thePrice.
//
//    literal
//    [
//        ^ StringWriter new; print("Name:",theName," Price:",thePrice).
//    ]
//}
//
//// --- Adapter ---
//
//class Adapter
//{
//    object theBM.
//    
//    constructor new
//    [
//        theBM := StringBookmark new.
//    ]
//
//    write : aLiteral
//    [
//        ifnot (0 == theBM length)
//           [ theBM append:";". ].
//        
//        theBM append:aLiteral.
//    ]
//
//    read : aVariable
//    [
//        if(0 == theBM length)
//            [ ^false ];
//            [
//                theBM find:";".
//                
//                theBM Marked; convert to:aVariable.
//                
//                theBM mark.
//                
//                ^ true
//            ]
//    ]
//}

// --- Program --- 

public program = 
[
    (forwards'program'arguments length == 1)?
        [  console printLine("Please provide the path to the file to view"). AbortException new; raise. ].

    var aList := List new.
    var anAdapter := Adapter new.

//    File new('program'arguments[1]); forEachLine(:aLine)
//    [
//        anAdapter write:aLine.
//
//        var aName := String new.
//        var aPrice := Real new.
//                        
//        while (anAdapter read:aName)
//        [
//            anAdapter read:aPrice.
//        
//            aList append(Record new Price(aPrice value) Name(aName value)).
//        ].
//    ].
//
//    aList forEach:printingLn.

    console readChar
].
