#define system'routines.
#define extensions.

// --- Program ---

#symbol program =
[
    stringControl starting:"hello" &with:"hel" ?
    [
        consoleEx writeLine:"hello starts with hel".
    ].
    
    stringControl ending:"hello" &with:"llo" ?
    [
        consoleEx writeLine:"hello ends with llo".
    ].

    stringControl contain:"el" &in:"hello" ?
    [
        consoleEx writeLine:"hello contains el".
    ].
].