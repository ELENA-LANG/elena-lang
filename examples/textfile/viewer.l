#define system.
#define extensions.
#define extensions'text.

#symbol program =
[
    ('program'arguments Count == 1)?
        [  console << "Please provide the path to the file to view". #throw BreakException new. ].

    #var aFileName := 'program'arguments@1.
    
    textFileControl forEachLine:aFileName &do:printingLn.

    console readChar. // wait for any key
].