#define system.
#define extensions.

// --- Prompt ---

#symbol IntSamplePrompt = "Enter the two integer numbers:".

#symbol RealSamplePrompt = "Enter the two real numbers:".

// --- EReader ---

#class(extension) EReader
{
    #method loadFrom : aStream
        =  convertor save:(aStream readLine) &to:self
            | onFormatError: e
                [
                    console writeLine:(e Message) readChar.
                    
                    #break.
                ].
}

#symbol enteredNumber = 'number new loadFrom:console.

// --- Program ---

#symbol Program =
[
    consoleEx 
        writeLine:'prompt
        writeLine:"The sum is ":(enteredNumber + enteredNumber)
        readChar.
].



