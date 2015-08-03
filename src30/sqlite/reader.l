#define system.
#define system'routines.

#class(sealed)DBReader
{
    #field(type:sqlite_cmd)theCommand.
    #field(type:handle)theCommandHandle.
    #field(type:array) theFieldNames.
    
    #constructor new &sqlite_cmd:aCommand
    [
        theCommand := aCommand.
        
        theCommandHandle := aCommand handle.
    ]
    
    #method sqlite_reader = $self.
    
    #method(stacksafe) readNext &vint:aResult
    [
        #var(type:int) aRetVal := system'external'sqlite3 sqlite3_step &handle:theCommandHandle.
        
        aResult << aRetVal.
    ]
    
    #method(type:bool)next
    [
        #var(type:int)aResult.
        $self readNext &vint:aResult.
        
        ^ aResult == SQLITE_ROW.
    ]
    
    #method getAt : aSelector
        = aSelector cast:%eval &to:
            {
                eval : aReader &int : anInt = aReader getAt &index:anInt.
                
                eval : aReader &literal : aFieldName = aReader getAt &literal:aFieldName.
                
                eval : aReader &wide : aFieldName = aReader getAt &literal:(aFieldName literal).
            } &with:$self.
    
    #method(stacksafe) getAt &int:anIndex
        = theCommand getAt &int:anIndex.
    
    #method getAt &literal:aFieldName
    [
        ($nil == theFieldNames)
            ? [ theFieldNames := theCommand sql_field_names. ].
            
        ^ $self getAt &int:(theFieldNames indexOf:aFieldName int).
    ]
    
    #method row
    [
        #var(type:int)aCount.
        theCommand readFieldCount &vint:aCount.
        
        #var(type:array) aRow := Array new &int:aCount.
        #var(type:int)i := 0.
        #loop (i < aCount)?
        [
            aRow setAt &int:i &object:($self getAt &int:i).
            
            i := i + 1.
        ].
        
        ^ aRow.
    ]
    
    #method enumerator = Enumerator
    {
        #method get = self row.
    
        #method(type:bool) next => self.
    }.
}
