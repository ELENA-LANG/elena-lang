#define system.
#define system'io.
#define extensions'routines.

#symbol program =
[
    ('program'arguments length == 1)?
        [  console writeLine:"Please provide the path to the file to view". #throw AbortException new. ].

    File new &path:('program'arguments@1) run &eachLine:printingLn.

    console readChar. // wait for any key
].