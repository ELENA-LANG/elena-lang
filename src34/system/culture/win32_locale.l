// --- Constants ---
const int LCMAP_LOWERCASE = 00000100h.  // lower case letters
const int LCMAP_UPPERCASE = 00000200h.  // UPPER CASE LETTERS

// --- Locale ---

public sealed class Locale
{
    wide theName.
    
    constructor new(wide aName)
    [
        theName := aName.
    ]
    
    constructor current
        <= new(emptyWideLiteral).
    
    constructor invariant
        <= new(nil).
        
    stacksafe strToUpper(ShortArray aSrc, int aLength, ShortArray aDest, ref<int> aDestLength)
    [
        int aRetVal := system'external'KERNEL32 LCMapStringEx(
            theName,
            LCMAP_UPPERCASE,
            aSrc,
            aLength,
            aDest,
            T<int>(aDestLength),
            0,
            0,
            0).
            
        aDestLength value := aRetVal.            
    ]

    stacksafe strToLower(ShortArray aSrc, int aLength, ShortArray aDest, ref<int> aDestLength)
    [
        int aRetVal := system'external'KERNEL32 LCMapStringEx(
            theName,
            LCMAP_UPPERCASE,
            aSrc,
            aLength,
            aDest,
            T<int>(aDestLength),
            0,
            0,
            0).
            
        aDestLength value := aRetVal.            
    ]
}

// --- invariantLocale ---

public static T<Locale> invariantLocale = Locale invariant.

public static T<Locale> currentLocale = Locale current.