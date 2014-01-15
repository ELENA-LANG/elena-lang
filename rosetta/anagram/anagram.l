// Two or more words can be composed of the same characters, but in 
// a different order. Using the word list at 
// http://www.puzzlers.org/pub/wordlists/unixdict.txt, find the sets of words 
// that share the same characters that contain the most words in them. 

#define system.
#define system'routines.
#define system'calendar.
#define system'collections.
#define extensions.
#define extensions'text.

// --- Normalized ---

#symbol Normalized = (:aLiteral)
[
     ^ Summing new:(String new) foreach:(arrayControl sort:(literalControl toArray:aLiteral)) literal.
].

// --- Program ---

#symbol program =
[
    #var aStart := now.

    #var aDictionary := Dictionary new.

    textFileControl forEachLine:"unixdict.txt" &do: aWord
    [    
        #var aKey := Normalized:aWord.
        #var anItem := aDictionary getAt &key:aKey.
        nil == anItem ?
        [
            anItem := List new.
            aDictionary set &key:aKey &value:anItem.
        ].
    
        anItem += aWord.
    ].

    listControl sort:aDictionary &with: (:aFormer:aLater) [ aFormer value length > aLater value length ].

    controlEx foreach:aDictionary &top:20 &do: aPair [ consoleEx writeLine:(aPair value) ].
    
    #var anEnd := now.
   
    #var aDiff := anEnd - aStart.
    consoleEx writeLine:"Time elapsed in msec:":(aDiff Milliseconds).
].