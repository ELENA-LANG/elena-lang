#define system.
#define extensions.

// --- Prompt ---

#symbol IntSamplePrompt = "Enter the two integer numbers:".

#symbol RealSamplePrompt = "Enter the two real numbers:".

// --- EReader ---

#class(extension) EReader
{
    #method loadFrom : aStream
        = aStream readLine convertTo:self
            | if &FormatError: e
                [
                    console writeLine:(e message) readChar.
                    
                    #throw AbortException new.
                ].
}

#symbol enteredNumber = 'number new loadFrom:console.

// --- Program ---

#symbol Program =
[
    console
        writeLine:'prompt
        writeLine:"The sum is ":(enteredNumber + enteredNumber)
        readChar.
].



