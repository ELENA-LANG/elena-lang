// --- default namespaces ---
#define system.
#define system'routines.
#define extensions.

// --- Prompter ---

#symbol prompter = (&index:anIndex)
[
     console writeLiteral:"Enter array[":anIndex:"]:" readLine:(Integer new) int
].

// --- bsortOp ---

#class(extension)bsortOp
{
    #method bsortRange &from:aStart &to:anEnd
    [
        #var aCurrent := aStart.
        
        #loop (aCurrent * 2 < anEnd) ?
        [
            #var aMaxChild.
            #var aChild := (aCurrent * 2) + 1.
    
            ((aChild == anEnd)or:[ (self@aChild) > (self@(aChild + 1))])
                ? [ aMaxChild := aChild. ]  
                ! [ aMaxChild := aChild + 1. ].
    
            ((self@aCurrent) < (self@aMaxChild))
                ? [
                    self exchange:aCurrent:aMaxChild.
    
                    aCurrent := aMaxChild.
                ]
                ! [ ^ self. ].
        ].
     ]
     
     #method bsort
     [
        (self length / 2) to:0 &doEach: anIndex
        [
            self::bsortOp bsortRange &from:anIndex &to:(self length - 1).
        ].

        (self length - 1) to:0 &doEach: anIndex
        [
            self exchange:anIndex:0.
            
            self::bsortOp bsortRange &from:0 &to:(anIndex - 1).
        ].
     ]
}

// === Program ===

#symbol Program =
[
    // 1. Create the array
    #var aSize := console write:"Enter the array size:" readLine:(Integer new).

    // 2. Initialize the array
    #var anArray := Array new &length:aSize set &every:prompter.

    // 3. Sort the array
    anArray bsort.

    // 4. Print the array
    console writeLine:"Sorted array:":anArray.

    console readChar. // wait for any key
].
