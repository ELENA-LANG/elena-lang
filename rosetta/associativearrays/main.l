#define system.
#define system'collections.

// --- Program ---

#symbol program =
[
    // 1. Create
    #var aMap := Dictionary new.
    aMap set &key:"key" &value:"foox".
    aMap set &key:"key" &value:"foo".
    aMap set &key:"key2" &value:"foo2".
    aMap set &key:"key3" &value:"foo3".
    aMap set &key:"key4" &value:"foo4".

    // Enumerate
    control run:aMap &forEach: aKeyValue
        [ console write:(aKeyValue key) write:" : " writeLine:(aKeyValue value) ].
].