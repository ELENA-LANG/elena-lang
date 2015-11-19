// ==== ver 3.4.1 ===

#import system.

// --- IOException ---

#class IOException :: Exception
{
    #constructor new
        <= %new &literal:"I/O error" &subject: %IOError.
        
    #constructor new:aMessage
        <= %new &literal:(aMessage literal) &subject: %IOError.
}
