// --- default namespaces ---
#import system'io.
#import system'routines.
#import system'collections.
#import extensions.
#import extensions'routines.
#import extensions'text.

// --- EField ---

extension fieldOp
{
    Unserialized
    [
        ^ Signature new &literal:self.
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
    
    set &Name:aName
    [
        aName convert &to:theName.
    ]
    
    Age = theAge Number.
    
    set &Age:anAge
    [
        anAge convert &to:theAge.
    ]
    
    Phone = thePhone literal.
    
    set &Phone:aPhone
    [
        thePhone set:aPhone.
    ]

    literal
        = StringWriter new
            writeLine:"Name:":theName
            writeLine:"Age:":theAge
            writeLine:"Phone:":thePhone.
}

// --- Program ---

Program = 
[
    ('program'arguments length == 1)?
        [  console << "Please provide the path to the file to view". AbortException new raise. ].

    var aDB := List new += Record new.
    
    'program'arguments@1 file_path run &eachLine: aLine
    [
        (aLine length == 0)
            ? [ aDB += Record new. ]
            ! [
                // split record line                    
                var aKeyValue := aLine split &by:":".
                var aName  := aKeyValue@0.
                var aValue := aKeyValue@1.

                // update field
                var aSubj := aName Unserialized.
                
                aDB last :: (aName Unserialized) set:aValue.
            ].
    ].
    
    // print the list
    aDB run &each:printingLn.

    console readChar.
].