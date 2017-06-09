import system'routines.
import system'collections.

// --- DBRow ---

sealed class DBRow
{
    array        theRow.
    sqlite_table theTable.
    
    constructor $new array:aRow sqlite_table:aTable
    [
        theRow := aRow.
        theTable := aTable.
    ]
    
    getAt : aSelector
        = aSelector cast:%eval to:
            {
                eval : aRow int : anInt = aRow getAt int:anInt.
                
                eval : aRow literal : aFieldName = aRow getAt literal:aFieldName.
                
                eval : aRow wide : aFieldName = aRow getAt literal:(aFieldName literal).
            } with:$self.

    stacksafe getAt int:anIndex
        = theRow getAt int:anIndex.

    stacksafe getAt literal:aFieldName
    [
        int anIndex := -1.

        theTable $indexOfField:aFieldName vint:anIndex.
                        
        if (-1 == anIndex)
            [ DBException new message:"Field is unknown"; raise ].
            
        ^ theRow getAt int:anIndex
    ]
}

// --- DBTable ---

sealed class DBTable
{
    array      theFieldNames.
    array_list theRows.
    
    constructor new sql_field_names:aNames
    [
        theFieldNames := aNames array.
        theRows := ArrayList new.
    ]

    $appendRow:aRow
    [
        theRows append:(DBRow $new array:(aRow array) sqlite_table:$self).
    ]

    stacksafe $indexOfField:aFieldName vint:anIndex
    [
        theFieldNames indexOf int:0 object:aFieldName vint:anIndex.
    ]

    sqlite_table = $self.

    rows = theRows.
    
    fields = theFieldNames.
    
    fieldCount = theFieldNames length.
}