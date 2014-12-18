#define system.
#define extensions.
#define extensions'text.

#symbol program =
[
    ('program'arguments length == 1)?
        [  console << "Please provide the path to the file to view". #throw AbortException new. ].

    #var aFileName := 'program'arguments@1.
    
    control run &path:(aFileName literal) &forEachLine:printingLn.

    console readChar. // wait for any key
].