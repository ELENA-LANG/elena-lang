#define system.
#define system'math.

// --- Functions ---

#symbol mathControl =
{
    exp : x
        = x cast:%eval &to:
            {
                eval &int : aValue = RealNumber new &int:aValue exp.
                
                eval &real : aValue = aValue exp.
            }.
            
    ln : x
        = x cast:%eval &to:
            {
                eval &int : aValue = RealNumber new &int:aValue ln.
                
                eval &real : aValue = aValue ln.
            }.
            
    round : x
        = x cast:%eval &to:
            {
                eval &int : aValue = RealNumber new &int:aValue.
                
                eval &real : aValue = aValue rounded.
            }.
            
    abs : x
        = x cast:%eval &to:
            {
                eval &int : aValue = aValue absolute.
                
                eval &real : aValue = aValue absolute.
            }.
            
    mod :x :y
        = x - (x / y * y).
        
    power : x : y
        = x cast:%eval &to:
            {
                eval &int : aValue = RealNumber new &int:aValue ln exp * y.
                
                eval &real : aValue = aValue ln exp * y.
            }.
}.
