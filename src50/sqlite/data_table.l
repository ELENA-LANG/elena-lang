import system'routines;
import system'collections;

// --- DBRow ---

public sealed class DBRow
{
    object[] theRow;
    DBTable  theTable;
    
    internal constructor newTableRow(object[] row, DBTable table)
    {
        theRow := row;
        theTable := table
    }
    
    at(int n)
        = theRow.at(n);

    at(string fieldName)
    {
        int index := theTable.indexOfField(fieldName);
                        
        if (-1 == index)
            { DBException.new("Field is unknown").raise() };
            
        ^ theRow[index]
    }
}

// --- DBTable ---

public sealed class DBTable
{
    string[]     theFieldNames;
    List<DBRow>  theRows;
    
    constructor new(string[] names)
    {
        theFieldNames := names;
        theRows := new List<DBRow>()
    }

    /*internal*/ appendRow(object[] row)
    {
        theRows.append(DBRow.newTableRow(row, self))
    }

    internal int indexOfField(string fieldName)
    {
        ^ theFieldNames.indexOfElement(fieldName)
    }

    DBRow[] rows() = theRows.Value;
    
    string[] fields() = theFieldNames;
    
    int fieldCount() = theFieldNames.Length;
}