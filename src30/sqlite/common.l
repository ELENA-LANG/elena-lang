#define system.
#define extensions.

#symbol(const,type:int)SQLITE_OK = 0.
#symbol(const,type:int)SQLITE_ROW = 100.
#symbol(const,type:int)SQLITE_DONE = 101.
#symbol(const,type:int)SQLITE_INTEGER = 1.
#symbol(const,type:int)SQLITE_FLOAT = 2.
#symbol(const,type:int)SQLITE_TEXT = 3.
//#symbol(const,type:int)SQLITE_BLOB = 4.
#symbol(const,type:int)SQLITE_NULL = 5.

#subject(class:DBConnection)sqlite_cnn.
#subject(class:DBCommand)sqlite_cmd.
#subject(class:DBReader)sqlite_reader.
#subject(class:DBTable)sqlite_table.

// --- DBException ---

#class DBException :: Exception
{
    #constructor new &message:aMessage
        <= %new &literal:(aMessage literal) &subject: %DBError.
}

// --- Connection ---

#class(sealed) DBConnection
{
    #field(type:handle) theDBHandle.
    #field(type:literal)thePath.
    #field(type:bool)theOpen.
    
    #constructor new &path:aPath
    [
        thePath := aPath literal.
        theOpen := false.
    ]
    
    #method sqlite_cnn = $self.
    
    #method handle = theDBHandle.
    
    #method is &available = theOpen.
    
    #method open
    [
        #var(type:int)aHandle.
        #var(type:int) aRetVal := system'external'sqlite3 sqlite3_open &literal:thePath &vint:aHandle.
        (aRetVal != SQLITE_OK)? [ #throw DBException new &message:"Could not open a database file". ] ! [ theOpen := true. ].

        theDBHandle := Handle new &int:aHandle.
    ]
    
    #method last_error_message
    [
        #var(type:dirty_ptr)aPtr := system'external'sqlite3 sqlite3_errmsg &handle:theDBHandle.

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
        
        aTable run &eachPair:(:aTable:aRow)
            [ aTable append &sqlite_row:aRow. ].

        aCommand free.

        ^ aTable.
    ]
        
    #method close
    [
        theOpen
            ? [ system'external'sqlite3 sqlite3_close &handle:theDBHandle. ].
            
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
                                                &handle:(theConnection handle)
                                                &literal:aText
                                                &int:aTextLen
                                                &vint:aHandle
                                                &int:0.
        (aRetVal != SQLITE_OK)? [ #throw DBException new &message:(theConnection last_error_message). ].
        
        theCommandHandle := Handle new &int:aHandle.
    ]
    
    #method eval
    [
        #var(type:int) aRetPrc := system'external'sqlite3 sqlite3_step &handle:theCommandHandle.

        (aRetPrc != SQLITE_DONE)? [ #throw DBException new &message:"Could not execute SQL statement.". ].
    ]
    
    #method sqlite_reader = sqlite'DBReader new &sqlite_cmd:$self sqlite_reader.
    
    #method enumerator = sqlite'DBReader new &sqlite_cmd:$self enumerator.
    
    #method(stacksafe) readFieldCount &vint:aCount
    [
        #var(type:int) ret := system'external'sqlite3 sqlite3_column_count &handle:theCommandHandle.
        
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
            #var(type:dirty_ptr)aColumnNamePtr := system'external'sqlite3 sqlite3_column_origin_name &handle:theCommandHandle &int:i.
            
            names setAt &int:i &object:(aColumnNamePtr literal).
            
            i := i + 1.
        ].
        
        ^ names.
    ]
    
    #method free
    [
        #var(type:int) aRetVal := system'external'sqlite3 sqlite3_finalize &handle:theCommandHandle.
        
        (aRetVal != SQLITE_OK) ? [ #throw DBException new &message:"Could not finalize SQL statement.". ].
    ]
}

//#class SQLite
//{   
//    #field(type:bool) isDbOpen.
//    #field dataTable.
//    
//    #method step : handle
//    [
//        #var(type:int) ret := system'external'sqlite3 sqlite3_step &int:handle.
//        ^ret.
//    ]
//    
//    #method executeQuery : query
//    [
//    ]
//    
//    #method getNameOfAllTable
//    [
//        (isDbOpen != true)? [ #throw "SQLite database is not open.". ].
//        
//        #var query := "SELECT name FROM sqlite_master WHERE type='table';".
//        #var(type:int) smtHandle := $self prepare:query.
//        
//        #var nameOfTables := List new.
//        
//        #var tempVar := false. 
//        #var(type:literal) aColumnText := "".
//        #loop (($self step:smtHandle) == SQLITE_ROW) ?
//        [
//            (tempVar)? [ nameOfTables append:aColumnText. ]. // use this technic just to not get the last table...
//            
//            #var(type:int)aColumnTextPtr := system'external'sqlite3 sqlite3_column_text  &int:smtHandle &int:0.
//            aColumnText :=  getStringFromPointer:aColumnTextPtr.    
//            tempVar := true.
//        ].
//        $self finalize:smtHandle.
//        
//        // this not work :(
//        //#var tblLength := (nameOfTables length) - 1.
//        //nameOfTables remove &index:tblLength.
//        
//        ^nameOfTables.
//    ]
//    
//    #method getNumberOfTable
//    [
//        (isDbOpen != true)? [ #throw "SQLite database is not open.". ].
//        
//        #var query := "SELECT COUNT(*) FROM sqlite_master WHERE type='table';".
//        #var(type:int) smtHandle := $self prepare:query.
//        ($self step:smtHandle != SQLITE_ROW)? [^0.].
//        #var(type:int) val := system'external'sqlite3 sqlite3_column_int &int:smtHandle &int:0.
//        $self finalize:smtHandle.
//        ^(convertor toInt:(val-1)). // remove the SQLITe default table...
//    ]
//}