/// Shows how to iterate over the key-value pairs of an associative array, and print each pair out. 

import system'collections.
import system'routines.
import extensions.

// --- Program ---

program =
[
    // 1. Create
    var aMap := Dictionary new.
    aMap@"key" := "foox".
    aMap@"key" := "foo".
    aMap@"key2":= "foo2".
    aMap@"key3":= "foo3".
    aMap@"key4":= "foo4".

    // Enumerate
    aMap run each: 
        (:aKeyValue)[ console printLine:(aKeyValue key):" : ":aKeyValue ].
].