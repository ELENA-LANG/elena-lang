// Two or more words can be composed of the same characters, but in 
// a different order. Using the word list at 
// http://www.puzzlers.org/pub/wordlists/unixdict.txt, find the sets of words 
// that share the same characters that contain the most words in them. 

import system'routines.
import system'calendar.
import system'io.
import system'collections.
import extensions.
import extensions'routines.
import extensions'text.

// --- Normalized ---

extension op
{
     T<literal> normalized
         = self toArray; ascendant; summarize(StringWriter new).
}

// --- Program ---

public program
[
    var aStart := now.

    auto aDictionary := Map<literal,object>().

    File new("unixdict.txt"); forEachLine(:aWord)
    [    
        var s := aWord.
        var aKey := aWord normalized.
        var anItem := aDictionary[aKey].
        if (nil == anItem)
        [
            anItem := ArrayList new.
            aDictionary[aKey] := anItem.
        ].
    
        anItem append:aWord.
    ].

    aDictionary values;
        sort(:aFormer:aLater)( aFormer item2; length > aLater item2; length );
        top:20; forEach(:aPair)[ console printLine(aPair item2) ].
    
    var anEnd := now.
   
    var aDiff := anEnd - aStart.

    console printLine("Time elapsed in msec:",aDiff milliseconds).
    
    console readChar
]