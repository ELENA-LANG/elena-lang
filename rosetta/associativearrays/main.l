#define system.
#define system'collections.

// --- Program ---

#symbol program =
[
    // 1. Create
    #var aMap := Dictionary new.
    aMap setAt:"key":"foox".
    aMap setAt:"key":"foo".
    aMap setAt:"key2":"foo2".
    aMap setAt:"key3":"foo3".
    aMap setAt:"key4":"foo4".

    // Enumerate
    control foreach:aMap &do: &&:aKeyValue
        [ console write:(aKeyValue Key) write:" : " writeLine:(aKeyValue Value) ].
].