#define system.

// --- Constants ---

// --- Locale ---

#subject(class:system'culture'Locale)locale.

#class(sealed) Locale
{
    #field(type:wide)theName.
    
    #constructor new &wide:aName
    [
        theName := aName.
    ]
    
    #constructor current
        <= new &wide:emptyWideLiteralValue.
    
    #constructor invariant
        <= new &wide:$nil.
        
//    #method strToUpper &shortarray:aSrc &int:aLength &shortarray:aDest &vint:aLength
//    [
//    ]
//        
//    #method strToLower &shortarray:aSrc &int:aLength &shortarray:aDest &vint:aLength
//    [
//    ]
}

// --- invariantLocale ---

#static(type:locale) invariantLocale = Locale invariant.

#static(type:locale) currentLocale = Locale current.