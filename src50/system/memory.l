
// --- String ---

// === Convertors ===

// --- wideConvertor ---

[info:"A wide literal convertor"]
public singleton wideConvertor
{
    wide convert(BaseVariable v)
        <= convert(*v);
    
    wide convert(char ch)
    {
        short buffer[2];
        int   length := 0;
        
        intern system'core_routines'__wideconvertor.convert(ch, buffer, ref length);
        
        ^ WideString.fromShortArray(0, length, buffer)
    }    
    
    /// Converts the parameter to the literal value
    wide convert(int n)
    {
        short buffer[12];
        extern elenart.core_inttowstr(n, 10, buffer, ref int length);
        
        ^ WideString.fromShortArray(0, length, buffer)
    }

    /// Converts the parameter to the literal value
    wide convert(uint n)
    {
        short buffer[12];
        extern elenart.core_uinttowstr(n, 10, buffer, ref int length);
        
        ^ WideString.fromShortArray(0, length, buffer)
    }

    /// Converts the parameter to the literal value
    wide convert(short n)
    {
        short buffer[12];
        extern elenart.core_inttostr(n, 10, buffer, ref int length);
        
        ^ WideString.fromShortArray(0, length, buffer)
    }

    /// Converts the parameter to the literal value
    wide convert(byte n)
    {
        short buffer[12];
        extern elenart.core_inttowstr(n, 10, buffer, ref int length);
        
        ^ WideString.fromShortArray(0, length, buffer)
    }

    /// Converts the parameter to the literal value
    wide convert(long n)
    {
        short buffer[64];
        int   length := 0;

        intern system'core_routines'__wideconvertor.convert(n, 10, buffer, ref length);
        
        ^ WideString.fromShortArray(0, length, buffer)
    }

    /// Converts the parameter to the literal value with the provided radix
    wide convert(int n, int radix)
    {
        short buffer[32];
        extern elenart.core_inttowstr(n, radix, buffer, ref int length);
        
        ^ WideString.fromShortArray(0, length, buffer)
    }

    /// Converts the parameter to the literal value with the provided radix
    wide convert(uint n, int radix)
    {
        short buffer[32];
        extern elenart.core_uinttowstr(n, radix, buffer, ref int length);
        
        ^ WideString.fromShortArray(0, length, buffer)
    }

    /// Converts the parameter to the literal value with the provided radix
    wide convert(short n, int radix)
    {
        short buffer[16];
        extern elenart.core_uinttowstr(n, radix, buffer, ref int length);
        
        ^ WideString.fromShortArray(0, length, buffer)
    }

    /// Converts the parameter to the literal value with the provided radix
    wide convert(byte n, int radix)
    {
        short buffer[12];
        extern elenart.core_uinttowstr(n, radix, buffer, ref int length);
        
        ^ WideString.fromShortArray(0, length, buffer)
    }

    /// Converts the parameter to the literal value with the provided radix
    wide convert(long n, int radix)
    {
        short buffer[64];
        int   length := 0;
        
        intern system'core_routines'__wideconvertor.convert(n, radix, buffer, ref length);
        
        ^ WideString.fromShortArray(0, length, buffer)
    }

    /// Converts the parameter to the literal value
    wide convert(real n)
    {
        short buffer[32];
        int   length := 0;
        
        intern system'core_routines'__wideconvertor.convert(n, 24, buffer, ref length);
        
        ^ WideString.fromShortArray(0, length, buffer)
    }

    /// Converts UTF-8 literal to UTF-16e
    wide convert(string s)
        = s;
        
    /// Returns a parameter
    wide convert(wide s)
        = s;

    /// Returns the literal presentation of the parameter
    wide convert(o)
        = cast wide(o.toPrintable());
}

/// An Unicode character convertor
public singleton charConvertor
{
    char convert(BaseVariable v)
        <= convert(v.Value);
        
    internal save(char ch, int index, short[] charArray, ref int length)
    {
        intern system'core_routines'__charconvertor.save(ch,index,charArray,ref length);
    }
    
    internal save(char ch, int index, byte[] charArray, ref int length)
    {
        intern system'core_routines'__charconvertor.save(ch,index,charArray,ref length);
    }
    
    internal char convert(int index, string s)
    {
        intern system'core_routines'__charconvertor.convert(index, s, ref char retVal);
        
        ^ retVal
    }
        
    internal char convert(int index, byte[] s)
    {
        intern system'core_routines'__charconvertor.convert(index, s, ref char retVal);
        
        ^ retVal
    }
  
    internal char convert(int index, wide s)
    {
        intern system'core_routines'__charconvertor.convert(index, s, ref char retVal);
        
        ^ retVal
    }

    internal convert(int index, short[] buffer, ref char retVal)
    {
        intern system'core_routines'__charconvertor.convert(index, buffer, ref retVal);
    }    
        
    internal char convert(int index, short[] buffer)
    {
        intern system'core_routines'__charconvertor.convert(index, buffer, ref char retVal);
        
        ^ retVal
    }    
        
    /// Returns a parameter        
    char convert(char ch)
        = new CharValue(ch);
  
    /// Converts a literal string into a character
    char convert(string s)
    {
        intern system'core_routines'__charconvertor.convert(0, s, ref char retVal);
        
        int l1 := retVal.Length;
        int l2 := s.Length;
        
        (l1 != l2)
            ? { InvalidArgumentException.new().raise() };
        
        ^ retVal
    }        
        
    /// Converts a wide literal string into a character                
    char convert(wide s)
    {
        intern system'core_routines'__charconvertor.convert(0, s, ref char retVal);

        int l1 := retVal.Length;
        int l2 := s.Length;
                
        (l1 != l2)
            ? { InvalidArgumentException.new().raise() };
        
        ^ retVal
    }      
        
    /// Converts a byte value into a character
    char convert(byte n)
    {
        intern system'core_routines'__charconvertor.convert(n, ref char retVal);
        
        ^ retVal
    }        
        
    /// Converts a short integer into a character
    char convert(short n)
    {
        intern system'core_routines'__charconvertor.convert(n, ref char retVal);
        
        ^ retVal
    }
        
    /// Converts an integer into a character
    char convert(int n)
    {
        intern system'core_routines'__charconvertor.convert(n, ref char retVal);
        
        ^ retVal
    }
        
    char convert(o)
        = cast char(o);
}

// === Enumerators / Indexers

// --- StringEnumerator ---

/// A string enumerator
class StringEnumerator : Enumerator<char>
{
    string   theLiteral;
    ref<int> theIndex;
    ref<int> theDiff;
    char     theChar;
    
    /// Creates an enumerator
    constructor(string s)
    {
        theLiteral := s;
        theIndex := -1;
        theDiff := 1;
        theChar := $0
    }

    /// Resets the enumerator
    reset()
    {
        theIndex.Value := -1;
        theDiff.Value := 1
    }

    /// Goes to the next enumeration member and returns true if it is not the last member
    bool next()
    {
        int aLength := theLiteral.Length;
        
        theIndex.append(cast int(theDiff));
        
        if (theIndex < aLength)
            {
                theChar := theLiteral.at(cast int(theIndex));
                
                theDiff.Value := theChar.Length;
                
                ^ true
            };

        theDiff.Value := 0;
        theChar := $0;

        ^ false
    }
    
    Enumerator cast() = new Enumerator
    {
        embeddable dispatch() => self;
    };
    
    enumerable() = theLiteral;
    
    /// Returns the current character
    char Value = theChar;
}

// --- LiteralIndexer ---

/// A literal value indexer
class StringIndexer : Indexer<char>
{
    string   theLiteral;
    ref<int> theIndex;
    
    /// Creates an indexer
    constructor(string s)
    {
        theLiteral := s;
        theIndex := 0
    }
    
    /// Returns false if the index out of range
    get bool Available()
    {
        int len := theLiteral.Length;
        int val := theIndex.Value;
        
        ^ val < len
    }

    /// Returns the current character
    get char Value()
    {
        char ch := $0;
        theLiteral.read(cast int(theIndex), ref ch);
        
        ^ ch
    }
    
    /// Sets the index by specified value
    set Value(char ch)
    {
        system'NotSupportedException.raise()
    }
    
    int Index
    {
        /// Reads the current index and saves it into an output parameter
        get()
        {
            int retVal := theIndex.Value;
            
            ^ retVal
        }
        
        /// Sets the index by specified value
        set(val)
        {
            theIndex.Value := val
        }
    }
    
    appendIndex(int val)
    {
        theIndex.append(val)
    }
    
    int Length
    {
        /// Reads the length to the output parameter    
        get()
        {
            int retVal := theLiteral.Length;
            
            ^ retVal
        }
    }
        
    Indexer cast() = new Indexer{ embeddable dispatch() => self; };
}

// --- WideLiteralEnumerator ---

/// A literal enumerator
class WideStringEnumerator : Enumerator<char>
{
    wide      theLiteral;
    ref<int>  theIndex;
    ref<int>  theDiff;
    ref<char> theChar;
    
    /// Creates an enumerator
    constructor(wide s)
    {
        theLiteral := s;
        theIndex := -1;
        theDiff := 1;
        theChar := $0
    }

    /// Resets the enumerator
    reset()
    {
        theIndex.Value := -1;
        theDiff.Value := 1
    }

    /// Goes to the next enumeration member and returns true if it is not the last member
    bool next()
    {
        int len := theLiteral.Length;

        int diff := theDiff.Value;
        theIndex.append(diff);
        
        int index := theIndex.Value;
        if (index < len)
        {
            theChar := theLiteral.at(index);                
            //theDiff := theChar.WideLength;
            diff := theChar.WideLength;
            theDiff.Value := diff;
            
            ^ true
        };

        theDiff.Value := 0;
        theChar := $0;

        ^ false
    }
    
    Enumerator cast() = new Enumerator
    {
        embeddable dispatch() => self;
    };
    
    enumerable() = theLiteral;
    
    /// Returns the current character
    char Value = theChar;
}

// --- WideLiteralInexer ---

/// A wide literal enumerator
class WideStringIndexer : Indexer<char>
{
    wide     theLiteral;
    ref<int> theIndex;
    
    /// Creates an indexer
    constructor(wide s)
    {
        theLiteral := s;
        theIndex := 0
    }

    /// Returns false if the index out of range    
    get bool Available()
    {
        int len := theLiteral.Length;
        int val := theIndex.Value;
        
        ^ val < len
    }

    /// Returns the current character
    get char Value()
        = theLiteral.at(cast int(theIndex));
    
    set Value(char ch)
    {
        system'NotSupportedException.raise()
    }
    
    /// Reads the current index and saves it into an output parameter
    int Index
    {
        get()
        {
            int retVal := theIndex.Value;
            
            ^ retVal
        }
        
        /// Sets the index by specified value
        set(val)
        {
            theIndex.Value := val
        }
    }
    
    appendIndex(int val)
    {
        theIndex.append(val)
    }
    
    int Length
    {
        /// Reads the length to the output parameter    
        get()
        {
            int retVal := theLiteral.Length;
            
            ^ retVal
        }
    }
        
    Indexer cast() = new Indexer{ embeddable dispatch() => self; };
}

// --- ArrayIndexer ---

/// An array indexer
public class ArrayIndexer : Indexer
{
    Array    _array;
    ref<int> _index;

    /// Returns the parameter indexer
    constructor(Array arr)
    {
        _array := arr;
        _index := 0
    }
    
    /// Returns false if the index out of range
    get bool Available()
    {
        int len := _array.Length;

        ^ _index < len
    }

    /// Returns the current member
    get Value()
        = _array.at(*_index);
    
    /// Sets the current member
    set Value(object)
    {
        int n := *_index;
        
        _array.setAt(n, object)
    }
    
    /// Reads the current index and saves it into an output parameter
    int Index
    {
        get()
        {
            int retVal := _index.Value;
            
            ^ retVal
        }
        
        /// Sets the index by specified value
        set(n)
        {
           _index.Value := n
        }
    }
    
    /// Sets the index by specified value
    appendIndex(int offs)
    {
        _index.append(offs)
    }

    int Length
    {
        /// Reads the length to the output parameter    
        get()
        {
            int retVal := _array.Length;
            
            ^ retVal
        }
    }
}

// --- ArrayEnumerator ---

/// An array enumerator
class ArrayEnumerator : Enumerator
{
    Array    theArray;
    ref<int> theIndex;
    
    /// Creates an array enumerator
    constructor(Array array)
    {
        theArray := array;
        theIndex := -1
    }

    /// Resets the enumerator
    reset()
    {
        theIndex.Value := -1
    }

    /// Goes to the next enumeration member and returns true if it is not the last member
    bool next()
    {
        int length := theArray.Length;
        
        theIndex.append(1);

        ^ cast int(theIndex) < length
    }
    
    /// Returns the current object
    get Value()
        = theArray.at(cast int(theIndex));
        
    enumerable() = theArray;
}

// --- BitArrayIndexer ---

/// An array indexer
class BitArrayIndexer : Indexer<bool>
{
    BitArray theArray;
    ref<int> theIndex;

    /// Returns the parameter indexer
    constructor(BitArray anArray)
    {
        theArray := anArray;
        theIndex := 0
    }
    
    /// Returns false if the index out of range
    get bool Available()
    {
        int length := theArray.Length;
        int index := theIndex.Value;
        
        ^ index < length
    }

    /// Returns the current member
    get bool Value()
    {
        int index := theIndex.Value;
        
        ^ theArray.at(index)
    }
    
    /// Sets the current member
    set Value(bool val)
    {
        int index := theIndex.Value;
        
        theArray.setAt(theIndex, val)
    }
    
    int Index
    {
        /// Reads the current index and saves it into an output parameter
        get()
        {
            int retVal := theIndex.Value;
            
            ^ retVal
        }
        
        /// Sets the index by specified value
        set(val)
        {
            theIndex.Value := val
        }
    }
    
    appendIndex(int val)
    {
        theIndex.append(val)
    }
    
    int Length
    {
        /// Reads the length to the output parameter    
        get()
        {
            int retVal := theArray.Length;
            
            ^ retVal
        }
    }
        
    Indexer cast() = new Indexer { embeddable dispatch() => self; };        
}

// --- BitArrayEnumerator ---

/// An array enumerator
class BitArrayEnumerator : Enumerator<bool>
{
    BitArray theArray;
    ref<int> theIndex;
    
    /// Creates an array enumerator
    constructor(BitArray a)
    {
        theArray := a;
        theIndex := -1
    }

    /// Resets the enumerator
    reset()
    {
        theIndex.Value := -1
    }

    /// Goes to the next enumeration member and returns true if it is not the last member
    bool next()
    {
        int length := theArray.Length;
        
        theIndex.append(1);
        
        int index := theIndex.Value;

        ^ index < length
    }
    
    /// Returns the current object
    get bool Value()
    {
        int index := theIndex.Value;
        
        ^ theArray.at(index)
    }
        
    enumerable() = theArray;
    
    Enumerator cast() = new Enumerator { embeddable dispatch() => self; };
}

// --- BitArray32Indexer ---

/// An array indexer
class BitArray32Indexer : Indexer<bool>
{
    BitArray32 theArray;
    ref<int>   theIndex;

    /// Returns the parameter indexer
    constructor(BitArray32 bs)
    {
        theArray := bs;
        theIndex := 0
    }
    
    /// Returns false if the index out of range
    get bool Available()
    {
        int index := theIndex;
        
        ^ index < 32
    }

    /// Returns the current member
    get bool Value()
    {
        int index := theIndex;
        
        ^ theArray.at(index)
    }
    
    /// Sets the current member
    set Value(bool val)
    {
        int index := theIndex;
        
        theArray.setAt(index, val)
    }

    int Index
    {    
        /// Reads the current index and saves it into an output parameter
        get()
        {
            int retVal := theIndex.Value;
            
            ^ retVal
        }
        
        /// Sets the index by specified value
        set(val)
        {
            theIndex.Value := val
        }
    }
    
    appendIndex(int val)
    {
        theIndex.append(val)
    }
    
    int Length
    {
        /// Reads the length to the output parameter    
        get()
            => theArray;
    }
        
    Indexer cast() = new Indexer { embeddable dispatch() => self; };        
}

// --- BitArray32Enumerator ---

// An array enumerator
class BitArray32Enumerator : Enumerator<bool>
{
    BitArray32 theArray;
    ref<int>   theIndex;
    
    /// Creates an array enumerator
    constructor(BitArray32 bs)
    {
        theArray := bs;
        theIndex := -1
    }

    /// Resets the enumerator
    reset()
    {
        theIndex.Value := -1
    }

    /// Goes to the next enumeration member and returns true if it is not the last member
    bool next()
    {
        theIndex.append(1);

        int index := theIndex;

        ^ index < 32
    }
    
    /// Returns the current object
    get bool Value()
    {
        int index := theIndex;
        
        ^ theArray.at(index)
    }
        
    enumerable() = theArray;        
    
    Enumerator cast() = new Enumerator { embeddable dispatch() => self; };
}

// template test cases
T1 { static Default = nil; }
T2;
T3;
T4;
T5;
T6;
T7;
T8;
T9;
T10;
T11;
T12;

template_test()
{
    ref<T1> o1;
    Enumerator<T1> o2;
    Enumerable<T1> o3;
    Indexer<T1> o4;
    Indexable<T1> o5;
    BaseEnumerator<T1> o6;
    BaseIndexer<T1> o7;
    Array<T1> o8;
    VariadicArray<T1> o9;
    Matrix<T1> o10;
    Func<T1,T2> f;
    Func<T1,T2,T3> f2;
    Func<T1,T2,T3,T4> f3;
    Func<T1,T2,T3,T4,T5> f4;
    Func<T1,T2,T3,T4,T5,T6> f5;
    Func<T1,T2,T3,T4,T5,T6,T7> f6;
    Func<T1,T2,T3,T4,T5,T6,T7,T8> f7;
    Func<T1,T2,T3,T4,T5,T6,T7,T8,T9> f8;
    Func<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10> f9;
    Func<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11> f10;
    Func<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12> f11;
    
    collections'List<T1> o11;
    collections'Stack<T1> o12;
    collections'Queue<T1> o13;
    collections'Tuple<T1> o14;
    collections'Tuple<T1,T2> o15;
    collections'Tuple<T1,T2,T3> o16;
    collections'Tuple<T1,T2,T3,T4> o17;
    collections'Tuple<T1,T2,T3,T4,T5> o18;
    collections'Tuple<T1,T2,T3,T4,T5,T6> o19;    
    collections'Map<T1,T2> o20;
}
