import extensions.

// --- Prompt ---

public const IntSamplePrompt = "Enter the two integer numbers:".

public const RealSamplePrompt = "Enter the two real numbers:".

// --- EReader ---

extension EReader
{
    loadFrom : aStream
        = aStream readLine; convertTo:self
            | if(:e)
                [
                    console writeLine(e message); readChar.
                    
                    AbortException new; raise.
                ].
}

enteredNumber = forwards'number new; loadFrom:console.

// --- Program ---

public program =
[
    console
        printLine:forwards'prompt;
        printLine("The sum is ",enteredNumber + enteredNumber);
        readChar
].
