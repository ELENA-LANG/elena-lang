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
     normalized
         = self toArray; ascendant; summarize(StringWriter new); literal.
}

// --- Program ---

public program
[
    var aStart := now.

    var aDictionary := Dictionary new.

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
        sort(:aFormer:aLater)( aFormer length > aLater length );
        top:20; forEach(:aPair)[ console printLine(aPair value) ].
    
    var anEnd := now.
   
    var aDiff := anEnd - aStart.

    console printLine("Time elapsed in msec:",aDiff milliseconds).
    
    console readChar.
]