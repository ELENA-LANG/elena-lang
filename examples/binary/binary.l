#define system.
#define extensions.

// --- Program --- 

#symbol Program = 
[ 
    // reading    
    #var aNumber := consoleEx write:"Enter the number you want to convert: " readLine:(Integer new).
    
    // converting
    #var aBinary := String new:(convertor toLiteral:aNumber &base:2).

    control while:[ aBinary length and:3 != 0 ] &do:
    [
        aBinary insert &index:0 &literal:"0".
    ].

    // printing
    consoleEx writeLine:"A binary presentation of the number : ":aBinary writeLine:"Press any key to continue".

    // waiting for any key
    console readChar.
].
