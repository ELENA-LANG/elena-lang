import system'routines;
import system'math;
import extensions;
import extensions'text;

// --- Constants ---

const string Letters    = "abcdefghijklmnopqrstuvwxyz";
const string BigLetters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const string TestText   = "Pack my box with five dozen liquor jugs.";
const int Key = 12;

// --- Encrypt / Decript ---

class Encrypting : Enumerator
{
    int        theKey;
    Enumerator theEnumerator;
    
    constructor(int key, string text)
    {
        theKey := key;
        theEnumerator := text.enumerator();
    }
    
    bool next() => theEnumerator;
    
    reset() => theEnumerator;
    
    enumerable() => theEnumerator;
    
    get()
    {
        var ch := theEnumerator.get();
        
        var index := Letters.indexOf(0, ch);
        
        if (-1 < index)
        {
            ^ Letters[(theKey+index).mod:26]
        }
        else
        {
            index := BigLetters.indexOf(0, ch);
            if (-1 < index)
            {
                ^ BigLetters[(theKey+index).mod:26]
            }
            else
            {
                ^ ch
            }
        }
    }
}

// --- Functions ---

extension encryptOp
{
    encrypt(key)
        = new Encrypting(key, self).summarize(new StringWriter()); 

    decrypt(key)
        = new Encrypting(26 - key, self).summarize(new StringWriter()); 
}

// --- Program ---

public program()
{
    console.printLine("Original text :",TestText);
        
    var encryptedText := TestText.encrypt:Key;

    console.printLine("Encrypted text:",encryptedText);

    var decryptedText := encryptedText.decrypt:Key;

    console.printLine("Decrypted text:",decryptedText);

    console.readChar()
}