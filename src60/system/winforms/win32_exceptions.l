import system;

// --- IOException ---

public class WinFormsException : Exception
{
    constructor new()
        <= super new("WinForms error");
}
