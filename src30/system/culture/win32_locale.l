#import system.

// --- Constants ---
#symbol(int,const) LCMAP_LOWERCASE = 00000100h.  // lower case letters
#symbol(int,const) LCMAP_UPPERCASE = 00000200h.  // UPPER CASE LETTERS

// --- Locale ---

#subject locale = system'culture'Locale.

#class(sealed) Locale
{
    #field(wide)theName.
    
    #constructor new &wide:aName
    [
        theName := aName.
    ]
    
    #constructor current
        <= new &wide:emptyWideLiteralValue.
    
    #constructor invariant
        <= new &wide:$nil.
        
    #method(stacksafe) strToUpper &shortarray:aSrc &int:aLength &shortarray:aDest &vint:aDestLength
    [
        #var(int)aRetVal := system'external'KERNEL32 LCMapStringEx
            &wide:theName
            &int:LCMAP_UPPERCASE
            &shortarray:aSrc
            &int:aLength
            &shortarray:aDest
            &int:aDestLength
            &int:0
            &int:0
            &int:0.
            
        aDestLength := aRetVal.            
    ]

    #method(stacksafe) strToLower &shortarray:aSrc &int:aLength &shortarray:aDest &vint:aDestLength
    [
        #var(int)aRetVal := system'external'KERNEL32 LCMapStringEx
            &wide:theName
            &int:LCMAP_LOWERCASE
            &shortarray:aSrc
            &int:aLength
            &shortarray:aDest
            &int:aDestLength
            &int:0
            &int:0
            &int:0.
            
        aDestLength := aRetVal.            
    ]
}

// --- invariantLocale ---

#static(locale) invariantLocale = Locale invariant.

#static(locale) currentLocale = Locale current.