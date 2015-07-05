// Two or more words can be composed of the same characters, but in 
// a different order. Using the word list at 
// http://www.puzzlers.org/pub/wordlists/unixdict.txt, find the sets of words 
// that share the same characters that contain the most words in them. 

#define system.
#define system'routines.
#define system'calendar.
#define system'io.
#define system'collections.
#define extensions.
#define extensions'routines.

// --- Normalized ---

#class(extension) op
{
     #method normalized
         = self toArray ascendant summarize:(String new) literal.
}

// --- Program ---

#symbol program =
[
    #var aStart := now.

    #var aDictionary := Dictionary new.

    File new &path:"unixdict.txt" run &eachLine: aWord
    [    
        #var aKey := aWord normalized.
        #var anItem := aDictionary@aKey.
        ($nil == anItem) ?
        [
            anItem := ArrayList new.
            aDictionary@aKey := anItem.
        ].
    
        anItem += aWord.
    ].

    aDictionary array_list
        sort: (:aFormer:aLater) [ aFormer length > aLater length ]
        top:20 run &each: aPair [ console writeLine:aPair ].
    
    #var anEnd := now.
   
    #var aDiff := anEnd - aStart.

    console writeLine:"Time elapsed in msec:":(aDiff milliseconds).
].