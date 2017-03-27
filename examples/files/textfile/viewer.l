#import system'io.
#import extensions'routines.

program =
[
    if ('program'arguments length == 1)
        [  console writeLine:"Please provide the path to the file to view". #throw AbortException new. ].

    'program'arguments@1 file_path run &eachLine:printingLn.

    console readChar. // wait for any key
].