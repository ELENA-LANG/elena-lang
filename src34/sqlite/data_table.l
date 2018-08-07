import system'routines.
import system'collections.

// --- DBRow ---

public sealed class DBRow
{
    T<Array>   theRow.
    T<DBTable> theTable.
    
    internal constructor newTableRow(Array aRow, DBTable aTable)
    [
        theRow := aRow.
        theTable := aTable.
    ]
    
    stacksafe getAt(int anIndex)
        = theRow getAt(anIndex).

    stacksafe getAt(literal aFieldName)
    [
        int anIndex := -1.

        theTable indexOfField(aFieldName, &anIndex).
                        
        if (-1 == anIndex)
            [ DBException new("Field is unknown"); raise ].
            
        ^ theRow getAt(anIndex)
    ]
}

// --- DBTable ---

public sealed class DBTable
{
    T<Array>     theFieldNames.
    T<ArrayList> theRows.
    
    constructor new(Array names)
    [
        theFieldNames := names.
        theRows := ArrayList new.
    ]

    /*internal*/ appendRow:aRow
    [
        theRows append(DBRow newTableRow(aRow, self)).
    ]

    internal sealed indexOfField(literal aFieldName, ref<int> anIndex)
    [
        ^ theFieldNames indexOf(0, aFieldName, anIndex).
    ]

    rows = theRows.
    
    fields = theFieldNames.
    
    fieldCount = theFieldNames length.
}