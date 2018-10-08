import system'routines.

public sealed class DBReader :: Enumerable
{
    T<DBCommand> theCommand.
    T<Handle>    theCommandHandle.
    T<Array>     theFieldNames.
    
    constructor new(DBCommand aCommand)
    [
        theCommand := aCommand.
        
        theCommandHandle := T<Handle>(aCommand).
    ]
    
    readNext(ref<int> aResult)
    [
        int aRetVal := system'external'sqlite3 sqlite3_step(theCommandHandle).
        
        aResult value := aRetVal
    ]
    
    bool next
    [
        int aResult := 0.
        self readNext(&aResult).
        
        ^ aResult == SQLITE_ROW
    ]
    
    getAt(int anIndex)
        = theCommand getAt(anIndex).
    
    getAt(literal aFieldName)
    [
        if (nil == theFieldNames)
            [ theFieldNames := theCommand sql_field_names ].
            
        ^ self getAt(T<int>(theFieldNames indexOf:aFieldName)).
    ]
    
    row
    [
        int aCount := 0.
        theCommand readFieldCount(&aCount).
        
        T<Array> aRow := Array new(aCount).
        int i := 0.
        while (i < aCount)
        [
            aRow setAt(i, self getAt(i)).
            
            i := i + 1
        ].
        
        ^ aRow
    ]
    
    T<Enumerator> enumerator = Enumerator::
    {
        get = self row.
    
        bool next => self.
        
        enumerable = self.
        
        reset [ NotSupportedException new; raise ]
    }.
}
