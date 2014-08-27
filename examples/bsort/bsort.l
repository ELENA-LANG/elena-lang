// --- default namespaces ---
#define system.
#define system'routines.
#define extensions.

// --- Prompter ---

#symbol prompter = (:anIndex)
[
     consoleEx write:"Enter array[":anIndex:"]:" readLine:(Integer new)
].

// --- sorting ---

#symbol sorting =
{
    sortRange:anArray:aStart:anEnd
    [
        #var aCurrent := Integer new << aStart.
        control while:[ aCurrent * 2 < anEnd ] &do:
        [
            #var aMaxChild.
            #var aChild := (aCurrent * 2) + 1.

            (aChild == anEnd)or:[(anArray@aChild) > (anArray@(aChild + 1))]
                ? [ aMaxChild := aChild. ]  
                ! [ aMaxChild := aChild + 1. ].

            (anArray@aCurrent) < (anArray@aMaxChild)
                ? [
                    anArray~indexable exchange:aCurrent:aMaxChild.

                    aCurrent << aMaxChild.
                ]
                ! [ #break. ].
        ].
     ]

    sort : anArray
    [
        #var aLastIndex := anArray length - 1.
        
        control forrange &int:(anArray length / 2) &int:0 &do: (&int:anIndex)
        [
            sorting sortRange:anArray:anIndex:aLastIndex
        ].
        
        control forrange &int::aLastIndex &int:0 &do: (&int:anIndex)
        [
            anArray~indexable exchange &int:anIndex &int:0.

            sorting sortRange:anArray:0:(anIndex - 1).
        ].
    ]
}.

// === Program ===

#symbol Program =
[
    // 1. Create the array
    #var aSize := consoleEx write:"Enter the array size:" readLine:(Integer new).

    // 2. Initialize the array
    #var anArray := arrayControl new &length:(aSize int) &each:prompter.

    // 3. Sort the array
    sorting sort:anArray.

    // 4. Print the array
    consoleEx writeLine:"Sorted array:":anArray.

    consoleEx readChar. // wait for any key
].
