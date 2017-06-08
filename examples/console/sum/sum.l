import extensions.

// --- Prompt ---

const IntSamplePrompt = "Enter the two integer numbers:".

const RealSamplePrompt = "Enter the two real numbers:".

// --- EReader ---

extension EReader
{
    loadFrom : aStream
        = aStream readLine; convert to:self
            | if FormatError: (:e)
                [
                    console writeLine:(e message); readChar.
                    
                    AbortException new; raise.
                ].
}

enteredNumber = 'number new; loadFrom:console.

// --- Program ---

Program =
[
    console
        writeLine:'prompt;
        writeLine:"The sum is ":(enteredNumber + enteredNumber);
        readChar
].



