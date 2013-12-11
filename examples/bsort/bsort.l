// --- default namespaces ---
#define system.
#define system'routines.
#define extensions.

// --- Prompter ---

#symbol prompter = &&:anIndex
[
     consoleEx write:"Enter array[":anIndex:"]:" readLine:(Integer new)
].

// --- sorting ---

#symbol sorting =
{
    sort:anArray &from:aStart &to:anEnd
    [
        #var aCurrent := aStart.
        control while:[aCurrent * 2 < anEnd] &do:
        [
            #var aMaxChild := nil.
            #var aChild := (aCurrent * 2) + 1.

            (aChild == anEnd)or:[(anArray@aChild) > (anArray@(aChild + 1))]
                ? [ aMaxChild := aChild. ]  
                ! [ aMaxChild := aChild + 1. ].

            (anArray@aCurrent) < (anArray@aMaxChild)
                ? [
                    anArray~EArray exchange:aCurrent:aMaxChild.

                    aCurrent := aMaxChild.
                ]
                ! [ #break nil. ].
        ].
     ]

    sort : anArray
    [
        #var aLastIndex := anArray Count - 1.
        
        control from:(anArray Count / 2) &backTo:0 &do: &&:anIndex
        [
            sorting sort:anArray &from:anIndex &to:aLastIndex
        ].
        
        control from:aLastIndex &backTo:0 &do: &&:anIndex
        [
            anArray~EArray exchange:anIndex:0.

            sorting sort:anArray &from:0 &to:(anIndex - 1).
        ].
    ]
}.

// === Program ===

#symbol Program =
[
    // 1. Create the array
    #var aSize := consoleEx write:"Enter the array size:" readLine:(Integer new).

    // 2. Initialize the array
    #var anArray := Array new &type'length:aSize &function:prompter.

    // 3. Sort the array
    sorting sort:anArray.

    // 4. Print the array
    consoleEx writeLine:anArray.

    consoleEx readChar. // wait for any key
].
