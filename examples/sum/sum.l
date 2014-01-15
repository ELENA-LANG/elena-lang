#define system.
#define extensions.

// --- Prompt ---

#symbol IntSamplePrompt = "Enter the two integer numbers:".

#symbol RealSamplePrompt = "Enter the two real numbers:".

// --- EReader ---

#class(role)EReader
{
    #method read : aStream
        =  convertor saveVariant:(aStream readLine) &to:self
            | onFormatError: e
                [
                    console writeLine:(e Message) readChar.
                    
                    #throw BreakException new.
                ].
}
    
// --- Program ---

#symbol Program =
[
    console writeLine:'prompt.

    #var a := 'number new ~EReader read:console.
    #var b := 'number new ~EReader read:console.
 
    console write:"The sum is " writeLine:(a + b).
            
    console readChar. // wait for any key
].
