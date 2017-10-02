import system'math.
import system'dynamic.

// --- Functions ---

class mathControl = 
    { 
        min : l : r
            = (l < r) iif(l,r).
    
        generic : n = Wrap::(n, mathOp)~$subject get.
        
        generic : n : m = Wrap::(n, mathOp)~$subject eval(m).
    }.
