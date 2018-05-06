import system'io.
import extensions'routines.

public program =
[
    if (forwards'program'arguments length == 1)
        [  console writeLine:"Please provide the path to the file to view". AbortException new; raise ].

    File new(forwards'program'arguments[1]); forEachLine:printingLn.
  
    console readChar. // wait for any key
].