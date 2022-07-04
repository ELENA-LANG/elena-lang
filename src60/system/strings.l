// --- Array template ---

public sealed Array<T> //: Indexable<T>
{
    __string T[] theArray;
}

// --- String ---

public const struct String : BaseValue
{
    __string byte[] theArray;

    string toPrintable() = self;

    int Length
    {
        /// Reads the literal length into an output parameter
        get()
        {
           int l := $len theArray;
           l := l - 1;

           ^ l;
        }
    }

}

