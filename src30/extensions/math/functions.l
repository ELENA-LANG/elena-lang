#define system.
#define system'math.

// --- Functions ---

#symbol mathControl =
{
    exp : x
        = x real exp.
            
    ln : x
        = x real ln.
            
    round : x
        = x real rounded.
            
    abs : x
        = x cast:%eval &to:
            {
                eval &int : aValue = aValue absolute.
                
                eval &long : aValue = aValue absolute.
                
                eval &byte : aValue = aValue absolute.
                
                eval &short : aValue = aValue absolute.
                
                eval &real : aValue = aValue absolute.
            }.
            
    mod :x :y
        = x - (x / y * y).
        
    power : x : y
       = x real ln multiply:y real exp.
}.
