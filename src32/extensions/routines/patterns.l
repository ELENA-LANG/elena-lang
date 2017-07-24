// --- MatrixSpace ---

limited class MatrixSpace
{
    int rows = $nil.

    int columns = $nil.
        
    stacksafe readLength1 &vint:retVal 
    [
        retVal := $self rows.
    ]
    
    stacksafe readLength2 &vint:retVal
    [
        retVal := $self columns.
    ]
    
    stacksafe getAt &int:i &int:j []
    
    stacksafe setAt &int:i &int:j &object:o []
    
    indexer2d = BaseMatrixIndexer
    {
        implicit object
        [
            theIndex1 := 0.
            theIndex2 := 0.            

            int len1 := 0.
            int len2 := 0.
                                    
            $owner readLength1 &vint:len1.
            $owner readLength2 &vint:len2.
            
            theLength1 := len1.
            theLength2 := len2.
        ]
        
        get
            = $owner getAt &int:theIndex1 &int:theIndex2.
            
        set : o
        [
            $owner setAt &int:theIndex1 &int:theIndex2 &object:o.
        ]            
    }.
}