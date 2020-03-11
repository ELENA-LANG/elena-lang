namespace culture
{
    // --- Constants ---
    const int LCMAP_LOWERCASE = 00000100h;  // lower case letters
    const int LCMAP_UPPERCASE = 00000200h;  // UPPER CASE LETTERS
    
    // --- Locale ---
    
    public sealed class Locale
    {
        wide theName;
        
        constructor new(wide name)
        {
            theName := name
        }
        
        constructor()
        {
            theName := nil
        }
        
        get static Current()
            = Locale.new(emptyWideString);
        
        get static Invariant()
            = new Locale();
            
        strToUpper(short[] src, int length, short[] dest, ref int destLength)
        {
            int retVal := extern KERNEL32.LCMapStringEx(
                theName,
                LCMAP_UPPERCASE,
                src,
                length,
                dest,
                destLength,
                0,
                0,
                0);
                
            destLength := retVal
        }
    
        strToLower(short[] src, int length, short[] dest, ref int destLength)
        {
            int retVal := extern KERNEL32.LCMapStringEx(
                theName,
                LCMAP_LOWERCASE,
                src,
                length,
                dest,
                destLength,
                0,
                0,
                0);
                
            destLength := retVal            
        }
    }
    
    // --- invariantLocale ---
    
    public static Locale invariantLocale = Locale.Invariant;
    
    public static Locale currentLocale = Locale.Current;

}