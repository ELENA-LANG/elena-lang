#define system.

#subject(class:system'BaseFunction1) WinFormsError.

// --- IOException ---

#class WinFormsException :: Exception
{
    #constructor new
        <= %new &message:"WinForms error" &error_type: %WinFormsError.
        
    #constructor new &message:aMessage
        <= %new &message:aMessage &error_type: %IOError.
}
