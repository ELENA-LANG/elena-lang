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
    int        _key;
    Enumerator _enumerator;
    
    constructor(int key, string text)
    {
        _key := key;
        _enumerator := text.enumerator();
    }
    
    bool next() => _enumerator;
    
    reset() => _enumerator;
    
    enumerable() => _enumerator;
    
    get Value()
    {
        var ch := *_enumerator;
        
        var index := Letters.indexOf(0, ch);
        
        if (-1 < index)
        {
            ^ Letters[(_key+index).mod(26)]
        }
        else
        {
            index := BigLetters.indexOf(0, ch);
            if (-1 < index)
            {
                ^ BigLetters[(_key+index).mod(26)]
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
    Console.printLine("Original text :",TestText);
        
    var encryptedText := TestText.encrypt(Key);

    Console.printLine("Encrypted text:",encryptedText);

    var decryptedText := encryptedText.decrypt(Key);

    Console.printLine("Decrypted text:",decryptedText);

    Console.readChar()
}