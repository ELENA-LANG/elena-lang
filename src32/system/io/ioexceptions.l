// --- IOException ---

class IOException :: Exception
{
    /// Creates the exception
    constructor new
        <= new literal:"I/O error".
        
    constructor new literal:l
        <= new literal:l.        
}
