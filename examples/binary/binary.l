#define system.
#define extensions.

// --- Program --- 

#symbol Program = 
[ 
    // reading    
    #var aNumber := consoleEx write:"Enter the number you want to convert: " readLine:(Integer new).
    
    // converting
    #var aBinary := convertor toLiteral:aNumber &base:2.

    control while:[ aBinary length and::3 != 0 ] &do:
    [
        aBinary := "0" + aBinary.
    ].

    // printing
    consoleEx writeLine:"A binary presentation of the number : ":aBinary writeLine:"Press any key to continue".

    // waiting for any key
    console readChar.
].
