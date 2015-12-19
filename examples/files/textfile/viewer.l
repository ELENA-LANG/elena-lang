#define system.
#define system'io.
#define extensions'routines.

#symbol program =
[
    ('program'arguments length == 1)?
        [  console writeLine:"Please provide the path to the file to view". #throw AbortException new. ].

    'program'arguments@1 file_path run &eachLine:printingLn.

    console readChar. // wait for any key
].