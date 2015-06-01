#define system.

#subject(class:system'BaseFunction1) IOError.

// --- IOException ---

#class IOException :: Exception
{
    #constructor new
        <= %new &message:"I/O error" &error_type: %IOError.
        
    #constructor new &message:aMessage
        <= %new &message:aMessage &error_type: %IOError.
}
