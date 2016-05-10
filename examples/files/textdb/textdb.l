// --- default namespaces ---
#import system.
#import system'io.
#import system'routines.
#import system'collections.
#import extensions.
#import extensions'routines.
#import extensions'text.

// --- EField ---

#class(extension) fieldOp
{
    #method Unserialized
    [
        ^ Signature new &literal:self.
    ]
}

// --- Record ---
#class Record
{
    #field theName.
    #field theAge.
    #field thePhone.
    
    #constructor new
    [
        theName := String new.
        theAge := Integer new.
        thePhone := String new.
    ]

    #method Name = theName literal.
    
    #method set &Name:aName
    [
        aName convert &to:theName.
    ]
    
    #method Age = theAge Number.
    
    #method set &Age:anAge
    [
        anAge convert &to:theAge.
    ]
    
    #method Phone = thePhone literal.
    
    #method set &Phone:aPhone
    [
        thePhone set:aPhone.
    ]

    #method literal
        = StringWriter new
            writeLine:"Name:":theName
            writeLine:"Age:":theAge
            writeLine:"Phone:":thePhone.
}

// --- Program ---

#symbol Program = 
[
    ('program'arguments length == 1)?
        [  console << "Please provide the path to the file to view". #throw AbortException new. ].

    #var aDB := List new += Record new.
    
    'program'arguments@1 file_path run &eachLine: aLine
    [
        (aLine length == 0)
            ? [ aDB += Record new. ]
            ! [
                // split record line                    
                #var aKeyValue := aLine split &by:":".
                #var aName  := aKeyValue@0.
                #var aValue := aKeyValue@1.

                // update field
                #var aSubj := aName Unserialized.
                
                aDB last :: (aName Unserialized) set:aValue.
            ].
    ].
    
    // print the list
    aDB run &each:printingLn.

    console readChar.
].