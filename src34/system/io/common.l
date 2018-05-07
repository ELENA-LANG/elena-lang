// === basic interfaces ===

// --- Stream ---

public interface Stream
{
//    stream = $self.
    
    int vprop length :: readLengthTo,writeLength.
    
    int vprop index :: readIndexTo,writeIndex.
    
    abstract stacksafe writeLength(int aLength) [ ]
        
    abstract stacksafe readLengthTo(ref<int> aLength) [ ]
    
    abstract stacksafe readIndexTo(ref<int> anIndex) []
    
    abstract stacksafe writeIndex(int anIndex) []

    stacksafe appendIndex(int anIndex)
    [
        int aNewIndex := self index.
        
        aNewIndex := aNewIndex + anIndex.
        
        self writeIndex(aNewIndex).
    ]
        
    stacksafe reduceIndex(int anIndex)
    [
        int aNewIndex := self index.
        
        aNewIndex := aNewIndex - anIndex.
        
        self writeIndex(aNewIndex).
    ]
        
    abstract stacksafe read(ByteArray aDump, ref<int> aLength) []
    
    abstract stacksafe write(ByteArray aDump, int aLength) []
    
    abstract close []
}
