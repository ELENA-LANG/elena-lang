#import system.
#import system'routines.
#import system'collections.
#import extensions.

#symbol(const,int)SQLITE_OK = 0.
#symbol(const,int)SQLITE_ROW = 100.
#symbol(const,int)SQLITE_DONE = 101.
#symbol(const,int)SQLITE_INTEGER = 1.
#symbol(const,int)SQLITE_FLOAT = 2.
#symbol(const,int)SQLITE_TEXT = 3.
#symbol(const,int)SQLITE_BLOB = 4.
#symbol(const,int)SQLITE_NULL = 5.

#subject sqlite_cnn    = DBConnection.
#subject sqlite_cmd    = DBCommand.
#subject sqlite_reader = DBReader.
#subject sqlite_table  = DBTable.
#subject sqlite_row    = DBRow.

// --- DBException ---

#class(exceptionof:DBError) DBException
{
    #constructor new &literal:message
    [
        theMessage := message.
        theCallStack := CallStack new.
    ]
    
    /// Creates the exception
    #constructor new
        <= new &literal:"The process terminated".
    
    /// Creates the exception
    #constructor new &message:aMessage
        <= new &literal:(aMessage literal).
}

// --- Connection ---

#class(sealed) DBConnection
{
    #field(int)     theDBHandle.
    #field(literal) thePath.
    #field(bool)    theOpen.
    
    #constructor new &path:aPath
    [
        thePath := aPath literal.
        theOpen := false.
    ]
    
    #method int = theDBHandle.
    
    #method(bool) is &available = theOpen.
    
    #method open
    [
        #var(int)aHandle.
        #var(int) aRetVal := system'external'sqlite3 sqlite3_open &literal:thePath &vint:aHandle.
        (aRetVal != SQLITE_OK)? [ #throw DBException new &message:"Could not open a database file". ] ! [ theOpen := true. ].

        theDBHandle := aHandle.
    ]
    
    #method last_error_message
    [
        #var(dirty_ptr)aPtr := system'external'sqlite3 sqlite3_errmsg &int:theDBHandle.

        ^ aPtr literal.
    ]

    #method do &sql:aQuery
        <= %executeNonQuery:aQuery.

    #method executeNonQuery : aSqlText
    [
        theOpen ! [ #throw DBException new &message:"SQLite database is not open.". ].

        #var(sqlite_cmd)aCommand := DBCommand new &sqlite_cnn:$self &sql:aSqlText.
        
        aCommand eval.
        
        aCommand free.
    ]

    #method executeQuery : aSqlText
    [
        theOpen ! [ #throw DBException new &message:"SQLite database is not open.". ].

        #var(sqlite_cmd)aCommand := DBCommand new &sqlite_cnn:$self &sql:aSqlText.

        #var aTable := DBTable new &sql_field_names:(aCommand sql_field_names).
        #var aReader := DBReader new &sqlite_cmd:aCommand.
        
        aReader run:aTable &eachPair:(:aTable:aRow)
            [ aTable $appendRow:aRow. ].

        aCommand free.

        ^ aTable.
    ]
        
    #method numberOfTable
    [
        theOpen ! [ #throw DBException new &message:"SQLite database is not open.". ].

        #var(sqlite_cmd)aCommand := DBCommand new &sqlite_cnn:$self &sql:"SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%';".

        #var aNumberOfTable := aCommand evalScalar.
        
        aCommand free.

        ^ aNumberOfTable.
    ]

    #method tables
    [
        theOpen ! [ #throw DBException new &message:"SQLite database is not open.". ].

        #var(sqlite_cmd)aCommand := DBCommand new &sqlite_cnn:$self &sql:"SELECT name FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%';".

        #var(array_list)nameOfTables := ArrayList new.
        
        #loop (aCommand evalNext)?
        [
            nameOfTables += aCommand getAt &int:0.
        ].
        
        aCommand free.
        
        ^ nameOfTables.
    ]

    #method close
    [
        theOpen
            ? [ system'external'sqlite3 sqlite3_close &int:theDBHandle. ].
            
        theOpen := false.
    ]
    
    #method free <= %close.    
}

// --- Commnad ---

#class(sealed) DBCommand
{
    #field(sqlite_cnn) theConnection.
    #field(handle)     theCommandHandle.
    
    #constructor new &sqlite_cnn:aConnection &sql:aText
    [
        theConnection := aConnection.
            
        theConnection is &available ! [ #throw DBException new &message:"SQLite database is not open.". ].

        $self $prepare &literal:(aText literal).
    ]
    
    #method sqlite_cmd = $self.
    
    #method handle = theCommandHandle.
    
    #method $prepare &literal:aText
    [
        #var(int) aHandle. 
        #var(int)aTextLen := aText length.
        
        #var(int) aRetVal := system'external'sqlite3 sqlite3_prepare_v2
                                                &int:(theConnection int)
                                                &literal:aText
                                                &int:aTextLen
                                                &vint:aHandle
                                                &int:0.
        (aRetVal != SQLITE_OK)? [ #throw DBException new &message:(theConnection last_error_message). ].
        
        theCommandHandle := Handle new &int:aHandle.
    ]
    
    #method eval
    [
        #var(int) aRetPrc := system'external'sqlite3 sqlite3_step &int:theCommandHandle.

        (aRetPrc != SQLITE_DONE)? [ #throw DBException new &message:"Could not execute SQL statement.". ].
    ]
    
    #method(bool) evalNext
    [
        #var(int) aRetPrc := system'external'sqlite3 sqlite3_step &int:theCommandHandle.

        ^ (aRetPrc == SQLITE_ROW).
    ]
    
    #method(stacksafe) evalScalar
    [
        #var(int) aRetPrc := system'external'sqlite3 sqlite3_step &int:theCommandHandle.

        (aRetPrc != SQLITE_ROW)? [ ^ $nil. ].

        ^ $self getAt &int:0.                
    ]
    
    #method sqlite_reader = sqlite'DBReader new &sqlite_cmd:$self sqlite_reader.
    
    #method enumerator = sqlite'DBReader new &sqlite_cmd:$self enumerator.
    
    #method(stacksafe) getAt &int:anIndex
    [
        #var(int) columnType := system'external'sqlite3 sqlite3_column_type &int:theCommandHandle &int:anIndex.
        
        (columnType == SQLITE_INTEGER)?
        [
            #var(int) val := system'external'sqlite3 sqlite3_column_int &int:theCommandHandle &int:anIndex.
            
            ^ IntNumber new &int:val.
        ].
        
        (columnType == SQLITE_TEXT)?
        [
            #var(dirty_ptr)aColumnTextPtr := system'external'sqlite3 sqlite3_column_text &int:theCommandHandle &int:anIndex.

            ^ aColumnTextPtr literal.
        ].
        
        (columnType == SQLITE_FLOAT)?
        [
            #var(real) val := system'external'sqlite3 sqlite3_column_double &int:theCommandHandle &int:anIndex.
            
            ^ RealNumber new &real:val.
        ].

        (columnType == SQLITE_BLOB)?
        [
            #var(dirty_ptr)blob := system'external'sqlite3  sqlite3_column_blob &int:theCommandHandle &int:anIndex.
            #var(int)len := system'external'sqlite3 sqlite3_column_bytes &int:theCommandHandle &int:anIndex.
            
            #var(bytearray)arr := ByteArray new &int:len.

            blob read &bytearray:arr &int:len.

            ^ arr.            
        ].

        (columnType == SQLITE_NULL)?
        [
            ^ $nil.
        ].

        #throw DBException new &message:"Unsupported type".
    ]
    
    #method(stacksafe) readFieldCount &vint:aCount
    [
        #var(int) ret := system'external'sqlite3 sqlite3_column_count &int:theCommandHandle.
        
        aCount := ret.
    ]
    
    #method(array) sql_field_names
    [
        #var(int)field_count.
        $self readFieldCount &vint:field_count.
        
        #var(array) names := Array new &int:field_count.
        #var(int) i := 0.
        #loop (i < field_count)?
        [
            #var(dirty_ptr)aColumnNamePtr := system'external'sqlite3 sqlite3_column_origin_name &int:theCommandHandle &int:i.
            
            names setAt &int:i &object:(aColumnNamePtr literal).
            
            i := i + 1.
        ].
        
        ^ names.
    ]
    
    #method free
    [
        #var(int) aRetVal := system'external'sqlite3 sqlite3_finalize &int:theCommandHandle.
        
        (aRetVal != SQLITE_OK) ? [ #throw DBException new &message:"Could not finalize SQL statement.". ].
    ]
}
