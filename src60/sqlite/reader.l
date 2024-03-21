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
   DBCommand _command;
   Handle    _commandHandle;
   string[]  _fieldNames;
   
   constructor new(DBCommand command)
   {
      _command := command;
     
      _commandHandle := cast Handle(command);
   }
   
   int readNext()
   {
      int r := extern sqlite3.sqlite3_step(_commandHandle);
     
      ^ r
   }
   
   bool next()
   {
      int result := self.readNext();
     
      ^ result == SQLITE_ROW
   }
   
   at(int n)
      = _command.at(n);
   
   at(string fieldName)
   {
      if (nil == _fieldNames)
         { _fieldNames := _command.sql_field_names() };
         
      ^ self.at(_fieldNames.indexOfElement(fieldName))
   }
   
   object[] row()
   {
      int count := _command.readFieldCount();
     
      object[] row := new object[](count);
      for(int i := 0; i < count; i++)
      {
         row[i] := self.at(i)
      };
     
      ^ row
   }
   
   Enumerator<object[]> enumerator() = new DBEnumerator(self);
   
   close()
   {
      _command.close()
   }
}
