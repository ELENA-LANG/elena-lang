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
    sortRange:anArray &int:aStart &int:anEnd
    [
        #var(type:int) aCurrent := aStart.
        control while:[aCurrent * 2 < anEnd] &do:
        [
            #var(type:int) aMaxChild.
            #var(type:int) aChild := (aCurrent * 2) + 1.

            (aChild == anEnd)or:[(anArray@aChild) > (anArray@(aChild + 1))]
                ? [ aMaxChild := aChild. ]  
                ! [ aMaxChild := aChild + 1. ].

            (anArray@aCurrent) < (anArray@aMaxChild)
                ? [
                    anArray~indexable exchange &int:aCurrent &int:aMaxChild.

                    aCurrent := aMaxChild.
                ]
                ! [ #break nil. ].
        ].
     ]

    sort : anArray
    [
        #var(type:int) aLastIndex := anArray length - 1.
        
        control forrange &int:(anArray length / 2) &int:0 &do: (&int:anIndex)
        [
            sorting sortRange:anArray &int:anIndex &int:aLastIndex
        ].
        
        control forrange &int:aLastIndex &int:0 &do: (&int:anIndex)
        [
            anArray~indexable exchange &int:anIndex &int:0.

            sorting sortRange:anArray &int:0 &int:(anIndex - 1).
        ].
    ]
}.

// === Program ===

#symbol Program =
[
    // 1. Create the array
    #var aSize := consoleEx write:"Enter the array size:" readLine:(Integer new).

    // 2. Initialize the array
    #var anArray := arrayControl new &length:aSize &each:prompter.

    // 3. Sort the array
    sorting sort:anArray.

    // 4. Print the array
    consoleEx writeLine:"Sorted array:":anArray.

    consoleEx readChar. // wait for any key
].
