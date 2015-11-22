// ==== ver 3.4.1 ===

#import system.

// --- IOException ---

#class WinFormsException :: Exception
{
    #constructor new
        <= %new &literal:"WinForms error" &subject: %WinFormsError.
        
    #constructor new:aMessage
        <= %new &literal:(aMessage literal) &subject: %IOError.
}
