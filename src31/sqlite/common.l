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

type sqlite_cnn    :: DBConnection.
type sqlite_cmd    :: DBCommand.
type sqlite_reader :: DBReader.
type sqlite_table  :: DBTable.
type sqlite_row    :: DBRow.

// --- DBException ---

class DBException :: exceptionof(DBError)
{
    constructor new literal:message
    [
        theMessage := message.
        theCallStack := CallStack new.
    ]
    
    /// Creates the exception
    constructor new
        <= new literal:"The process terminated".
    
    /// Creates the exception
    constructor new message:aMessage
        <= new literal:aMessage.
}

// --- Connection ---

sealed class DBConnection
{
    int     theDBHandle.
    literal thePath.
    bool    theOpen.
    
    constructor new path:aPath
    [
        thePath := aPath literal.
        theOpen := false.
    ]
    
    int = theDBHandle.
    
    bool is available = theOpen.
    
    open
    [
        int aHandle := 0.
        int aRetVal := system'external'sqlite3 sqlite3_open literal:thePath vint:aHandle.
        if (aRetVal != SQLITE_OK) [ DBException new message:"Could not open a database file"; raise ]; [ theOpen := true ].

        theDBHandle := aHandle
    ]
    
    last_error_message
    [
        dirty_ptr aPtr := system'external'sqlite3 sqlite3_errmsg int:theDBHandle.

        ^ aPtr literal
    ]

    do sql:aQuery
        <= executeNonQuery:aQuery.

    executeNonQuery : aSqlText
    [
        ifnot (theOpen) [ DBException new message:"SQLite database is not open."; raise ].

        sqlite_cmd aCommand := DBCommand new sqlite_cnn:$self sql:aSqlText.
        finally (aCommand eval)
            [ aCommand free ]
    ]

    executeQuery : aSqlText
    [
        ifnot (theOpen) [ DBException new message:"SQLite database is not open."; raise ].

        sqlite_cmd aCommand := DBCommand new sqlite_cnn:$self sql:aSqlText.

        var aTable := DBTable new sql_field_names:(aCommand sql_field_names).
        var aReader := DBReader new sqlite_cmd:aCommand.
        
        finally(aReader run:aTable eachPair:(:aTable:aRow)
            [ aTable $appendRow:aRow ])
        [
            aCommand free
        ].

        ^ aTable
    ]
        
    numberOfTable
    [
        ifnot (theOpen) [ DBException new message:"SQLite database is not open."; raise ].

        sqlite_cmd aCommand := DBCommand new sqlite_cnn:$self sql:"SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%';".

        var aNumberOfTable := aCommand evalScalar.
        
        aCommand free.

        ^ aNumberOfTable
    ]

    tables
    [
        ifnot (theOpen) [ DBException new message:"SQLite database is not open."; raise ].

        sqlite_cmd aCommand := DBCommand new sqlite_cnn:$self sql:"SELECT name FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%';".

        array_list nameOfTables := ArrayList new.
        
        while (aCommand evalNext)
        [
            nameOfTables append:(aCommand getAt int:0).
        ].
        
        aCommand free.
        
        ^ nameOfTables
    ]

    close
    [
        if (theOpen)
            [ system'external'sqlite3 sqlite3_close int:theDBHandle ].
            
        theOpen := false
    ]
    
    free 
        <= close.    
}

// --- Commnad ---

sealed class DBCommand
{
    sqlite_cnn theConnection.
    handle     theCommandHandle.
    
    constructor new sqlite_cnn:aConnection sql:aText
    [
        theConnection := aConnection.
            
        ifnot (theConnection is available) [ DBException new message:"SQLite database is not open."; raise ].

        $self $prepare literal:(aText literal)
    ]
    
    sqlite_cmd = $self.
    
    handle = theCommandHandle.
    
    $prepare literal:aText
    [
        int aHandle := 0. 
        int aTextLen := aText length.
        
        int aRetVal := system'external'sqlite3 sqlite3_prepare_v2
                                                int:(theConnection int)
                                                literal:aText
                                                int:aTextLen
                                                vint:aHandle
                                                int:0.
        if (aRetVal != SQLITE_OK) [ DBException new message:(theConnection last_error_message); raise ].
        
        theCommandHandle := aHandle
    ]
    
    eval
    [
        int aRetPrc := system'external'sqlite3 sqlite3_step int:theCommandHandle.

        if (aRetPrc != SQLITE_DONE) [ DBException new message:"Could not execute SQL statement."; raise ]
    ]
    
    bool evalNext
    [
        int aRetPrc := system'external'sqlite3 sqlite3_step int:theCommandHandle.

        ^ (aRetPrc == SQLITE_ROW)
    ]
    
    stacksafe evalScalar
    [
        int aRetPrc := system'external'sqlite3 sqlite3_step int:theCommandHandle.

        if (aRetPrc != SQLITE_ROW) [ ^ $nil ].

        ^ $self getAt int:0  
    ]
    
    sqlite_reader = sqlite'DBReader new sqlite_cmd:$self; sqlite_reader.
    
    enumerator = sqlite'DBReader new sqlite_cmd:$self; enumerator.
    
    stacksafe getAt int:anIndex
    [
        int columnType := system'external'sqlite3 sqlite3_column_type int:theCommandHandle int:anIndex.
        
        columnType =>
            SQLITE_INTEGER 
                [
                    int val := system'external'sqlite3 sqlite3_column_int int:theCommandHandle int:anIndex.
                    
                    ^ IntNumber new int:val
                ];
            SQLITE_TEXT
                [
                    dirty_ptr aColumnTextPtr := system'external'sqlite3 sqlite3_column_text int:theCommandHandle int:anIndex.
        
                    ^ aColumnTextPtr literal
                ];
            SQLITE_FLOAT
                [
                    real val := system'external'sqlite3 sqlite3_column_double int:theCommandHandle int:anIndex.
                    
                    ^ RealNumber new real:val.
                ];
            SQLITE_BLOB
                [
                    dirty_ptr blob := system'external'sqlite3 sqlite3_column_blob int:theCommandHandle int:anIndex.
                    int len := system'external'sqlite3 sqlite3_column_bytes int:theCommandHandle int:anIndex.
                    
                    bytearray arr := ByteArray new int:len.
        
                    blob read bytearray:arr int:len.
        
                    ^ arr
                ];
            SQLITE_NULL
                [
                    ^ $nil
                ];
            !
                [
                    DBException new message:"Unsupported type"; raise
                ]
    ]
    
    stacksafe readFieldCount vint:aCount
    [
        int ret := system'external'sqlite3 sqlite3_column_count int:theCommandHandle.
        
        aCount int := ret
    ]
    
    array sql_field_names
    [
        int field_count := 0.
        $self readFieldCount vint:field_count.
        
        array names := Array new int:field_count.
        int i := 0.
        while (i < field_count)
        [
            dirty_ptr aColumnNamePtr := system'external'sqlite3 sqlite3_column_origin_name int:theCommandHandle int:i.
            
            names setAt int:i object:(aColumnNamePtr literal).
            
            i := i + 1
        ].
        
        ^ names
    ]
    
    free
    [
        int aRetVal := system'external'sqlite3 sqlite3_finalize int:theCommandHandle.
        
        if (aRetVal != SQLITE_OK) [ DBException new message:"Could not finalize SQL statement."; raise ].
    ]
}
