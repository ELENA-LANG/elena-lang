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

