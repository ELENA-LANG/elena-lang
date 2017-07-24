// --- IOException ---

class WinFormsException :: exceptionof(WinFormsError)
{
    constructor new
        <= new literal:"WinForms error".
        
    constructor new literal:message
    [
        theMessage := message.
        theCallStack := CallStack new.
    ]
}
