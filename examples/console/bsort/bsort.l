// --- default namespaces ---
import system'routines;
import extensions;

// --- Prompter ---

prompter(int index)
{
     ^ console.print("Enter array[",index,"]:").loadLineTo(new Integer()).Value
}

// --- bsortOp ---

extension bsortOp
{
    bsortRange(int start, int end)
    {
        int current := start;
        
        while(current * 2 < end)
        {
            int maxChild := 0;
            int child := current * 2 + 1;
    
            if (child == end || self[child] > self[child + 1])
            {
                maxChild := child
            }
            else
            {
                maxChild := child + 1
            };
    
            if (self[current] < self[maxChild])
            {
                self.exchange(current,maxChild);
    
                current := maxChild
            }
            else
            {
                ^ self
            }
        }
    }
     
    bsort()
    {
        for (int i := self.Length / 2, i >= 0, i -= 1)
        {
            self.bsortRange(i, self.Length - 1);
        };
        
        for (int i := self.Length - 1, i >= 0, i -= 1)
        {
            self.exchange(i, 0);
            
            self.bsortRange(0, i - 1);
        }
    }
}

// === Program ===

public program()
{
    // 1. Create the array
    var size := console.write:"Enter the array size:".loadLineTo(new Integer()).Value;

    // 2. Initialize the array
    var array := Array.allocate(size).populate:prompter;

    // 3. Sort the array
    anArray.bsort();

//    // 4. Print the array
//    console printLine("Sorted array:",anArray).
//
//    console readChar. // wait for any key
}