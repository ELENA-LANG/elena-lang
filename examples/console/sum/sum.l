import extensions.

// --- Prompt ---

const IntSamplePrompt = "Enter the two integer numbers:".

//const RealSamplePrompt = "Enter the two real numbers:".
//
//// --- EReader ---
//
//extension EReader
//{
//    loadFrom : aStream
//        = aStream readLine; convert to:self
//            | if(:e)
//                [
//                    console writeLine:(e message); readChar.
//                    
//                    AbortException new; raise.
//                ].
//}

//enteredNumber = forwards'number new; loadFrom:console.

// --- Program ---

public program =
[
    var s := console readLine.
    
//    console
//        printLine:forwards'prompt;
//        printLine("The sum is ",enteredNumber + enteredNumber);
//        readChar
].
