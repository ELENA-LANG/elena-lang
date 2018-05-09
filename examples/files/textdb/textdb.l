// --- default namespaces ---
import system'io.
import system'routines.
import system'collections.
import extensions.
import extensions'routines.
import extensions'text.

// --- EField ---

extension fieldOp
{
    Unserialized
    [
        ^ Signature new(T<literal>(self)).
    ]
}

// --- Record ---
class Record
{
    object theName.
    object theAge.
    object thePhone.
    
    constructor new
    [
        theName := String new.
        theAge := Integer new.
        thePhone := String new.
    ]

    Name = theName literal.
    
    set Name:aName
    [
        aName convertTo:theName.
    ]
    
    Age = theAge Number.
    
    set Age:anAge
    [
        anAge convertTo:theAge.
    ]
    
    Phone = thePhone literal.
    
    set Phone:aPhone
    [
        aPhone convertTo:thePhone.
    ]

    T<literal> literal
        = StringWriter new;
            printLine("Name:",theName);
            printLine("Age:",theAge);
            printLine("Phone:",thePhone).
}

// --- Program ---

public program = 
[
    if (forwards'program'arguments length == 1)
        [  console << "Please provide the path to the file to view". AbortException new; raise ].

    var aDB := List new; append(Record new).
    
    File new(forwards'program'arguments[1]); forEachLine(:aLine)
    [
        if (aLine length == 0)
            [ aDB append:(Record new). ];
            [
                // split record line                    
                var aKeyValue := aLine splitBy:":".
                var aName  := aKeyValue[0].
                var aValue := aKeyValue[1].

                // update field
                var aSubj := aName Unserialized.
                
                (aDB last) ~ /*(aName Unserialized)*/aSubj set:aValue.
            ].
    ].
    
    // print the list
    aDB forEach:printingLn.

    console readChar.
].