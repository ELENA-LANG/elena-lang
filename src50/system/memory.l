
// --- Array template ---

public sealed Array<T> : Indexable<T>
{
    embeddable __string T[] theArray;

    constructor allocate(int length)
        = new T[](length);
        
    constructor copy(T[] array, int index, int length)
        <= allocate(length)
    {
        for(int i := 0 , i < length, i += 1)
        {
            theArray[i] := array[i + index]
        }
    }

    Enumerator<T> enumerator() = new BaseEnumerator<T>
    {
        this theIndex := -1;
        
        this theLength := self.Length;
        
        get T Value() = self.at(this theIndex.Value);
        
        enumerable() = self;
        
        reset()
        {
            this theIndex := -1
        }
    };

    Enumerable cast() = new Enumerable{ Enumerator enumerator() => self; };

    Indexer<T> indexer() = new BaseIndexer<T>
    {
        this theIndex := 0;
        
        this theLength := self.Length;
        
        get T Value()
            = self.at(theIndex);
            
        set Value(T o)
        {
            self.setAt(theIndex, o)
        }
    };

    int Length
    {
        get()
        {
            int len := 0;
            len $sizeof theArray;
            
            ^ len
        }
    }

    T at(int n)
    {
        if (n < 0)
            { system'InvalidArgumentException.raise() };
        
        int len := 0;
        len $sizeof theArray;

        if (n < len)
        { 
            ^ theArray[n] 
        }
        else
        {
            system'InvalidArgumentException.raise();
            
            ^ nil // !! never reached
        }
    }

    setAt(int n, T item)
    {
        if (n < 0)
            { system'InvalidArgumentException.raise() };
        
        int len := 0;
        len $sizeof theArray;

        if (n < len)
        {
            theArray[n] := item
        }
        else
        {
            system'InvalidArgumentException.raise()
        }
    }

//    setAt(index, item)
//        <= setAt(cast int(index), cast T(item));

    /// Concatinates the arrays    
    T[] add(T[] a)
        <= add(a, a.Length);
                              
    T[] add(T[] a, int length)
    {
        int length1 := self.Length;
        int length2 := length;
        
        auto retVal := new T[](length1 + length2);

        for (int i:=0, i < length1, i += 1) {
            retVal[i] := theArray[i]
        };

        for (int i:=0, i < length2, i += 1) {
            retVal[length1 + i] := a[i]
        };
            
        ^ retVal
    }
                              
    T[] clone()
    {
        int len := self.Length;
        T[] copy := new T[](len);
        for(int i := 0, i < len, i+=1) {
            copy[i] := theArray[i]
        };
        
        ^ copy
    }
}

// --- Array template ---

public sealed nonstructural VariadicArray<T> : Indexable<T>
{
    embeddable __string T[] theArray;

    readLengthTo(ref int retVal)
        => intern system'core_routines'__args;

    int Length
    {
        get()
        {
            self.readLengthTo(ref int retVal);
            
            ^ retVal
        }            
    }
    
    T[] cast()
    {
        int len := self.Length;
        
        T[] array := new T[](len);
        for(int i := 0, i < len, i += 1)
        {
            array[i] := theArray[i]
        };
        
        ^ array
    }
    
    Enumerator<T> enumerator() = new BaseEnumerator<T>
    {
        this theIndex := -1;
        
        this theLength := self.Length;
        
        get T Value() = self.at(this theIndex.Value);
        
        enumerable() = self;
        
        reset()
        {
            this theIndex := -1
        }
    };

    Indexer<T> indexer() = new BaseIndexer<T>
    {
        this theIndex := 0;
        
        this theLength := self.Length;
        
        get T Value()
            = self.at(theIndex);
            
        set Value(T o)
        {
            self.setAt(theIndex, o)
        }
    };    
    
    T at(int n)
        = theArray[n];

    setAt(int n, T item)
    {
        theArray[n] := item;
    }    
}

// --- matrixof ---

public sealed Matrix<T>
{
    T[] theArray;
    int theRows;
    int theColumns;
    
    constructor allocate(int rows, int columns)
    {
        theRows := rows;
        theColumns := columns;
        
        int len := theRows * theColumns;
        
        theArray := new T[](len)
    }
    
    constructor allocate(rows, columns)
        <= allocate(cast int(rows), cast int(columns));
    
    constructor load(params)
    {
        theRows := params.Rows;
        theColumns := params.Columns;
        
        int len := theRows * theColumns;
        
        theArray := new T[](/*theRows * theColumns*/len);
        
        int i := 0;
        int j := 0;
        
        while (i < theRows)
        {
            j := 0;
            while (j < theColumns)
            {
                T val := params.at(i,j);
                
                self.setAt(i, j, val);
                
                j := j + 1
            };
            
            i := i + 1
        }
    }    
    
    int Length
    {
        get()
        {
            ^ theRows
        }
    }    

    int Columns
    {
        get()
        {
            ^ theColumns
        }
    }

    int Rows
    {
        get()
        {
            ^ theRows
        }
    }

    /// Writes an integer value at the specified position
    setAt(int i, int j, T v)
    {        
        int index := i * theColumns + j;
        
        theArray[index] := v
    }

    /// Reads an integer value at the specified position and saves it into an output variable
    read(int i, int j, ref T retVal)
    {
        int index := i * theColumns + j;
        
        retVal := theArray[index];
    }

    T at(int i, int j)
    {
        self.read(i, j, ref T retVal);
        ^ retVal
    }

    /// Returns a row
    at(i) = new
    {
        Length = theColumns;
        
        at(j)
            = self.at(i,j);
        
        setAt(j, v)
        {
            self.setAt(i,j,v)
        }
    };

    Enumerator enumerator() 
    {
        int i := 0;
        int j := -1;
        int length1 := theRows;
        int length2 := theColumns;
        
        ^ new Enumerator
          {
              get Value() = self.at(i, j);
              
              enumerable() = self;
              
              reset() { i := 0; j := -1; }
              
              bool next()
              {
                  ifnot (i < length1)
                  {
                     ^ false
                  };
                  
                  j := j + 1;
                  
                  ifnot (j < length2)
                  {
                      j := -1;
                      i := i + 1;
                      
                      ^ false
                  };
                  
                  ^ true
              }
          }
    }
}

// === basic memory classes ===

// --- BitArray ---

public sealed class BitArray
{
    int[] theArray;
    int   theLength;
    
    /// Creates the array with the specified length
    constructor allocate(int length)
    {
        theLength := length;
        theArray := new int[](length / 32 + 1)
    }
    
    int Length
    {
        get()
        {
            ^ theLength
        }
    }
    
    at(index)
        = self.at(cast int(index));
    
    bool at(int index)
    {
        int arr_index := index / 32;
        int bit_index := arr_index * 32;
        bit_index := index - arr_index;
        
        int bit_mask := 1;
        if (bit_index != 0)
            { bit_mask := bit_mask $shl bit_index };
        
        int bits := theArray[arr_index];
        
        ^ (bits & bit_mask) != 0
    }

    setAt(int index, bool value)
    {
        int arr_index := index / 32;
        int bit_index := arr_index * 32;
        bit_index := index - arr_index;
        
        int bit_mask := 1;
        if (bit_index != 0)
            { bit_mask := bit_mask $shl bit_index };
        
        int bits := theArray[arr_index];
        
        if(value)
        { 
            bits := bits | bit_mask 
        }
        else
        {  
            bit_mask := bit_mask.BInverted;
            
            bits := bits & bit_mask
        };
            
        theArray[arr_index] := bits
    }
    
    /// Returns the array enumerator
    Enumerator<bool> enumerator() = new BitArrayEnumerator(self);

    /// Returns the array indexer
    Indexer<bool> indexer() = new BitArrayIndexer(self);
}

// --- BitArray32 ---

public sealed class BitArray32
{
    int theValue;
    
    constructor load(int value)
    {
        theValue := value
    }
    
    int Length = 32;
    
    write(int value)
    {
        theValue := value
    }
    
    bool at(int index)
    {
        int bit_mask := 1;
        if (index != 0)
            { bit_mask := bit_mask $shl index };
        
        ^ (theValue & bit_mask) != 0
    }
    
    setAt(int index, bool value)
    {
        int bit_mask := 1;
        if (index != 0)
            { bit_mask := bit_mask $shl index };
        
        if(value)
        { 
            theValue := theValue | bit_mask 
        }
        else
        {  
            bit_mask := bit_mask.BInverted;
            
            theValue := theValue & bit_mask
        }
    }
    
    /// Returns the array enumerator
    Enumerator<bool> enumerator() = new BitArray32Enumerator(self);

    /// Returns the array indexer
    Indexer<bool> indexer() = new BitArray32Indexer(self);
}

// --- IntMatrix ---

public sealed class IntMatrix
{
    int[] theArray;
    int   theRows;
    int   theColumns;
    
    constructor allocate(int rows, int columns)
    {
        theRows := rows;
        theColumns := columns;
        
        theArray := new int[](columns * rows)
    }
        
    constructor allocate(rows,columns)
        <= allocate(cast int(rows), cast int(columns));
        
    /// Reads the matrix lengths to the output parameter
    readLengthsTo(ref int rows, ref int columns)
    {
        rows := theRows;
        columns := theColumns
    }

    int Length
    {
        get()
        {
            ^ theRows
        }
    }    

    int Columns
    {
        get()
        {
            ^ theColumns
        }
    }

    int Rows
    {
        get()
        {
            ^ theRows
        }
    }

    /// Writes an integer value at the specified position
    setAt(int i, int j, int v)
    {
        int index := i * theColumns + j;
        
        theArray[index] := v
    }

    /// Reads an integer value at the specified position and saves it into an output variable
    read(int i, int j, ref int retVal)
    {
        int index := i * theColumns + j;
        
        retVal := theArray[index];
    }

    int at(int i, int j)
    {
        int index := i * theColumns + j;
        int retVal := theArray[index];
        
        ^ retVal
    }

    /// Returns a row
    at(int i) = new Indexable<int>
    {
        Length = theColumns;
        
        Indexer<int> indexer() = new BaseIndexer<int>
        {
            this theIndex := 0;
            
            this theLength := self.Columns;
            
            get int Value()
                = self.at(i,theIndex);
                
            set Value(int o)
            {
                self.setAt(i,theIndex,o);
            }                        
        }; 
        
        int at(int j)
            = self.at(i,j);
        
        setAt(int j, int v)
        {
            self.setAt(i,j,v)
        }
    };
}

// --- RealMatrix ---

public sealed class RealMatrix
{
    real[] theArray;
    int    theRows;
    int    theColumns;
    
    constructor allocate(int rows, int columns)
    {
        theRows := rows;
        theColumns := columns;
        
        theArray := new real[](columns * rows)
    }
        
    /// Reads the matrix lengths to the output parameter
    readLengthsTo(ref int rows, ref int columns)
    {
        rows := theRows;
        columns := theColumns
    }

    int Length
    {
        get()
        {
            ^ theRows
        }
    }    

    int Columns
    {
        get()
        {
            ^ theColumns
        }
    }

    int Rows
    {
        get()
        {
            ^ theRows
        }
    }

    /// Writes a real value at the specified position
    setAt(int i, int j, real v)
    {
        int index := i * theColumns + j;
        
        theArray[index] := v
    }

    /// Reads a real value at the specified position and saves it into an output variable
    read(int i, int j, ref real retVal)
    {
        int index := i * theColumns + j;
        
        retVal := theArray[index];
    }

    real at(int i, int j)
    {
        int index := i * theColumns + j;
        real retVal := theArray[index];
        
        ^ retVal
    }

    at(int i) = new Indexable<real>
    {
        Length = theColumns;
        
        Indexer<real> indexer() = new BaseIndexer<real>
        {
            this theIndex := 0;
            
            this theLength := self.Columns;
            
            get real Value()
                = self.at(i,theIndex);
                
            set Value(real o)
            {
                self.setAt(i,theIndex,o);
            }                        
        }; 
        
        real at(int j)
            = self.at(i,j);
        
        setAt(int j, real v)
        {
            self.setAt(i,j,v)
        }
    };
}

// --- String ---

[info:"A UTF-8 literal value"]
public sealed const struct String : BaseValue
{
    __string byte[] theArray;

    /// Creates the empty literal
    const static string MinValue
        = "";
                
    [info:"Creates the literal string with specified length"]
    constructor allocate(int size)
        = new byte[](size + 1);

    /// Creates the literal string with specified length and copies the parameter value
    constructor copy(int size, string s)
        <= allocate(size)
    {
        extern elenart.core_subcopyz(self, 0, size, s);
    }

    /// Creates the literal string with specified length fills with specified character value        
    constructor fill(int size, char ch)
        <= allocate(ch.Length * size)
    {
        byte buffer[4];
        int  length := 0;
        intern system'core_routines'__literalconvertor.convert(ch, buffer, ref length);
        
        int offs := 0;
        for (int i := 0, i < size, i := i + 1)
        {
            for (int j := 0, j < length, j := j + 1) {
                theArray[offs] := buffer[j];
                offs := offs + 1;
            }
        }
    }

    /// Creates the literal string copy              
    constructor copy(string s)
        <= copy(s.Length, s);
        
    /// Creates the copy of a subarray        
    constructor copy(int index, int length, char[] array)
        <= allocate(stringConvertor.getSubLength(index, length, array))
    {
        extern elenart.core_scopychars(self, index, length, array);
    }

    internal constructor fromByteArray(int index, int size, byte[] buffer)
        <= allocate(size)
    {
        extern elenart.core_subcopyz(self, index, size, buffer);
    }
    
    /// Converts to UTF-16 literal
    wide cast()
    {
        int        i := 0;
        int        m := 0;
        int        page := 0;
        int        l := self.Length;
        short buffer[256];            
        if (l < 256)
        {
            intern system'core_routines'__wideconvertor.convert(self, buffer, ref m);

            ^ WideString.fromShortArray(0, m, buffer)
        };

        wide retVal := WideString.MinValue;
        while(l > 0)
        {
            page := l;
            if (page > 256)
            {
                page := 256
            };
            
            intern system'core_routines'__wideconvertor.convert(self.Substring(i,page), buffer, ref m);
            
            retVal := retVal.add(WideString.fromShortArray(0, m, buffer));
            
            i := i + page;
            l := l - page
        };
        
        ^ retVal
    }
     
    Enumerable<char> cast() = new Enumerable<char>{ Enumerator<char> enumerator() => self; };

    string toPrintable() = self;

    bool equal(o)
    {
        var s := cast string(o) ?? nil;
        (nil == s)
            ? { ^ false } : { ^ self.equal(s) }
    }

    bool less(o)
        <= less(cast string(o));
        
//    bool equal(BaseVariable v)
//        <= equal(v.Value);
//        
//    bool less(BaseVariable v)
//        <= less(v.Value);
        
    /// Compares the values
    bool equal(string s)
        => intern system'core_routines'__literal;

    /// Compares the values
    bool less(string s)
        => intern system'core_routines'__literal;

    /// Compares the values
    bool equal(wide s)
        = cast wide(self).equal(s);

    /// Compares the values
    bool less(wide s)
        = cast wide(self).less(s);

    /// Compares the values
    bool equal(char ch)
        <= equal(cast string(ch));

    /// Compares the values
    bool less(char ch)
        <= less(cast string(ch));

    [info:"Returns the character at the specified index"]
    char at(int n)
    {
        int len := self.Length;
        if (n < 0 || n >= len)
        {
            OutOfRangeException.raise()
        };
                
        char ch := extern elenart.core_strtochar(n, self);
        
        ^ ch;
    }
    
    string add(o)
        <= add(cast string(o));

    [info:"Concatinates the values"]
    string add(BaseVariable v)
        <= add(v.Value);
        
    [info:"Concatinates the values"]
    string add(string s)
    {
        int len1 := self.Length;
        int len2 := s.Length;
        string ret := String.allocate(len1+len2);
        extern elenart.core_insert(ret,self,0,len1);
        extern elenart.core_insert(ret,s,len1,len2);
    
        ^ ret; 
    }

    [info:"Saves the index of the first occurence of the specified substring into an output parameter.<br/>If the substring is not found. returns -1"]
    int indexOf(int index, string literal)
    {
        int sublen := literal.Length;
        int retVal := extern elenart.core_sseek(self, literal, index, sublen);
        
        ^ retVal
    }
    //    => intern system'core_routines'__literal;

    /// Saves the index of the first occurence of the specified symbol into an output parameter.
    ///
    /// If the substring is not found. returns -1
    int indexOf(int index, char ch)
    {
        byte buffer[4];
        intern system'core_routines'__literalconvertor.convert(ch, buffer, ref int sublen);
        
        int retVal := extern elenart.core_sseek(self, buffer, index, sublen);
        
        ^ retVal
    }

    [info:"Inserts the substring into and returns the new one"]
    string insert(int index, string s)
    {
        int len1 := self.Length;
        int len2 := s.Length;
        int dindex := index + len2;
        string ret := String.allocate(len1+len2);
        extern elenart.core_insert(ret,self,0,index);
        extern elenart.core_insert(ret,s,index,len2);
        extern elenart.core_sadd(ret,self,index,dindex);
    
        ^ ret;         
    }
    
    /// Inserts a character into and returns the new one
    string insert(int index, char ch)
        = self.insert(index, cast string(ch));

    [info:"Deletes the substring from the object and returns the new one"]
    string delete(int index, int length)
    {
        int len1 := self.Length;
        int sindex := index + length;
        string ret := String.allocate(len1-length);
        extern elenart.core_insert(ret,self,0,index);
        extern elenart.core_sadd(ret,self,sindex,index);
    
        ^ ret;         
    }

    [info:"Returns the substring"]
    string Substring(int index, int length)
    {
        auto subs := String.allocate(length);
        
        extern elenart.core_subcopyz(subs, index, length, self);
        
        ^ subs
    }

    int Length
    {
        /// Reads the literal length into an output parameter
        get()
        {
            intern system'core_routines'__literal.readLengthTo(self, ref int retVal);
            
            ^ retVal
        }
    }

    [info:"Saves the literal into a UTF-32 character array"]
    save(int index, char[] array, ref int retVal)
        => intern system'core_routines'__literal;

    internal saveToByteArray(int index, byte[] array, int retVal)
        => intern system'core_routines'__literal;

    [info:"Returns the string enumerator"]
    Enumerator<char> enumerator() = new StringEnumerator(self);

    /// Returns the string indexer
    Indexer<char> indexer() = new StringIndexer(self);
    
    /// Clones the value
    string clone() = String.copy(self);
}

// --- emptyString ---

/// An empty literal value
public const string emptyString = String.MinValue;

// --- WideString ---

[info:"A UTF-18 literal value"]
public sealed const struct WideString : BaseValue
{
    __string short[] theArray;

    /// Creates the empty literal
    const static wide MinValue
        = ""w;
        
    /// Creates the literal string with specified length
    constructor allocate(int size)
        = new short[](size + 1);

    /// Creates the literal string with specified length and copies the parameter value
    constructor copy(int size, wide s)
        <= allocate(size)
    {
        extern elenart.core_wsubcopyz(self, 0, size, s);
    }

    /// Creates the literal string with specified length fills with specified character value        
    constructor fill(int size, char ch)
        <= allocate(ch.WideLength * size)
    {
        short buffer[2];
        int   length := 0;
        intern system'core_routines'__wideconvertor.convert(ch, buffer, ref length);
        
        int offs := 0;
        for (int i := 0, i < size, i := i + 1)
        {
            for (int j := 0, j < length, j := j + 1) {
                theArray[offs] := buffer[j];
                offs := offs + 1;
            }
        }
    }

    /// Creates the literal string copy
    constructor copy(wide s)
        <= copy(s.Length, s);
        
    /// Creates the copy of a subarray
    constructor copy(int index, int length, char[] array)
        <= allocate(wideConvertor.getSubLength(index, length, array))
    {
        extern elenart.core_wscopychars(self, index, length, array);
    }

    internal constructor fromShortArray(int index, int size, short[] array)
        <= allocate(size)
    {
        extern elenart.core_wsubcopyz(self, index, size, array);
    }

    /// Converts to UTF-16 literal
    string cast()
    {
        int  i := 0;
        int  m := 0;
        int  page := 0;
        int  l := self.Length;
        byte buffer[256];
        if (l < 128)
        {
            intern system'core_routines'__literalconvertor.convert(self, buffer, ref m);
            
            ^ String.fromByteArray(0, m, buffer)
        };

        string retVal := String.MinValue;
        while(l > 0)
        {
            page := l;
            if (page > 127)
            {
               page := 127
            };
            
            intern system'core_routines'__literalconvertor.convert(self.Substring(i, page), buffer, ref m);
            
            retVal := retVal.add(String.fromByteArray(0, m, buffer));
            
            i := i + page;
            l := l - page
        };
        
        ^ retVal
    }

    Enumerable<char> cast() = new Enumerable<char>{ Enumerator<char> enumerator() => self; };

    string toPrintable() = self;

    bool equal(o)
    {
        var s := cast wide(o) ?? nil;
        (nil == s)
            ? { ^ false } : { ^ self.equal(s) }
    }

    bool less(o)
        <= less(cast wide(o));
        
//    bool equal(BaseVariable v)
//        <= equal(v.Value);
//        
//    bool less(BaseVariable v)
//        <= less(v.Value);

    /// Compares the values
    bool equal(wide s)
        => intern system'core_routines'__wide;

    /// Compares the values
    bool less(wide s)
        => intern system'core_routines'__wide;

    /// Compares the values
    bool equal(char ch)
        <= equal(cast wide(ch));

    /// Compares the values
    bool less(char ch)
        <= less(cast wide(ch));

    [info:"Returns the character at the specified index"]
    char at(int n)
    {
        int len := self.Length;
        if (n < 0 || n >= len)
        {
            OutOfRangeException.raise()
        };
                
        char ch := extern elenart.core_wstrtochar(n, self);
        
        ^ ch;
    }

    wide add(o)
        <= add(cast wide(o));

    /// Concatinates the values
    wide add(BaseVariable v)
        <= add(v.Value);
    
    [info:"Concatinates the values"]
    wide add(wide s)
    {
        int len1 := self.Length;
        int len2 := s.Length;
        wide ret := WideString.allocate(len1+len2);
        extern elenart.core_winsert(ret,self,0,len1);
        extern elenart.core_winsert(ret,s,len1,len2);
    
        ^ ret; 
    }
       
    /// Concatinates the values
    wide add(char ch)
        = self.add(wideConvertor.convert(ch));

    /// Concatinates the values
    wide add(string s)
        = self.add(cast wide(s));

    /// Returns the index of the first occurence of the specified character.
    ///
    /// If the substring is not found. returns -1
    int indexOf(int index, char ch)
    {
        short buffer[2];
        intern system'core_routines'__wideconvertor.convert(ch, buffer, ref int sublen);
        
        int retVal := extern elenart.core_sseek(self, buffer, index, sublen);
        
        ^ retVal
    }

    /// Returns the index of the first occurence of the specified substring.
    ///
    /// If the substring is not found. returns -1
    int indexOf(int index, wide s)
    {
        int sublen := s.Length;
        int retVal := extern elenart.core_wseek(self, s, index, sublen);
        
        ^ retVal
    }

    [info:"Inserts the substring into and returns the new one"]
    wide insert(int index, wide s)
    {
        int len1 := self.Length;
        int len2 := s.Length;
        int dindex := index + len2;
        wide ret := WideString.allocate(len1+len2);
        extern elenart.core_winsert(ret,self,0,index);
        extern elenart.core_winsert(ret,s,index,len2);
        extern elenart.core_wadd(ret,self,index,dindex);
    
        ^ ret;         
    }
        
    /// Inserts the substring into and returns the new one
    wide insert(int index, string s)
        = self.insert(index, cast wide(s));

    /// Inserts a character into and returns the new one
    wide insert(int index, char ch)
        = self.insert(index, wideConvertor.convert(ch));

    [info:"Deletes the substring from the object and returns the new one"]
    wide delete(int index, int length)
    {
        int len1 := self.Length;
        int sindex := index + length;
        wide ret := WideString.allocate(len1-length);
        extern elenart.core_winsert(ret,self,0,index);
        extern elenart.core_wadd(ret,self,sindex,index);
    
        ^ ret;         
    }
    
    [info:"Returns the substring"]
    wide Substring(int index, int length)
    {
        auto subs := WideString.allocate(length);
        
        extern elenart.core_wsubcopyz(subs, index, length, self);
        
        ^ subs
    }

    /// Reads the literal length into an output parameter
    int Length
    {
        /// Reads the literal length into an output parameter
        get()
        {
           intern system'core_routines'__wide.readLengthTo(self, ref int retVal);
           
            ^ retVal 
        }
    }

    /// Saves the literal into a UTF-32 character array
    save(int index, char[] array, ref int length)
        => intern system'core_routines'__wide;

    internal saveToShortArray(int index, short[] array, int length)
        => intern system'core_routines'__wide;

    /// Returns the object enumerator
    Enumerator<char> enumerator() = new WideStringEnumerator(self);
    
    /// Returns the object indexer
    Indexer<char> indexer() = new WideStringIndexer(self);
    
    /// Clones the value
    wide clone() = WideString.copy(self);
}

// --- emptyWideString ---

/// An empty wide literal value
public const wide emptyWideString = WideString.MinValue;

// --- Variant ---

[info:"A basic type variant class"]
public sealed class Variant
{
    object value;
    
    constructor(object)
    {
        value := object
    }
    
    /// Tries to convert the value to a byte number
    byte cast()
        = byteConvertor.convert(value);

    /// Tries to convert the value to a short integer number
    short cast()
        = shortConvertor.convert(value);

    /// Tries to convert the value to a character
    char cast()
        = charConvertor.convert(value);

    /// Tries to convert the literal value to the integer number
    int cast()
        = intConvertor.convert(value);

    /// Tries to convert the literal value to the long integer number
    long cast()
        = longConvertor.convert(value);

    /// Tries to convert the literal value to the real number
    real cast()
        = realConvertor.convert(value);

    /// Returns a literal value
    string cast()
        = stringConvertor.convert(value);
    
    /// Returns a wide literal value
    wide cast()
        = wideConvertor.convert(value);

    string toPrintable()
        => value;

    private castValueTo(target)
        = new{
            castTo(o)
                = o.toPrintable();
            
            castTo(BaseVariable v)
                = self.castValueTo(v.Value);
            
            castTo(int val)
                = cast int(self);
                
            castTo(byte val)
                = cast byte(self);
                
            castTo(short val)
                = cast short(self);
                
            castTo(long val)
                = cast long(self);
                
            castTo(real val)
                = cast real(self);
                
            castTo(string val)
                = cast string(self);
                
            castTo(wide val)
                = cast wide(self);
        }.castTo(target);

    saveTo(target)
    {
        target.Value := self.castValueTo(target.Value);
        
        ^ target
    }

    add(p)
        = p.add(self.castValueTo(p));

    subtract(p)
        = self.castValueTo(p).subtract(p);

    multiply(p)
        = self.castValueTo(p).multiply(p);

    divide(p)
        = self.castValueTo(p).divide(p);

    bool equal(p)
        = self.castValueTo(p).equal(p);
}

// --- Array ---

/// An array
public sealed Array : Indexable
{
    __string object[] theArray;
    
    /// Creates an empty array
    constructor MinValue
        = new object[](0);
    
    /// Creates the array with the specified length
    constructor allocate(int len)
        = new object[](len);

    /// Creates a copy of the provided sub array
    constructor copy(int index, int length, Array array)
        <= allocate(length)
    {
        intern system'core_routines'__array.copy(self, index, length, array)
    }
        
    /// Concatinates the arrays    
    add(Array a)
    {
        int length1 := self.Length;
        int length2 := a.Length;
        
        auto retVal := Array.allocate(length1 + length2);

        for (int i:=0, i < length1, i += 1) {
            retVal[i] := theArray[i]
        };

        for (int i:=0, i < length2, i += 1) {
            retVal[length1 + i] := a[i]
        };
            
        ^ retVal
    }

    int Length
    {
        /// Reads the array length into an output parameter
        get()
        {
            intern system'core_routines'__array.readLengthTo(self, ref int retVal);
            
            ^ retVal
        }
    }

    /// Sets the specified array member
    setAt(int n, object o)
    {
        theArray[n] := o
    }

    /// Returns the specified array member
    object at(int n)
        = theArray[n];
 
    /// Returns the array enumerator
    Enumerator enumerator() = new ArrayEnumerator(self);

    /// Returns the array indexer
    Indexer indexer() = new ArrayIndexer(self);

    object[] Value = theArray;

    /// Clones the array
    Array clone()
        = Array.copy(0, self.Length, self);
}

// === Convertors ===

// --- stringConvertor ---

[info:"A literal convertor"]
public singleton stringConvertor
{
    int getSubLength(int index, int length, char[] array)
    {
        int len;
        extern elenart.core_slen_ch(index, length, array, ref len);
        
        ^ len
    }
    
    string convert(BaseVariable v)
        <= convert(*v);
    
    string convert(BaseVariable v, int radix)
        <= convert(*v, radix);
    
    string convert(char ch)
    {
        byte buffer[4];
        int  length := 0;

        intern system'core_routines'__literalconvertor.convert(ch, buffer, ref length);
        
        ^ String.fromByteArray(0, length, buffer)
    }
    
    /// Converts the parameter to the literal value
    string convert(int n)
    {
        byte buffer[12];
        extern elenart.core_inttostr(n, 10, buffer, ref int length);
        
        ^ String.fromByteArray(0, length, buffer)
    }
    
    /// Converts the parameter to the literal value
    string convert(uint n)
    {
        byte buffer[12];
        extern elenart.core_uinttostr(n, 10, buffer, ref int length);
        
        ^ String.fromByteArray(0, length, buffer)
    }

    /// Converts the parameter to the literal value
    string convert(short n)
    {
        byte buffer[12];
        extern elenart.core_uinttostr(n, 10, buffer, ref int length);
        
        ^ String.fromByteArray(0, length, buffer)
    }

    /// Converts the parameter to the literal value
    string convert(byte n)
    {
        byte buffer[4];
        extern elenart.core_uinttostr(n, 10, buffer, ref int length);
        
        ^ String.fromByteArray(0, length, buffer)
    }

    /// Converts the parameter to the literal value
    string convert(long n)
    {
        byte buffer[64];
        int  length := 0;
        
        intern system'core_routines'__literalconvertor.convert(n, 10, buffer, ref length);
        
        ^ String.fromByteArray(0, length, buffer)
    }

    /// Converts the parameter to the literal value with the provided radix
    string convert(int n, int radix)
    {
        byte buffer[48];
        extern elenart.core_inttostr(n, radix, buffer, ref int length);
        
        ^ String.fromByteArray(0, length, buffer)
    }

    /// Converts the parameter to the literal value with the provided radix
    string convert(uint n, int radix)
    {
        byte buffer[48];
        extern elenart.core_uinttostr(n, radix, buffer, ref int length);
        
        ^ String.fromByteArray(0, length, buffer)
    }

    /// Converts the parameter to the literal value with the provided radix
    string convert(short n, int radix)
    {
        byte buffer[32];
        extern elenart.core_inttostr(n, radix, buffer, ref int length);
        
        ^ String.fromByteArray(0, length, buffer)
    }

    /// Converts the parameter to the literal value with the provided radix
    string convert(byte n, int radix)
    {
        byte buffer[8];
        extern elenart.core_uinttostr(n, radix, buffer, ref int length);
        
        ^ String.fromByteArray(0, length, buffer)
    }

    /// Converts the parameter to the literal value
    string convert(long n, int radix)
    {
        byte buffer[64];
        int  length := 0;
        
        intern system'core_routines'__literalconvertor.convert(n, radix, buffer, ref length);
        
        ^ String.fromByteArray(0, length, buffer)
    }

    /// Converts the parameter to the literal value
    string convert(real n)
    {
        byte buffer[32];
        int  length := 0;
        
        intern system'core_routines'__literalconvertor.convert(n, 24, buffer, ref length);
        
        ^ String.fromByteArray(0, length, buffer)
    }

    string convert(wide w)
        = cast string(w);
    
    /// Converts the parameter to the literal value
    convert(char ch, byte[] buffer, ref int length)
    {
        intern system'core_routines'__literalconvertor.convert(ch, buffer, ref length)
    }

    convert(int n, byte[] buffer, ref int length)
    {
        extern elenart.core_inttostr(n, 10, buffer, ref length);
    }
    
    convert(uint n, byte[] buffer, ref int length)
    {
        extern elenart.core_uinttostr(n, 10, buffer, ref length);
    }

    /// Returns a parameter                
    string convert(string s)
        = s;
        
    /// Returns the literal presentation of the parameter
    string convert(o)
        = o.toPrintable();
}

// --- wideConvertor ---

[info:"A wide literal convertor"]
public singleton wideConvertor
{
    int getSubLength(int index, int length, char[] array)
    {
        int len;
        extern elenart.core_wslen_ch(index, length, array, ref len);
        
        ^ len
    }       
    
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
