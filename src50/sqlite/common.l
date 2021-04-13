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
        <= new(message);
}

// --- Connection ---

public sealed class DBConnection
{
    int     theDBHandle;
    string  thePath;
    bool    theOpen;
    
    constructor new(system'io'File p)
        <= new(cast string(p));
    
    constructor new(string path)
    {
        thePath := path;
        theOpen := false;
    }
    
    int cast() = theDBHandle;
    
    bool isAvailable() = theOpen;
    
    open()
    {
        int retVal := extern sqlite3.sqlite3_open(thePath, ref int handle);
        if (retVal != SQLITE_OK) { DBException.new:"Could not open a database file".raise() } else { theOpen := true };

        theDBHandle := handle
    }
    
    string last_error_message()
    {
        dirty_ptr ptr := extern sqlite3.sqlite3_errmsg(theDBHandle);

        ^ cast string(ptr)
    }

    doSql(query)
        <= executeNonQuery(query);

    executeNonQuery(string sqlText)
    {
        ifnot (theOpen) { DBException.new:"SQLite database is not open.".raise() };

        using(auto command := DBCommand.new(self, sqlText))
        {
            command.eval()
        }
    }

    executeQuery(string sqlText)
    {
        ifnot (theOpen) { DBException.new:"SQLite database is not open.".raise() };

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
        ifnot (theOpen) { DBException.new:"SQLite database is not open.".raise() };

        auto command := DBCommand.new(self, "SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%';");

        var numberOfTable := command.evalScalar();
        
        command.close();

        ^ numberOfTable
    }

    tables()
    {
        ifnot (theOpen) { DBException.new:"SQLite database is not open.".raise() };

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
        if (theOpen)
            { extern sqlite3.sqlite3_close(theDBHandle) };
            
        theOpen := false
    }
}

// --- Commnad ---

public sealed class DBCommand
{
    DBConnection theConnection;
    Handle       theCommandHandle;
    
    constructor new(DBConnection connection, string text)
    {
        theConnection := connection;
            
        ifnot (theConnection.isAvailable()) { DBException.new:"SQLite database is not open.".raise() };

        self.prepare(text)
    }
    
    Handle cast() = theCommandHandle;
    
    private prepare(string text)
    {
        int cnn := cast int(theConnection);
        int handle := 0; 
        int textLen := text.Length;
        
        int retVal := extern sqlite3.sqlite3_prepare_v2(
                                                cnn,
                                                text,
                                                textLen,
                                                ref handle,
                                                0);
        if (retVal != SQLITE_OK) { DBException.new(theConnection.last_error_message()).raise() };
        
        theCommandHandle := handle
    }
    
    eval()
    {
        int retPrc := extern sqlite3.sqlite3_step(theCommandHandle);

        if (retPrc != SQLITE_DONE) { DBException.new:"Could not execute SQL statement.".raise() }
    }
    
    bool evalNext()
    {
        int retPrc := extern sqlite3.sqlite3_step(theCommandHandle);

        ^ (retPrc == SQLITE_ROW)
    }
    
    evalScalar()
    {
        int retPrc := extern sqlite3.sqlite3_step(theCommandHandle);

        if (retPrc != SQLITE_ROW) { ^ nil };

        ^ self.at(0)
    }
    
    DBReader reader() = DBReader.new(self);
    
    Enumerator enumerator() = sqlite'DBReader.new(self);
    
    at(int index)
    {
        int columnType := extern sqlite3.sqlite3_column_type(theCommandHandle, index);
        
        columnType =>
            SQLITE_INTEGER
                {
                    int val := extern sqlite3.sqlite3_column_int(theCommandHandle, index);
                    
                    ^ val
                }
            SQLITE_TEXT
                {
                    dirty_ptr columnTextPtr := extern sqlite3.sqlite3_column_text(theCommandHandle, index);
        
                    ^ cast string(columnTextPtr)
                }
            SQLITE_FLOAT
                {
                    real val := extern sqlite3.sqlite3_column_double(theCommandHandle, index);
                    
                    ^ val
                }
            SQLITE_BLOB
                {
                    dirty_ptr blob := extern sqlite3.sqlite3_column_blob(theCommandHandle, index);
                    int len := extern sqlite3.sqlite3_column_bytes(theCommandHandle, index);
                    
                    byte[] arr := new byte[](len);
        
                    blob.read(arr, len);
        
                    ^ arr
                }
            SQLITE_NULL
                {
                    ^ nil
                }
            :
                {
                    DBException.new:"Unsupported type".raise()
                }
    }
    
    int readFieldCount()
    {
        int ret := extern sqlite3.sqlite3_column_count(theCommandHandle);
        
        ^ ret
    }
    
    string[] sql_field_names()
    {
        int field_count := self.readFieldCount();
        
        auto names := new string[](field_count);
        for(int i := 0, i < field_count, i += 1)
        {
            dirty_ptr columnNamePtr := extern sqlite3.sqlite3_column_origin_name(theCommandHandle, i);
            
            names[i] := cast string(columnNamePtr)
        };
        
        ^ names
    }
    
    close()
    {
        int retVal := extern sqlite3.sqlite3_finalize(theCommandHandle);
        
        if (retVal != SQLITE_OK) { DBException.new:"Could not finalize SQL statement.".raise() };
    }
}
