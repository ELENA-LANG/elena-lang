#define system.
#define system'math.
#define system'routines.
#define extensions.

// --- Program --- 

#symbol Program = 
[ 
    // reading    
    #var aNumber := console write:"Enter the number you want to convert: " readLine:(Integer new).
    
    // converting
    #var aBinaryStr := aNumber toLiteral &base:2.

    // printing
    console
        writeLine:"A binary presentation of the number : ":(aBinaryStr padLeft &char:#48 &length:((aBinaryStr length / 4 + 1) * 4))
        writeLine:"Press any key to continue".

    // waiting for any key
    console readChar.
].
