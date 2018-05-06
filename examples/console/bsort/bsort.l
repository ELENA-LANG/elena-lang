// --- default namespaces ---
import system'routines.
import extensions.

// --- Prompter ---

prompter = (:anIndex)<int>
[
     ^ console print("Enter array[",anIndex,"]:"); readLineTo(Integer new); value
].

// --- bsortOp ---

extension bsortOp
{
    bsortRange(int aStart, int anEnd)
    [
        int aCurrent := aStart.
        
        while(aCurrent * 2 < anEnd)
        [
            int aMaxChild := 0.
            int aChild := (aCurrent * 2) + 1.
    
            if ((aChild == anEnd) || (self[aChild] > self[aChild + 1]))
                [ aMaxChild := aChild ];
                [ aMaxChild := aChild + 1 ].
    
            if (self[aCurrent] < self[aMaxChild])
                [
                    self exchange(aCurrent,aMaxChild).
    
                    aCurrent := aMaxChild
                ];
                [ ^ self ]
        ].
     ]
     
     bsort
     [
        (self length / 2) to:0 do(:anIndex)
        [
            self~bsortOp bsortRange(anIndex, self length - 1).
        ].

        (self length - 1) to:0 do(:anIndex)
        [
            self exchange(anIndex,0).
            
            self~bsortOp bsortRange(0, anIndex - 1).
        ].
     ]
}

// === Program ===

public program =
[
    // 1. Create the array
    var aSize := console write:"Enter the array size:"; readLineTo(Integer new); value.

    // 2. Initialize the array
    var anArray := Array new(aSize); populate:prompter.

    // 3. Sort the array
    anArray bsort.

    // 4. Print the array
    console printLine("Sorted array:",anArray).

    console readChar. // wait for any key
].
