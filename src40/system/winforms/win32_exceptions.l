// --- IOException ---

public class WinFormsException : Exception
{
    constructor new()
        <= new("WinForms error");
        
    static raise()
    {
        WinFormsException.new().raise()
    }            
}
