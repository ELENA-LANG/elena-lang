import system'routines.
import system'collections.
import extensions.

const int SQLITE_OK      = 0.
const int SQLITE_ROW     = 100.
const int SQLITE_DONE    = 101.
const int SQLITE_INTEGER = 1.
const int SQLITE_FLOAT   = 2.
const int SQLITE_TEXT    = 3.
const int SQLITE_BLOB    = 4.
const int SQLITE_NULL    = 5.

// --- DBException ---

class DBException :: Exception
{
    /// Creates the exception
    constructor new
        <= new:"The process terminated".
    
    /// Creates the exception
    constructor new(literal aMessage)
        <= new(aMessage).
}

// --- Connection ---

public sealed class DBConnection
{
    int     theDBHandle.
    literal thePath.
    bool    theOpen.
    
    constructor new(system'io'File p)
        <= new(p path).
    
    constructor new(literal aPath)
    [
        thePath := aPath.
        theOpen := false.
    ]
    
    cast T<int> = theDBHandle.
    
    bool isAvailable = theOpen.
    
    open
    [
        int aHandle := 0.
        int aRetVal := system'external'sqlite3 sqlite3_open(thePath, &aHandle).
        if (aRetVal != SQLITE_OK) [ DBException new:"Could not open a database file"; raise ]; [ theOpen := true ].

        theDBHandle := aHandle
    ]
    
    last_error_message
    [
        dirty_ptr aPtr := system'external'sqlite3 sqlite3_errmsg(theDBHandle).

        ^ aPtr literal
    ]

    doSql:aQuery
        <= executeNonQuery:aQuery.

    executeNonQuery(literal aSqlText)
    [
        ifnot (theOpen) [ DBException new:"SQLite database is not open."; raise ].

        auto aCommand := DBCommand new(self, aSqlText).
        finally (aCommand eval)
            [ aCommand free ]
    ]

    executeQuery(literal aSqlText)
    [
        ifnot (theOpen) [ DBException new:"SQLite database is not open."; raise ].

        auto aCommand := DBCommand new(self, aSqlText).

        var aTable := DBTable new(aCommand sql_field_names).
        var aReader := DBReader new:aCommand.
        
        finally(aReader forEachPair(aTable,(:aTable:aRow)
            [ aTable appendRow:aRow ]))
        [
            aCommand free
        ].

        ^ aTable
    ]
        
    numberOfTable
    [
        ifnot (theOpen) [ DBException new:"SQLite database is not open."; raise ].

        auto aCommand := DBCommand new(self, "SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%';").

        var aNumberOfTable := aCommand evalScalar.
        
        aCommand free.

        ^ aNumberOfTable
    ]

    tables
    [
        ifnot (theOpen) [ DBException new:"SQLite database is not open."; raise ].

        auto aCommand := DBCommand new(self, "SELECT name FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%';").

        auto nameOfTables := ArrayList new.
        
        while (aCommand evalNext)
        [
            nameOfTables append:(aCommand getAt(0)).
        ].
        
        aCommand free.
        
        ^ nameOfTables
    ]

    close
    [
        if (theOpen)
            [ system'external'sqlite3 sqlite3_close(theDBHandle) ].
            
        theOpen := false
    ]
    
    free 
        <= close.    
}

// --- Commnad ---

sealed class DBCommand
{
    T<DBConnection> theConnection.
    T<Handle>       theCommandHandle.
    
    constructor new(DBConnection aConnection, literal aText)
    [
        theConnection := aConnection.
            
        ifnot (theConnection isAvailable) [ DBException new:"SQLite database is not open."; raise ].

        self prepare(aText)
    ]
    
    cast T<Handle> = theCommandHandle.
    
    sealed private prepare(literal aText)
    [
        int aHandle := 0. 
        int aTextLen := aText length.
        
        int aRetVal := system'external'sqlite3 sqlite3_prepare_v2(
                                                T<int>(theConnection),
                                                aText,
                                                aTextLen,
                                                &aHandle,
                                                0).
        if (aRetVal != SQLITE_OK) [ DBException new(theConnection last_error_message); raise ].
        
        theCommandHandle := aHandle
    ]
    
    eval
    [
        int aRetPrc := system'external'sqlite3 sqlite3_step(theCommandHandle).

        if (aRetPrc != SQLITE_DONE) [ DBException new:"Could not execute SQL statement."; raise ]
    ]
    
    bool evalNext
    [
        int aRetPrc := system'external'sqlite3 sqlite3_step(theCommandHandle).

        ^ (aRetPrc == SQLITE_ROW)
    ]
    
    stacksafe evalScalar
    [
        int aRetPrc := system'external'sqlite3 sqlite3_step(theCommandHandle).

        if (aRetPrc != SQLITE_ROW) [ ^ nil ].

        ^ self getAt(0)
    ]
    
    T<DBReader> reader = DBReader new(self).
    
    T<Enumerator> enumerator = sqlite'DBReader new(self).
    
    stacksafe getAt(int anIndex)
    [
        int columnType := system'external'sqlite3 sqlite3_column_type(theCommandHandle, anIndex).
        
        columnType =>
            SQLITE_INTEGER 
                [
                    int val := system'external'sqlite3 sqlite3_column_int(theCommandHandle, anIndex).
                    
                    ^ IntNumber new(val)
                ];
            SQLITE_TEXT
                [
                    dirty_ptr aColumnTextPtr := system'external'sqlite3 sqlite3_column_text(theCommandHandle, anIndex).
        
                    ^ aColumnTextPtr literal
                ];
            SQLITE_FLOAT
                [
                    real val := system'external'sqlite3 sqlite3_column_double(theCommandHandle, anIndex).
                    
                    ^ RealNumber new(val).
                ];
            SQLITE_BLOB
                [
                    dirty_ptr blob := system'external'sqlite3 sqlite3_column_blob(theCommandHandle, anIndex).
                    int len := system'external'sqlite3 sqlite3_column_bytes(theCommandHandle, anIndex).
                    
                    T<ByteArray> arr := ByteArray new(len).
        
                    blob read(arr, len).
        
                    ^ arr
                ];
            SQLITE_NULL
                [
                    ^ nil
                ];
            !
                [
                    DBException new:"Unsupported type"; raise
                ]
    ]
    
    stacksafe readFieldCount(ref<int> aCount)
    [
        int ret := system'external'sqlite3 sqlite3_column_count(theCommandHandle).
        
        aCount value := ret
    ]
    
    T<Array> sql_field_names
    [
        int field_count := 0.
        self readFieldCount(&field_count).
        
        auto names := Array new(field_count).
        int i := 0.
        while (i < field_count)
        [
            dirty_ptr aColumnNamePtr := system'external'sqlite3 sqlite3_column_origin_name(theCommandHandle, i).
            
            names setAt(i, aColumnNamePtr literal).
            
            i := i + 1
        ].
        
        ^ names
    ]
    
    free
    [
        int aRetVal := system'external'sqlite3 sqlite3_finalize(theCommandHandle).
        
        if (aRetVal != SQLITE_OK) [ DBException new:"Could not finalize SQL statement."; raise ].
    ]
}
