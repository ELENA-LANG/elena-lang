// ==== ver 3.4.1 ===

#import system.
#import system'routines.
#import system'collections.

// --- DBRow ---

#class(sealed) DBRow
{
    #field(type:array)        theRow.
    #field(type:sqlite_table) theTable.
    
    #constructor $new &array:aRow &sqlite_table:aTable
    [
        theRow := aRow.
        theTable := aTable.
    ]
    
    #method getAt : aSelector
        = aSelector cast:%eval &to:
            {
                eval : aRow &int : anInt = aRow getAt &int:anInt.
                
                eval : aRow &literal : aFieldName = aRow getAt &literal:aFieldName.
                
                eval : aRow &wide : aFieldName = aRow getAt &literal:(aFieldName literal).
            } &with:$self.

    #method(stacksafe) getAt &int:anIndex
        = theRow getAt &int:anIndex.

    #method(stacksafe) getAt &literal:aFieldName
    [
        #var(type:int)anIndex := -1.

        theTable $indexOfField:aFieldName &vint:anIndex.
                        
        (-1 == anIndex)
            ? [ #throw DBException new &message:"Field is unknown". ].
            
        ^ theRow getAt &int:anIndex.
    ]
}

// --- DBTable ---

#class(sealed) DBTable
{
    #field(type:array)theFieldNames.
    #field(type:array_list) theRows.
    
    #constructor new &sql_field_names:aNames
    [
        theFieldNames := aNames array.
        theRows := ArrayList new.
    ]

    #method $appendRow:aRow
    [
        theRows append:(DBRow $new &array:(aRow array) &sqlite_table:$self).
    ]

    #method(stacksafe) $indexOfField:aFieldName &vint:anIndex
    [
        theFieldNames indexOf &int:0 &object:aFieldName &vint:anIndex.
    ]

    #method sqlite_table = $self.

    #method rows = theRows.
    
    #method fields = theFieldNames.
    
    #method fieldCount = theFieldNames length.
}