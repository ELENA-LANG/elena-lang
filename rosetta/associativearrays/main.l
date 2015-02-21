#define system.
#define system'collections.
#define extensions.

// --- Program ---

#symbol program =
[
    // 1. Create
    #var aMap := Dictionary new.
    aMap set &key:"key" &object:"foox".
    aMap set &key:"key" &object:"foo".
    aMap set &key:"key2" &object:"foo2".
    aMap set &key:"key3" &object:"foo3".
    aMap set &key:"key4" &object:"foo4".

    // Enumerate
    control run:aMap &forEach: aKeyValue
        [ console write:(aKeyValue key) write:" : " writeLine:aKeyValue ].
].