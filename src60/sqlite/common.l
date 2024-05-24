import system'routines;
import system'collections;
import extensions;

const int SQLITE_OK      = 0;
const int SQLITE_ROW     = 100;
const int SQLITE_DONE    = 101;
const int SQLITE_INTEGER = 1;
const int SQLITE_FLOAT   = 2;
const int SQLITE_TEXT    = 3;
const int SQLITE_BLOB    = 4;
const int SQLITE_NULL    = 5;

// --- DBException ---

public class DBException : Exception
{
   /// Creates the exception
   constructor new()
     <= new("The process terminated");
   
   /// Creates the exception
   constructor new(string message)
     <= super new(message);
}

// --- Connection ---

public sealed class DBConnection
{
   handle  _DBHandle;
   string  _path;
   bool    _open;
   
   constructor new(system'io'File p)
      <= new(cast string(p));
   
   constructor new(string path)
   {
      _path := path;
      _open := false;
   }
   
   handle cast() = _DBHandle;
   
   bool isAvailable() = _open;
   
   open()
   {
      int retVal := extern sqlite3.sqlite3_open(_path, out handle h);
      if (retVal != SQLITE_OK) { DBException.new("Could not open a database file").raise() } else { _open := true };
      
      _DBHandle := h
   }
   
   string last_error_message()
   {
      pointer ptr := extern sqlite3.sqlite3_errmsg(_DBHandle);
      
      ^ cast string(ptr)
   }
   
   doSql(query)
     <= executeNonQuery(query);
   
   executeNonQuery(string sqlText)
   {
      ifnot (_open) { DBException.new("SQLite database is not open.").raise() };
      
      using(auto command := DBCommand.new(self, sqlText))
      {
         command.eval()
      }
   }
   
   executeQuery(string sqlText)
   {
      ifnot (_open) { DBException.new("SQLite database is not open.").raise() };
      
      auto command := DBCommand.new(self, sqlText);
      
      var table := DBTable.new(command.sql_field_names());
      using(auto reader := DBReader.new(command))
      {
         auto en := reader.enumerator();
         while (en.next()) {
            table.appendRow(*en)
         }
      };
      
      ^ table
   }
     
   numberOfTable()
   {
      ifnot (_open) { DBException.new("SQLite database is not open.").raise() };
      
      auto command := DBCommand.new(self, "SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%';");
      
      var numberOfTable := command.evalScalar();
      
      command.close();
      
      ^ numberOfTable
   }
   
   tables()
   {
      ifnot (_open) { DBException.new("SQLite database is not open.").raise() };
      
      auto command := DBCommand.new(self, "SELECT name FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%';");
      
      auto nameOfTables := new ArrayList();
      
      while (command.evalNext())
      {
         nameOfTables.append(command.at(0))
      };
      
      command.close();
      
      ^ nameOfTables
   }
   
   close()
   {
      if (_open)
         { extern sqlite3.sqlite3_close(_DBHandle) };
         
      _open := false
   }
}

// --- Commnad ---

public sealed class DBCommand
{
   DBConnection _connection;
   Handle       _commandHandle;
   
   constructor new(DBConnection connection, string text)
   {
      _connection := connection;
         
      ifnot (_connection.isAvailable()) { DBException.new("SQLite database is not open.").raise() };
      
      self.prepare(text)
   }
   
   Handle cast() = _commandHandle;
   
   private prepare(string text)
   {
      handle cnn := _connection;
      int textLen := text.Length;
      
      int retVal := extern sqlite3.sqlite3_prepare_v2(
                                             cnn,
                                             text,
                                             textLen,
                                             out handle h,
                                             0);
      if (retVal != SQLITE_OK) { DBException.new(_connection.last_error_message()).raise() };
      
      _commandHandle := h
   }
   
   eval()
   {
      int retPrc := extern sqlite3.sqlite3_step(_commandHandle);
      
      if (retPrc != SQLITE_DONE) { DBException.new("Could not execute SQL statement.").raise() }
   }
   
   bool evalNext()
   {
      int retPrc := extern sqlite3.sqlite3_step(_commandHandle);
      
      ^ (retPrc == SQLITE_ROW)
   }
   
   evalScalar()
   {
      int retPrc := extern sqlite3.sqlite3_step(_commandHandle);
      
      if (retPrc != SQLITE_ROW) { ^ nil };
      
      ^ self.at(0)
   }
   
   DBReader reader() = DBReader.new(self);
   
   Enumerator enumerator() = sqlite'DBReader.new(self).enumerator();
   
   at(int index)
   {
      int columnType := extern sqlite3.sqlite3_column_type(_commandHandle, index);
      
      columnType =>
         SQLITE_INTEGER
             {
                 int val := extern sqlite3.sqlite3_column_int(_commandHandle, index);
                 
                 ^ val
             }
         SQLITE_TEXT
             {
                 pointer columnTextPtr := extern sqlite3.sqlite3_column_text(_commandHandle, index);
      
                 ^ cast string(columnTextPtr)
             }
         SQLITE_FLOAT
             {
                 real val := extern sqlite3.sqlite3_column_double(_commandHandle, index);
                 
                 ^ val
             }
         SQLITE_BLOB
             {
                 pointer blob := extern sqlite3.sqlite3_column_blob(_commandHandle, index);
                 int len := extern sqlite3.sqlite3_column_bytes(_commandHandle, index);
                 
                 byte[] arr := new byte[](len);
      
                 blob.copyTo(arr, len);
      
                 ^ arr
             }
         SQLITE_NULL
             {
                 ^ nil
             }
         !
             {
                 DBException.new("Unsupported type").raise()
             }
   }
   
   int readFieldCount()
   {
      int ret := extern sqlite3.sqlite3_column_count(_commandHandle);
      
      ^ ret
   }
   
   string[] sql_field_names()
   {
      int field_count := self.readFieldCount();
      
      auto names := new string[](field_count);
      for(int i := 0; i < field_count; i++)
      {
         pointer columnNamePtr := extern sqlite3.sqlite3_column_origin_name(_commandHandle, i);
         
         names[i] := cast string(columnNamePtr)
      };
      
      ^ names
   }
   
   close()
   {
      int retVal := extern sqlite3.sqlite3_finalize(_commandHandle);
      
      if (retVal != SQLITE_OK) { DBException.new("Could not finalize SQL statement.").raise() };
   }
}
