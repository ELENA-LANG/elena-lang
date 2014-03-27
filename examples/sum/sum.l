#define system.
#define extensions.

// --- Prompt ---

#symbol IntSamplePrompt = "Enter the two integer numbers:".

#symbol RealSamplePrompt = "Enter the two real numbers:".

// --- EReader ---

#class(role) EReader
{
    #method(extension) loadNumber : aStream
        =  convertor saveVariant:(aStream readLine) &to:self
            | onFormatError: e
                [
                    console writeLine:(e Message) readChar.
                    
                    #throw BreakException new.
                ].
}

#use EReader.

// --- Program ---

#symbol Program =
[
    console writeLine:'prompt.

    #var a := 'number new loadNumber:console.
    #var b := 'number new loadNumber:console.

    console write:"The sum is " writeLine:(a + b).
            
    console readChar. // wait for any key
].
