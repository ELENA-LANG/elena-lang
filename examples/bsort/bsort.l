// --- default namespaces ---
#define system.
#define system'routines.
#define extensions.

// --- Prompter ---

#symbol prompter = (&index:anIndex)
[
     consoleEx write:"Enter array[":anIndex:"]:" readLine:(Integer new) int
].

// --- sorting ---

#symbol sorting =
{
    sortRange &array:anArray &int:aStart &int:anEnd
    [
        #var(type:int)aCurrent := aStart.
        
        #loop (aCurrent * 2 < anEnd) ?
        [
            #var(type:int)aMaxChild.
            #var(type:int)aChild := (aCurrent * 2) + 1.

            (aChild == anEnd)or:[(anArray@aChild) > (anArray@(aChild + 1))]
                ? [ aMaxChild := aChild. ]  
                ! [ aMaxChild := aChild + 1. ].

            (anArray@aCurrent) < (anArray@aMaxChild)
                ? [
                    anArray~indexable exchange:aCurrent:aMaxChild.

                    aCurrent := aMaxChild.
                ]
                ! [ ^ self. ].
        ].
     ]

    sort : anArray
    [
        #var(type:int) aLastIndex := anArray length - 1.
        
        control run &int:(anArray length / 2) &int:0 &forEvery: (&index:anIndex)
        [
            sorting sortRange &array::anArray &int:anIndex &int:aLastIndex
        ].
        
        control run &int:aLastIndex &int:0 &forEvery: (&index:anIndex)
        [
            anArray~indexable exchange &index:anIndex &index:0.

            sorting sortRange &array::anArray &int:0 &int:(anIndex - 1).
        ].
    ]
}.

// === Program ===

#symbol Program =
[
    // 1. Create the array
    #var aSize := consoleEx write:"Enter the array size:" readLine:(Integer new).

    // 2. Initialize the array
    #var anArray := arrayControl new &length:(aSize int) &every:prompter.

    // 3. Sort the array
    sorting sort:anArray.

    // 4. Print the array
    consoleEx writeLine:"Sorted array:":anArray.

    consoleEx readChar. // wait for any key
].
