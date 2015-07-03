#define system.
#define system'routines.
#define system'math.
#define extensions.

// --- Constants ---

#symbol Letters    = "abcdefghijklmnopqrstuvwxyz".
#symbol BigLetters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ".
#symbol TestText   = "Pack my box with five dozen liquor jugs.".
#symbol Key = 12.

// --- Encrypt / Decript ---

#class Encrypting :: Enumerator
{
    #field theKey.
    #field theEnumerator.
    
    #constructor new &key:aKey &text:aText
    [
        theKey := aKey.
        theEnumerator := aText enumerator.
    ]
    
    #method next => theEnumerator.
    
    #method reset => theEnumerator.
    
    #method get
    [
        #var aChar := theEnumerator get.
        
        #var anIndex := Letters indexOf:0:aChar.
        
        (-1 < anIndex)
            ? [
                ^ Letters @ ((theKey+anIndex) mod:26).
            ]
            ! [
                anIndex := BigLetters indexOf:0:aChar.
                (-1 < anIndex)
                    ? [
                        ^ BigLetters @ ((theKey+anIndex) mod:26).
                    ]
                    ! [
                        ^ aChar.
                    ].
            ].
    ]
}

// --- Functions ---

#class(extension)encryptOp
{
    #method encrypt : aKey
        = Encrypting new &key:aKey &text:self summarize:(String new). 

    #method decrypt :aKey
        = Encrypting new &key:(26 - aKey) &text:self summarize:(String new). 
}

// --- Program ---

#symbol program =
[
    console writeLine:"Original text :" :TestText.
        
    #var anEncryptedText := TestText encrypt:Key.

    console writeLine:"Encrypted text:" :anEncryptedText.

    #var aDecryptedText := anEncryptedText decrypt:Key.

    console writeLine:"Decrypted text:" :aDecryptedText.

    console readChar.
].