// Two or more words can be composed of the same characters, but in 
// a different order. Using the word list at 
// http://www.puzzlers.org/pub/wordlists/unixdict.txt, find the sets of words 
// that share the same characters that contain the most words in them. 

#define system.
#define system'collections.
#define extensions'text.

// --- Normalized ---

#symbol Normalized = &&:aLiteral
[
     ^ Summing new:(String new) foreach:(arrayControl sort:(stringControl toArray:aLiteral)) Literal.
].

// --- Program ---

#symbol program =
[
    #var aStart := now.

    #var aDictionary := Dictionary new.

    textFileControl forEachLine:"unixdict.txt" &do: &&:aWord
    [    
        #var aKey := Normalized eval:aWord.
        #var anItem := aDictionary @ aKey.
        nil == anItem ?
        [
            anItem := List new.
            aDictionary setAt:aKey:anItem.
        ].
    
        anItem += aWord.
    ].

    listControl sort:aDictionary &with: &&:aFormer:aLater [ aFormer Value Count > aLater Value Count ].

    listControl foreach:aDictionary &top:20 &do: &&:aPair [ console writeLine:(ListPresenter new:(aPair Value)) ].
    
    #var anEnd := now.
   
    #var aDiff := anEnd - aStart.
    console writeLine:"Time elapsed in msec:" write:(aDiff Milliseconds).
].