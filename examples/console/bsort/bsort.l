// --- default namespaces ---
import system'routines.
import extensions.

// --- Prompter ---

prompter = (&int:anIndex)
[
     ^ console writeLiteral:"Enter array[":anIndex:"]:"; readLineTo:(Integer new); int
].

// --- bsortOp ---

extension bsortOp
{
    bsortRange int:aStart int:anEnd
    [
        int aCurrent := aStart.
        
        while(aCurrent * 2 < anEnd)
        [
            int aMaxChild := 0.
            int aChild := (aCurrent * 2) + 1.
    
            if ((aChild == anEnd)or:$(self@aChild > self@(aChild + 1)))
                [ aMaxChild := aChild ];
                [ aMaxChild := aChild + 1 ].
    
            if ((self@aCurrent) < (self@aMaxChild))
                [
                    self exchange:aCurrent:aMaxChild.
    
                    aCurrent := aMaxChild
                ];
                [ ^ self ]
        ].
     ]
     
     bsort
     [
        (self length / 2) to:0 doEach: (:anIndex)
        [
            self~bsortOp bsortRange int:anIndex int:(self length - 1).
        ].

        (self length - 1) to:0 doEach: (:anIndex)
        [
            self exchange:anIndex:0.
            
            self~bsortOp bsortRange int:0 int:(anIndex - 1).
        ].
     ]
}

// === Program ===

Program =
[
    // 1. Create the array
    var aSize := console write:"Enter the array size:"; readLineTo:(Integer new).

    // 2. Initialize the array
    var anArray := Array new length:aSize; set every:prompter.

    // 3. Sort the array
    anArray bsort.

    // 4. Print the array
    console writeLine:"Sorted array:":anArray.

    console readChar. // wait for any key
].
