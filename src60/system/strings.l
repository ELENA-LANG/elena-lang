// --- Array template ---

public sealed Array<T> //: Indexable<T>
{
    embeddable __string T[] theArray;
}

// --- String ---

public const struct String : BaseValue
{
    __string byte[] theArray;

    string toPrintable() = self;

    constructor allocate(int size)
        = new byte[](size + 1);

//    /*internal */constructor fromByteArray(int index, int size, byte[] buffer)
//        <= allocate(size)
//    {
//        extern elenart.core_subcopyz(self, index, size, buffer);
//    }

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

