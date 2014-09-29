#define system.

// --- IOException ---

#class(type:IOError) IOException : Exception
{
    #constructor new
    [
        theMessage := "I/O error".
    ]    
}
