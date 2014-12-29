#define system.

#type(class:system'BaseFunction1) IOError.

// --- IOException ---

#class IOException :: Exception
{
    #constructor new
        <= %new &message:"The process terminated" &error_type: %IOError.
}
