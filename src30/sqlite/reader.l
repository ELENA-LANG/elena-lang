#import system.
#import system'routines.

#class(sealed)DBReader
{
    #field(sqlite_cmd) theCommand.
    #field(handle)     theCommandHandle.
    #field(array)      theFieldNames.
    
    #constructor new &sqlite_cmd:aCommand
    [
        theCommand := aCommand.
        
        theCommandHandle := aCommand handle.
    ]
    
    #method sqlite_reader = $self.
    
    #method(stacksafe) readNext &vint:aResult
    [
        #var(int) aRetVal := system'external'sqlite3 sqlite3_step &int:theCommandHandle.
        
        aResult := aRetVal.
    ]
    
    #method(bool)next
    [
        #var(int)aResult.
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
        #var(int)aCount.
        theCommand readFieldCount &vint:aCount.
        
        #var(array) aRow := Array new &int:aCount.
        #var(int)i := 0.
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
    
        #method next => self.
    }.
}
