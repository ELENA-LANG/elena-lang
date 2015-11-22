// ==== ver 3.4.1 ===

#import system.
#import system'routines.
#import system'collections.
#import extensions.

#symbol(const,type:int)SQLITE_OK = 0.
#symbol(const,type:int)SQLITE_ROW = 100.
#symbol(const,type:int)SQLITE_DONE = 101.
#symbol(const,type:int)SQLITE_INTEGER = 1.
#symbol(const,type:int)SQLITE_FLOAT = 2.
#symbol(const,type:int)SQLITE_TEXT = 3.
#symbol(const,type:int)SQLITE_BLOB = 4.
#symbol(const,type:int)SQLITE_NULL = 5.

#subject(class:DBConnection)sqlite_cnn.
#subject(class:DBCommand)sqlite_cmd.
#subject(class:DBReader)sqlite_reader.
#subject(class:DBTable)sqlite_table.
#subject(class:DBRow)sqlite_row.

// --- DBException ---

#class DBException :: Exception
{
    #constructor new &message:aMessage
        <= %new &literal:(aMessage literal) &subject: %DBError.
}

// --- Connection ---

#class(sealed) DBConnection
{
    #field(type:int) theDBHandle.
    #field(type:literal)thePath.
    #field(type:bool)theOpen.
    
    #constructor new &path:aPath
    [
        thePath := aPath literal.
        theOpen := false.
    ]
    
    #method sqlite_cnn = $self.
    
    #method int = theDBHandle.
    
    #method is &available = theOpen.
    
    #method open
    [
        #var(type:int)aHandle.
        #var(type:int) aRetVal := system'external'sqlite3 sqlite3_open &literal:thePath &vint:aHandle.
        (aRetVal != SQLITE_OK)? [ #throw DBException new &message:"Could not open a database file". ] ! [ theOpen := true. ].

        theDBHandle := aHandle.
    ]
    
    #method last_error_message
    [
        #var(type:dirty_ptr)aPtr := system'external'sqlite3 sqlite3_errmsg &int:theDBHandle.

        ^ aPtr literal.
    ]

    #method do &sql:aQuery
        <= %executeNonQuery:aQuery.

    #method executeNonQuery : aSqlText
    [
        theOpen ! [ #throw DBException new &message:"SQLite database is not open.". ].

        #var(type:sqlite_cmd)aCommand := DBCommand new &sqlite_cnn:$self &sql:aSqlText.
        
        aCommand eval.
        
        aCommand free.
    ]

    #method executeQuery : aSqlText
    [
        theOpen ! [ #throw DBException new &message:"SQLite database is not open.". ].

        #var(type:sqlite_cmd)aCommand := DBCommand new &sqlite_cnn:$self &sql:aSqlText.

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

        #var(type:sqlite_cmd)aCommand := DBCommand new &sqlite_cnn:$self &sql:"SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%';".

        #var aNumberOfTable := aCommand evalScalar.
        
        aCommand free.

        ^ aNumberOfTable.
    ]

    #method tables
    [
        theOpen ! [ #throw DBException new &message:"SQLite database is not open.". ].

        #var(type:sqlite_cmd)aCommand := DBCommand new &sqlite_cnn:$self &sql:"SELECT name FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%';".

        #var(type:array_list)nameOfTables := ArrayList new.
        
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
    #field(type:sqlite_cnn)theConnection.
    #field(type:handle)theCommandHandle.
    
    #constructor new &sqlite_cnn:aConnection &sql:aText
    [
        theConnection := aConnection.
            
        $self $prepare &literal:(aText literal).
    ]
    
    #method sqlite_cmd = $self.
    
    #method handle = theCommandHandle.
    
    #method $prepare &literal:aText
    [
        #var(type:int) aHandle. 
        #var(type:int)aTextLen.
        aText readLength &vint:aTextLen.
        
        #var(type:int) aRetVal := system'external'sqlite3 sqlite3_prepare_v2
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
        #var(type:int) aRetPrc := system'external'sqlite3 sqlite3_step &int:theCommandHandle.

        (aRetPrc != SQLITE_DONE)? [ #throw DBException new &message:"Could not execute SQL statement.". ].
    ]
    
    #method evalNext
    [
        #var(type:int) aRetPrc := system'external'sqlite3 sqlite3_step &int:theCommandHandle.

        ^ (aRetPrc == SQLITE_ROW).
    ]
    
    #method(stacksafe) evalScalar
    [
        #var(type:int) aRetPrc := system'external'sqlite3 sqlite3_step &int:theCommandHandle.

        (aRetPrc != SQLITE_ROW)? [ ^ $nil. ].

        ^ $self getAt &int:0.                
    ]
    
    #method sqlite_reader = sqlite'DBReader new &sqlite_cmd:$self sqlite_reader.
    
    #method enumerator = sqlite'DBReader new &sqlite_cmd:$self enumerator.
    
    #method(stacksafe) getAt &int:anIndex
    [
        #var(type:int) columnType := system'external'sqlite3 sqlite3_column_type &int:theCommandHandle &int:anIndex.
        
        (columnType == SQLITE_INTEGER)?
        [
            #var(type:int) val := system'external'sqlite3 sqlite3_column_int &int:theCommandHandle &int:anIndex.
            
            ^ IntNumber new &int:val.
        ].
        
        (columnType == SQLITE_TEXT)?
        [
            #var(type:dirty_ptr)aColumnTextPtr := system'external'sqlite3 sqlite3_column_text &int:theCommandHandle &int:anIndex.

            ^ aColumnTextPtr literal.
        ].
        
        (columnType == SQLITE_FLOAT)?
        [
            #var(type:real) val := system'external'sqlite3 sqlite3_column_double &int:theCommandHandle &int:anIndex.
            
            ^ RealNumber new &real:val.
        ].

        (columnType == SQLITE_BLOB)?
        [
            #var(type:int)blob := system'external'sqlite3  sqlite3_column_blob &int:theCommandHandle &int:anIndex.
            #var(type:int)len := system'external'sqlite3 sqlite3_column_bytes &int:theCommandHandle &int:anIndex.
            
            #var(type:bytearray)arr := ByteArray new &int:len.

            #var(type:dirty_ptr)ptr.
            ptr write &int:blob.

            ptr read &bytearray:arr &int:len.

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
        #var(type:int) ret := system'external'sqlite3 sqlite3_column_count &int:theCommandHandle.
        
        aCount << ret.
    ]
    
    #method(type:array) sql_field_names
    [
        #var(type:int)field_count.
        $self readFieldCount &vint:field_count.
        
        #var(type:array) names := Array new &int:field_count.
        #var(type:int) i := 0.
        #loop (i < field_count)?
        [
            #var(type:dirty_ptr)aColumnNamePtr := system'external'sqlite3 sqlite3_column_origin_name &int:theCommandHandle &int:i.
            
            names setAt &int:i &object:(aColumnNamePtr literal).
            
            i := i + 1.
        ].
        
        ^ names.
    ]
    
    #method free
    [
        #var(type:int) aRetVal := system'external'sqlite3 sqlite3_finalize &int:theCommandHandle.
        
        (aRetVal != SQLITE_OK) ? [ #throw DBException new &message:"Could not finalize SQL statement.". ].
    ]
}
