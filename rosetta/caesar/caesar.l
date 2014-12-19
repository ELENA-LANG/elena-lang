#define system.
#define system'routines.
#define extensions.
#define system'math.

// --- Constants ---

#symbol Letters    = "abcdefghijklmnopqrstuvwxyz".
#symbol BigLetters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ".
#symbol TestText   = "Pack my box with five dozen liquor jugs.".
#symbol Key = 12.

// --- Encrypt / Decript ---

#class Encrypting : Enumerator
{
    #field(type:int) theKey.
    #field(type:enumerator) theEnumerator.
    
    #constructor new &key:aKey &text:aText
    [
        theKey := aKey int.
        theEnumerator := aText literal enumerator.
    ]
    
    #method next = theEnumerator next.
    
    #method reset [ theEnumerator reset. ]
    
    #method get
    [
        #var(type:char) aChar := theEnumerator get char.
        
        #var(type:int) anIndex := Letters indexOf &index:0 &char:aChar.
        
        (-1 < anIndex)
            ? [
                ^ Letters @ ((theKey+anIndex) int mod:26).
            ]
            ! [
                anIndex := BigLetters indexOf &index:0 &char:aChar.
                (-1 < anIndex)
                    ? [
                        ^ BigLetters @ ((theKey+anIndex) int mod:26).
                    ]
                    ! [
                        ^ aChar.
                    ].
            ].
    ]
}

// --- Functions ---

#symbol encrypt = (:aText:aKey)
    [ Summing new:(String new) run &enumerator:(Encrypting new &key:aKey &text:aText). ].

#symbol decrypt = (:aText:aKey)
    [ Summing new:(String new) run &enumerator:(Encrypting new &key:(26 - aKey) &text:aText). ].

// --- Program ---

#symbol program =
[
    #var anS := TestText.
    
    consoleEx writeLine:"Original text :" :anS.
        
    anS := encrypt:anS:Key.

    consoleEx writeLine:"Encrypted text:" :anS.

    anS := decrypt:anS:Key.

    consoleEx writeLine:"Decrypted text:" :anS.

    consoleEx readChar.
].