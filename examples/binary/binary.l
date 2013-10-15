#define system.
#define extensions.
#define extensions'io.

// --- Program --- 

#symbol Program = 
[ 
    // reading    
    #var aNumber := consoleEx writeLine:"Enter the number you want to convert:" readLine:(Integer new).
    
    // converting
    #var aBinary := convertControl toLiteral:aNumber &radix:2.

    control while:[ aBinary Length and::3 != 0 ] &do:
    [
        aBinary := "0" + aBinary.
    ].

    // printing
    console writeLine:aBinary writeLine:"Press any key to continue".

    // waiting for any key
    console readChar.
].
