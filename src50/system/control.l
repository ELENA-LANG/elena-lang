// --- MatrixIndexer ---

/// An indexer prototype
public interface Indexer2D
{
    /// Returns false if the index out of range
    get abstract bool Available();

    /// Returns the current member
    get abstract Value();
    
    /// Sets the current member
    set abstract Value(object);
    
    abstract int Index1
    {
        /// Reads the current index
        get();
        
        /// Sets the index by specified value
        set(value);
    }
    
    abstract int Index2
    {
        /// Reads the current index
        get();
        
        /// Sets the index by specified value
        set(value);
    }
    
    /// Increaments the index by specified value
    abstract appendIndex1(int offs);
    
    /// Increaments the index by specified value
    abstract appendIndex2(int offs);

    abstract int Length1
    {
        get();
    }

    abstract int Length2
    {
        get();
    }
}
