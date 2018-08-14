/// Shows how to iterate over the key-value pairs of an associative array, and print each pair out. 

import system'collections.
import system'routines.
import extensions.

// --- Program ---

public program
[
    // 1. Create
    auto aMap := Map<literal,literal>().
    aMap["key"] := "foox".
    aMap["key"] := "foo".
    aMap["key2"]:= "foo2".
    aMap["key3"]:= "foo3".
    aMap["key4"]:= "foo4".

    // Enumerate
    aMap forEach
        (:tuple)[ console printLine(tuple item1," : ",tuple item2) ].
]