#import system.

// --- Constants ---
#symbol(type:int,const) LCMAP_LOWERCASE = 00000100h.  // lower case letters
#symbol(type:int,const) LCMAP_UPPERCASE = 00000200h.  // UPPER CASE LETTERS

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
        
    #method(stacksafe) strToUpper &shortarray:aSrc &int:aLength &shortarray:aDest &vint:aDestLength
    [
        #var(type:int)aRetVal := system'external'KERNEL32 LCMapStringEx
            &wide:theName
            &int:LCMAP_UPPERCASE
            &shortarray:aSrc
            &int:aLength
            &shortarray:aDest
            &int:aDestLength
            &int:0
            &int:0
            &int:0.
            
        aDestLength << aRetVal.            
    ]

    #method(stacksafe) strToLower &shortarray:aSrc &int:aLength &shortarray:aDest &vint:aDestLength
    [
        #var(type:int)aRetVal := system'external'KERNEL32 LCMapStringEx
            &wide:theName
            &int:LCMAP_LOWERCASE
            &shortarray:aSrc
            &int:aLength
            &shortarray:aDest
            &int:aDestLength
            &int:0
            &int:0
            &int:0.
            
        aDestLength << aRetVal.            
    ]
}

// --- invariantLocale ---

#static(type:locale) invariantLocale = Locale invariant.

#static(type:locale) currentLocale = Locale current.