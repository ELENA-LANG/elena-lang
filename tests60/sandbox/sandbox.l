public sealed class DBReader : Enumerable<object[]>
{
//   DBCommand _command;
//   Handle    _commandHandle;
//   string[]  _fieldNames;
//   
//   constructor new(DBCommand command)
//   {
//      theCommand := command;
//     
//      theCommandHandle := cast Handle(command);
//   }
//   
//   int readNext()
//   {
//      int r := extern sqlite3.sqlite3_step(theCommandHandle);
//     
//      ^ r
//   }
//   
//   bool next()
//   {
//      int result := self.readNext();
//     
//      ^ result == SQLITE_ROW
//   }
//   
//   at(int n)
//      = theCommand.at(n);
//   
//   at(string fieldName)
//   {
//      if (nil == theFieldNames)
//         { theFieldNames := theCommand.sql_field_names() };
//         
//      ^ self.at(theFieldNames.indexOfElement(fieldName))
//   }
//   
//   object[] row()
//   {
//      int count := theCommand.readFieldCount();
//     
//      object[] row := new object[](count);
//      for(int i := 0; i < count; i++)
//      {
//         row[i] := self.at(i)
//      };
//     
//      ^ row
//   }
   
   Enumerator<object[]> enumerator() = new DBEnumerator(self);
}


public program()
{
}