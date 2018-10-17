// --- MatrixSpace ---

public interface MatrixSpace
{
    abstract int rows []

    abstract int columns []
        
    readLength1To(ref<int> retVal)
    [
        retVal value := self rows.
    ]
    
    readLength2To(ref<int> retVal)
    [
        retVal value := self columns.
    ]
    
    abstract getAt(int i, int j) []
    
    abstract setAt(int i, int j, object o) []
    
    cast T<Indexer2D> = BaseMatrixIndexer::
    {
        constructor
        [
            theIndex1 := 0.
            theIndex2 := 0.            

            int len1 := 0.
            int len2 := 0.
                                    
            self readLength1To(&len1).
            self readLength2To(&len2).
            
            theLength1 := len1.
            theLength2 := len2.
        ]
        
        get
            = self getAt(theIndex1, theIndex2).
            
        set : o
        [
            self setAt(theIndex1, theIndex2, o).
        ]            
    }.
}