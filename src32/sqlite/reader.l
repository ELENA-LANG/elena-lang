import system'routines.

sealed class DBReader
{
    sqlite_cmd theCommand.
    handle     theCommandHandle.
    array      theFieldNames.
    
    constructor new sqlite_cmd:aCommand
    [
        theCommand := aCommand.
        
        theCommandHandle := aCommand handle.
    ]
    
    sqlite_reader = $self.
    
    stacksafe readNext vint:aResult
    [
        int aRetVal := system'external'sqlite3 sqlite3_step int:theCommandHandle.
        
        aResult int := aRetVal
    ]
    
    bool next
    [
        int aResult := 0.
        $self readNext vint:aResult.
        
        ^ aResult == SQLITE_ROW
    ]
    
    stacksafe getAt int:anIndex
        = theCommand getAt int:anIndex.
    
    getAt literal:aFieldName
    [
        if ($nil == theFieldNames)
            [ theFieldNames := theCommand sql_field_names ].
            
        ^ $self getAt int:(theFieldNames indexOf:aFieldName; int).
    ]
    
    row
    [
        int aCount := 0.
        theCommand readFieldCount vint:aCount.
        
        array aRow := Array new int:aCount.
        int i := 0.
        while (i < aCount)
        [
            aRow setAt int:i object:($self getAt int:i).
            
            i := i + 1
        ].
        
        ^ aRow
    ]
    
    enumerator = Enumerator::
    {
        get = $owner row.
    
        next => $owner.
    }.
}
