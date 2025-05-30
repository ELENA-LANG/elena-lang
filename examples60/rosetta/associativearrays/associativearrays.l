/// Shows how to iterate over the key-value pairs of an associative array, and print each pair out. 

import system'collections;
import system'routines;
import extensions;

// --- Program ---

public program()
{
    // 1. Create
    auto map := new Map<string, string>();
    map["key"] := "foox";
    map["key"] := "foo";
    map["key2"]:= "foo2";
    map["key3"]:= "foo3";
    map["key4"]:= "foo4";

    // Enumerate
    map.forEach::
        (tuple){ Console.printLine(tuple.Item1," : ",tuple.Item2) }
}