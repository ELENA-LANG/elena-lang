#define system.
#define system'routines.
#define system'collections.

#class(sealed) DBTable
{
    #field(type:array)theFieldNames.
    #field(type:array_list) theRows.
    
    #constructor new &sql_field_names:aNames
    [
        theFieldNames := aNames array.
        theRows := ArrayList new.
    ]

    #method append &sqlite_row:aRow
    [
        theRows append:aRow.
    ]

    #method getAt : aSelector
        = aSelector cast:%eval &to:
            {
                eval : aTable &int : anInt = aTable getAt &int:anInt.
                
                eval : aTable &literal : aFieldName = aTable getAt &literal:aFieldName.
                
                eval : aTable &wide : aFieldName = aTable getAt &literal:(aFieldName literal).
            } &with:$self.

    #method(stacksafe) getAt &int:anIndex
        = theRows getAt &int:anIndex.

    #method getAt &literal:aFieldName
    [
        #var(type:int) anIndex := theFieldNames indexOf:aFieldName int.
        
        (-1 == anIndex)
            ? [ #throw DBException new &message:"Field name is unknown". ].
            
        ^ $self getAt &int:anIndex.
    ]

    #method sqlite_table = $self.

    #method => theRows.
}