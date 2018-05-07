import extensions.

// --- Program --- 

public program = 
[ 
    // reading    
    var aNumber := console write:"Enter the number you want to convert: "; readLineTo(Integer new).
    
    // converting
    var aBinaryStr := aNumber toLiteral(2).

    // printing
    console
        printLine("A binary presentation of the number : ",aBinaryStr padLeft($48, (aBinaryStr length / 4 + 1) * 4));
        printLine("Press any key to continue").

    // waiting for any key
    console readChar.
].
