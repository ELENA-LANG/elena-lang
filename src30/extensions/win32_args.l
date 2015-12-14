// ==== ver 3.4.1 ===

#import system.

// --- Arguments ---

#static arguments = 
    (extensions'text'TokenEnumerator new &literal:(commandLine literal))::convertorOp toArray.
