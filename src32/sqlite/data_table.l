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
    
    stacksafe getAt int:anIndex
        = theRow getAt int:anIndex.

    stacksafe getAt literal:aFieldName
    [
        int anIndex := -1.

        theTable $indexOfField literal:aFieldName vint:anIndex.
                        
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

    stacksafe $indexOfField literal:aFieldName vint:anIndex
    [
        theFieldNames indexOf int:0 object:aFieldName vint:anIndex.
    ]

    sqlite_table = $self.

    rows = theRows.
    
    fields = theFieldNames.
    
    fieldCount = theFieldNames length.
}