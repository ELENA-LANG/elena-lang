#import system'routines.
#import system'math.
#import extensions.

// --- Constants ---

const Letters    = "abcdefghijklmnopqrstuvwxyz".
const BigLetters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ".
const TestText   = "Pack my box with five dozen liquor jugs.".
const Key = 12.

// --- Encrypt / Decript ---

class Encrypting :: Enumerator
{
    object theKey.
    object theEnumerator.
    
    constructor new &key:aKey &text:aText
    [
        theKey := aKey.
        theEnumerator := aText enumerator.
    ]
    
    next => theEnumerator.
    
    reset => theEnumerator.
    
    get
    [
        var aChar := theEnumerator get.
        
        var anIndex := Letters indexOf:0:aChar.
        
        if (-1 < anIndex)
            [
                ^ Letters @ ((theKey+anIndex) mod:26).
            ];
            [
                anIndex := BigLetters indexOf:0:aChar.
                if (-1 < anIndex)
                    [
                        ^ BigLetters @ ((theKey+anIndex) mod:26).
                    ];
                    [
                        ^ aChar.
                    ].
            ].
    ]
}

// --- Functions ---

extension encryptOp
{
    encrypt : aKey
        = Encrypting new &key:aKey &text:self summarize:(String new). 

    decrypt :aKey
        = Encrypting new &key:(26 - aKey) &text:self summarize:(String new). 
}

// --- Program ---

program =
[
    console writeLine:"Original text :" :TestText.
        
    var anEncryptedText := TestText encrypt:Key.

    console writeLine:"Encrypted text:" :anEncryptedText.

    var aDecryptedText := anEncryptedText decrypt:Key.

    console writeLine:"Decrypted text:" :aDecryptedText.

    console readChar.
].