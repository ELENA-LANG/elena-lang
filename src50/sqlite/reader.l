import system'routines;

public sealed class DBEnumerator : Enumerator<object[]>
{
    DBReader reader;
    
    constructor(DBReader reader)
    {
        this reader := reader
    }
    
    get object[] Value() = reader.row();

    bool next() => reader;
    
    enumerable() = reader;
    
    reset() { NotSupportedException.raise() }
    
    Enumerator cast() = new Enumerator{ embeddable dispatch() => self; };
}

public sealed class DBReader : Enumerable<object[]>
{
    DBCommand theCommand;
    Handle    theCommandHandle;
    string[]  theFieldNames;
    
    constructor new(DBCommand command)
    {
        theCommand := command;
        
        theCommandHandle := cast Handle(command);
    }
    
    int readNext()
    {
        int r := extern sqlite3.sqlite3_step(theCommandHandle);
        
        ^ r
    }
    
    bool next()
    {
        int result := self.readNext();
        
        ^ result == SQLITE_ROW
    }
    
    at(int n)
        = theCommand.at(n);
    
    at(string fieldName)
    {
        if (nil == theFieldNames)
            { theFieldNames := theCommand.sql_field_names() };
            
        ^ self.at(theFieldNames.indexOfElement(fieldName))
    }
    
    object[] row()
    {
        int count := theCommand.readFieldCount();
        
        object[] row := new object[](count);
        for(int i := 0, i < count, i += 1)
        {
            row[i] := self.at(i)
        };
        
        ^ row
    }
    
    Enumerator<object[]> enumerator() = new DBEnumerator(self);
    
    close()
    {
        theCommand.close()
    }
}
