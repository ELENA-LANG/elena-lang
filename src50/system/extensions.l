// --- common methods ---

/// A generic extender
public extension objectOp
{
    __getClass()
        => intern system'core_routines'__object;

    __getParentClass()
        => intern system'core_routines'__object;

    bool instanceOf(type)
        => intern system'core_routines'__object;

    string __getClassName()
    {
        dirty_ptr c := self.__getClass();

        byte dump[256];
        int         len := extern elenart.LoadClassName(c, dump, 255);
         
        if (len == 0)
            { ^ "Unknown class" };
        
        ^ String.fromByteArray(0,len,dump)
    }
    
    /// Returns true if the specified object instances are equal; otherwise, false.
    bool equalReference(o)
        => intern system'core_routines'__object;
        
    /// Returns true if the object is nil
    bool isNil()
        = nil == self;
        
    /// Returns true if the object is literal or character
    isLiteral()
    {
        var o := cast String(self) ?? nil;
        if (nil == o)
        {
            o := cast WideString(self) ?? nil;
            if (nil == o)
            {
                o := cast CharValue(self) ?? nil;
                
                if (nil == o)
                {
                    ^ false
                }
            }
        };
        
        ^ true
    }
    
    /// Returns true if the object is integer numereric
    isInteger()
    {
        var b := self.instanceOf(IntNumber);
        if (b)
        {
            ^ true
        };
        
        b := self.instanceOf(UIntNumber);
        if (b)
        {
            ^ true
        };
        
        b := self.instanceOf(LongNumber);
        if (b)
        {
            ^ true
        };
        
        b := self.instanceOf(ShortNumber);
        if (b)
        {
            ^ true
        };
        
        b := self.instanceOf(ByteNumber);
        if (b)
        {
            ^ true
        };  
        
        ^ false
     }
    
    /// Returns true if the object is numereric
    isNumeric()
    {
        if (self.instanceOf(RealNumber))
        {
            ^ true
        }
        else {
            ^ self.isInteger();
        }
    }
        
    bool isEmpty()
    {
        int len := self.Length;
        
        ^ len == 0;
    }
        
    bool isNonempty()
    {
        int len := self.Length;
        
        ^ len != 0;
    }
        
    /// Returns true if the parameter is equal to the object or false. It does not raise an exception if the objects are not compatible
    bool safeEqual(o)
        = self.equal:o \ back:false;

    doWith(action)
        = action(self);

    reallocate(int newLength)
    {
        int len := self.Length;
        
        var class := self.__getClass();
        var newArr := class.allocate(newLength);
        
        for(int i := 0, i < len, i += 1)
        {
            newArr[i] := self[i];
        };
        
        ^ newArr
    }
                
    shift(int n)
    {
        if (n < 0)
        { 
            ^ self.shiftLeft(n.Negative) 
        }
        else
        {
            ^ self.shiftRight(n)
        }
    }
}

// --- Operations ---

// --- byteOp ---

public extension byteOp : ByteNumber
{
    int Hashcode
    {
        // Reads the object hash code into the output parameter
        get()
        {
            intern system'core_routines'__int.readHashCodeTo(self, ref int retVal);
            
            ^ retVal
        }
    }

    get byte Absolute()
    {
        byte n := self;
        if (n < 0)
            { n := n * -1 };
                
        ^ new ByteNumber(n)
    }
    
    byte mod(operand)
    {
        var retVal := self / operand * operand;
        
        ^ byteConvertor.convert(retVal)
    }    
    
    byte power(int y)
    {
        byte x := self;
        byte r := 1;
        
        for (int i := 0, i < y, i += 1)
        {
            r := r * x
        };
        
        ^ new ByteNumber(r)
    }
    
    byte sqr()
    {
        byte retVal := 0;
        
        retVal := self * self;
        
        ^ new ByteNumber(retVal)
    }

    bool isOdd()
        = self.mod:2 != 0;
        
    bool isEven()
        = self.mod:2 == 0;
        
    bool isZero()
        = self.equal(0);
        
    bool isPositive()
        = self > 0;
        
    bool isNegative()
        = self < 0;
        
    bool isNonnegative()
        = self <= 0;
}

// --- shortOp ---

public extension shortOp : ShortNumber
{
    int Hashcode
    {
        // Reads the object hash code into the output parameter
        get()
        {
            intern system'core_routines'__int.readHashCodeTo(self, ref int retVal);
            
            ^ retVal
        }
    }
    
    get short Absolute()
    {
        short n := self;
        if (n < 0)
            { n := n * -1 };
                            
        ^ new ShortNumber(n)
    }
    
    short mod(operand)
    {
        var retVal := self / operand * operand;
        
        ^ shortConvertor.convert(retVal)
    }
    
    short power(int y)
    {
        short x := self;
        short r := 1;
        
        for (/*int*/IntNumber i := 0, i < y, i += 1)
        {
            r := r * x
        };
        
        ^ new ShortNumber(r)
    }
    
    short sqr()
    {
        short n := self;
        short retVal := 0;
        
        retVal := n * n;
        
        ^ new ShortNumber(retVal)
    }

    bool isOdd()
        = self.mod:2 != 0;
        
    bool isEven()
        = self.mod:2 == 0;
        
    bool isZero()
        = self.equal(0);
        
    bool isPositive()
        = self > 0;
        
    bool isNegative()
        = self < 0;
        
    bool isNonnegative()
        = self <= 0;
    
    get byte High()
    {
        short w := self;
        w := w & 0FF00h;
        w := w.shiftRight(8);
        
        ^ byteConvertor.convert(w)
    }
        
    get byte Low()
    {
        short w := self;
        w := w & 0FFh;
        
        ^ byteConvertor.convert(w)
    }
}

// --- intOp ---

public extension intOp : IntNumber
{
    int Hashcode
    {
        // Reads the object hash code into the output parameter
        get()
        {
            intern system'core_routines'__int.readHashCodeTo(self, ref int retVal);
            
            ^ retVal
        }
    }
    
    get int Absolute()
    {
        int n := self;
    
        if (n < 0)
           { n := n * -1 };
    
        ^ n
    }

    int power(int y)
    {
        int x := self;
        int r := 1;
        if (x == 0)
            { r := 0 };
        
        for(int i := 0, i < y, i += 1)
        {
            r := r * x
        };
        
        ^ r
    }
  
    int mod(int y)
    {
        int x := self;
        
        int z := x / y;
        z := z * y;
        
        int retVal := x - z;
        
        ^ retVal
    }

    int mod(o)
    {
        int n := self;
        
        int retVal := self.mod(cast int(o));
        
        ^ retVal
    }
    
    real realDiv(n)
        = new RealNumber(self).divide:n;
   
    int sqr()
    {
        int n := self;        
        
        n := n * n;
        
        ^ n
    }
    
    int sqrt()
    {   
        int num := self;
        
        if (num == 0)
            { ^ 0 };
            
        int n := num / 2 + 1;
        int n1 := (n + (num / n)) / 2;
        while (n1 < n)
        {
            n := n1;
            n1 := (n + (num / n)) / 2
        };
        
        ^ n
    }

    bool isOdd()
        = (self & 1) != 0;
        
    bool isEven()
        = self.band(1) == 0;
        
    bool isZero()
        = self.equal(0);
        
    bool isPositive()
        = self > 0;
        
    bool isNegative()
        = self < 0;
        
    bool isNonnegative()
        = self <= 0;

    get short High()
    {
        int w := self;
        w := w & 0FFFF0000h;
        w := w.shiftRight(16);
        
        ^ shortConvertor.convert(w)
    }

    get short Low()
    {
        int w := self;
        w := w & 0FFFFh;
        
        ^ shortConvertor.convert(w)
    }
}

public extension uintOp : UIntNumber
{
    int Hashcode
    {
        // Reads the object hash code into the output parameter
        get()
        {
            intern system'core_routines'__int.readHashCodeTo(self, ref int retVal);
            
            ^ retVal
        }
    }
}
    
// --- longOp ---

public extension longOp : LongNumber
{
    int Hashcode
    {
        // Reads the object hash code into the output parameter
        get()
        {
            intern system'core_routines'__int.readHashCodeTo(self, ref int retVal);
            
            ^ retVal
        }
    }
    
    get long Absolute()
    {
        long n := self;

        if (n < 0l)
            { n := n * -1l };
                
        ^ new LongNumber(n)
    }
    
    long mod(n)
    {
        long x := self;
        long y := cast long(n);
        
        long z := x / y;
        z := z * y;
        
        ^ new LongNumber(x - z)
    }
    
    long power(long y)
    {
        long x := self;
        long r := 1;
        
        for(long i := 0l, i < y, i += 1l)
        {
            r := r * x;
        };
        
        ^ new LongNumber(r)
    }
    
    long power(int y)
    {
        long x := self;
        long r := 1;
        
        int i := 0;
        while (i < y)
        {
            r := r * x;
            
            i += 1;
        };
        
        ^ new LongNumber(r)
    }
    
    long sqr()
    {
        long n := self;        
        long retVal := 0l;
        
        retVal := n * n;
        
        ^ new LongNumber(retVal)
    }

    bool isOdd()
        = self.mod:2l != 0;
        
    bool isEven()
        = self.mod:2l == 0;
        
    bool isZero()
        = self.equal(0l);
        
    bool isPositive()
        = self > 0l;
        
    bool isNegative()
        = self < 0l;
        
    bool isNonnegative()
        = self <= 0l;

    get int High()
    {
        long w := self;
        w := w & -4294967296l;
        w := w.shiftRight(32);
        
        ^ intConvertor.convert(w)
    }

    get int Low()
    {
        long w := self;
        w := w & 4294967295l;
        
        ^ intConvertor.convert(w)
    }
}

// --- realOp ---

public extension realOp : RealNumber
{        
    int Hashcode
    {
        // Reads the object hash code into the output parameter
        get()
        {
            intern system'core_routines'__int.readHashCodeTo(self, ref int retVal);
            
            ^ retVal
        }
    }
    
    get real Absolute()
    {
        real n := self;        
        real retVal := realOpController.abs(n);
        
        ^ retVal
    }
        
    real exp()
    {
        real n := self;        
        real retVal := realOpController.exp(n);
        
        ^ retVal
    }
    
    real ln()
    {
        real n := self;
        real retVal := realOpController.ln(n);
        
        ^ retVal
    }
    
    real sqrt()
    {
        real n := self;
        real retVal := realOpController.sqrt(n);
        
        ^ retVal
    }

    real power(real y)
    {
        real x := self;
        
        (x == 0.0r)
            ? { ^ 0.0r };
                        
        (y == 0.0r)
            ? { ^ 1.0r };
            
        x := realOpController.ln(x);
        
        x := x * y;
        
        x := realOpController.exp(x);
            
        ^ x            
    }

    real power(int y)
    {
        real x := self;

        (x == 0.0r)
            ? { ^ 0.0r };
                        
        (y == 0)
            ? { ^ 1.0r };
                        
        if (x < 0.0r)
        {
            x := x * -1.0r;
            
            x := realOpController.ln(x);
            
            x := x * y;
            
            x := realOpController.exp(x);
            
            (y.isOdd())
                ? { x := x * -1.0r }
        }
        else
        {
            x := realOpController.ln(x);
            
            x := x * y;
            
            x := realOpController.exp(x)
        };
        
        ^ x
    }

    real sqr()
    {
        real n := self;        
        real retVal := 0.0r;
        
        retVal := n * n;
        
        ^ retVal
    }

    get real Rounded()
    {
        real n := self;        
        real retVal := realOpController.round(n);
        
        ^ retVal
    }
    
    real round(int precision)
    {
        real a := self;
        bool neg := false;
        if(self.isNegative())
        {
            a := a * -1.0r;
            neg := true;
        };
           
        int b := 10.power(precision);
        real r := a * b + 0.5r;
        
        int c := intConvertor.convert(r.Rounded);
    
        a := c.realDiv(b);
    
        ^ neg ? (a * -1.0r) : a
    }
    
    real truncate(int precision)
    {
        real a := self;
        bool neg := false;
        if(self.isNegative())
        {
            a := a * -1.0r;
            neg := true
        };
       
        int b := 10.power(precision);
        real c := (a * b).Integer; // returns the integer part of the real number

        a := c / b;

        ^ neg ? (a * -1.0r) : a 
    }

    get real Integer()
    {
        real n := self;
        real retVal := realOpController.truncate(n);
        
        ^ retVal
    }

    real frac()
    {
        real n := self;
        real retVal := realOpController.abs(n);
        
        retVal := n - retVal;
        
        ^ retVal
    }

    real sin()
    {
        real n := self;  
        real retVal := realOpController.sin(n);
        
        ^ retVal
    }

    real cos()
    {
        real n := self;
        real retVal := realOpController.cos(n);
        
        ^ retVal
    }

    real tan()
    {
        real n := self;
        
        real sinVal := realOpController.sin(n);
        real cosVal := realOpController.cos(n);
        
        n := sinVal / cosVal;
        
        ^ n
    }

    real arctan()
    {
        real n := self;
        real retVal :=realOpController.arctan(n);
        
        ^ retVal
    }
            
    real arcsin()
    {
        real n := self;
        real retVal := 0.0r;
        
        retVal := n * n;    
        retVal := 1.0r - retVal;
        real arg := realOpController.sqrt(retVal);
        
        arg := n / arg;
        retVal := realOpController.arctan(arg);
        
        ^ retVal
    }
            
    real arccos()
    {
        real n := self;        
        real retVal := 0.0r;
        
        retVal := n * n;    
        retVal := 1.0r - retVal;        
        real arg := realOpController.sqrt(retVal);
        
        arg := arg / n;
        retVal := realOpController.arctan(arg);
        
        ^ retVal
    }
            
    get real Radian()
    {
        real value := self;
        
        value := value * Pi_value;
        value := value / 180.0r;
        
        ^ value
    }
            
    get real Degree()
    {
        real value := self;
        
        value := value * 180.0r;
        value := value / Pi_value;
        
        ^ value
    }
            
    get real Reciprocal()
    {
        real value := self;
        
        value := 1.0r / value;
        
        ^ value
    }    
            
    bool isZero()
        = self == 0.0r;
        
    bool isPositive()
        = self > 0.0r;
        
    bool isNegative()
        = self < 0.0r;
        
    bool isNonnegative()
        = self <= 0.0r;
}

// --- intArrayOp ---

public extension intArrayOp : Array<int>
{
    /// Copies the short array at the specified position
    write(int index, int length, int[] array)
        => intern system'core_routines'__intarray;

    exchange(int i1, int i2)
    {
        int n1 := self[i1];
        int n2 := self[i2];

        self[i1] := n2;
        self[i2] := n1;
    }

    populate(Func<int,int> f)
    {
        int i := 0;
        int l := self.Length;
        
        while (i < l)
        {
            self[i] := f(i);
            
            i := i + 1
        };
        
        ^ self
    }
    
    populate(Func1 f)
        = self.populate:(int n){ ^cast int(f(n)) };
}

// --- arrayOp ---

public extension weakArrayOp
{
    Array Subarray(int index, int length)
    {
        auto it := self.indexer();
        it.Index := index;
        
        auto a := Array.allocate(length);
        for (int i := 0, i < length, i += 1)
        {
            a[i] := it.get();
            
            it.appendIndex(1)
        };
        
        ^ a;
    }
}

public extension arrayOp<T> : Array<T>
{
    T[] Subarray(int index, int length)
    {
        ^ new Array<T>(self, index, length)
    }
}

// --- intmatrixOp ---

public extension intMatrixOp : IntMatrix
{
    populate(Func<int,int,int> fn)
    {
        int n := 0;
        int m := 0;
        self.readLengthsTo(ref n, ref m);
        
        int i := 0;
        int j := 0;
        while (i < n)
        {
            j := 0;
            while (j < m)
            {
                int v := fn(i, j);
                
                self.write(i, j, v);
                
                j := j + 1
            };
            
            i := i + 1
        };
        
        ^ self
    }
}

// --- literalOp ---

public extension stringOp : String
{
    int Hashcode
    {
        /// Reads the object hash code into the output parameter
        get()
        {
            int len := self.Length;
            
            int h := extern elenart.core_utf8hashcode(self, len);
            
            ^ h
        }    
    }

    int indexOf(string s)
        = self.indexOf(0, s);

    int indexOf(char ch)
        = self.indexOf(0, ch);
        
    string Substring(int index)
    {
        int len := self.Length;
        if (len > index)
        {
            len := len - index;
            
            ^ self.Substring(index, len)
        }
        else
        {
            ^ emptyString
        }        
    }        
    
    string trimLeft(char ch)
    {
        int  index := 0;
        int  len := self.Length;
        char current := $0;
        int  chLen := ch.Length;
        
        while(index < len)
        {
            current := self[index];
            
            ifnot(current == ch)
            { 
                if (index == 0)
                { 
                    ^ self 
                }
                else
                { 
                    ^ self.delete(0, index) 
                }
            };
            
            index += chLen
        };
        
        ^ emptyString
    }
    
    string trimLeft()
        = self.trimLeft($32);
       
    string trimRight(char ch)
    {
        int  len := self.Length;
        int  index := len - 1;
        int  prev := len;
        char current := $0;
        int  chLen := ch.Length;
        
        while (index >= 0)
        {
            current := self[index];
            
            ifnot (current == ch)
                { ^ self.delete(prev,len - prev) };
            
            prev := index;
            index -= chLen
        };
        
        ^ emptyString
    }
    
    string trimRight()
        = self.trimRight($32);
        
    string trim(char ch)
        = self.trimLeft(ch).trimRight(ch);
    
    string trim()
        = self.trimLeft($32).trimRight($32);
    
    string padLeft(int length)
        = self.padLeft($32, length);
    
    string padLeft(char ch, int length)
    {
        int currentLength := self.Length;
        
        if (currentLength < length)
            { ^ String.fill(length - currentLength, ch).add(self) };
            
        ^ self
    }
    
    string padRight(int length)
        = self.padRight($32, length);
    
    string padRight(char ch, int length)
    {
        int currentLength := self.Length;

        if (currentLength < length)
            { ^ self.add(String.fill(length - currentLength, ch)) };
            
        ^ self
    }

    bool startingWith(string s)
    {
        int n := self.indexOf(0, s);
        
        ^ 0 == n
    }
        
    bool endingWith(string s)
    {
        int l1 := self.Length;
        int l2 := s.Length;

        int n := self.indexOf(l1 - l2, s);
        
        ^ n != -1
    }
        
    bool containing(string s)
    {
        int l1 := self.Length;
        int l2 := s.Length;

        int n := self.indexOf(0, s);
        
        ^ n != -1
    }

    string replaceFirst(string replacee, string replacer)
    {
        int replaceeLen := replacee.Length;
        int i := self.indexOf(0, replacee);
        
        if (i != -1)
        { 
            ^ self.delete(i, replaceeLen).insert(i, replacee)
        }
        else
        {
            ^ self
        }    
    }    
    
    string replace(string replacee, string replacer)
    {
        string target := emptyString;
        int    start := 0;
        int    end := self.indexOf(start, replacee);
        int    replaceeLen := replacee.Length;
        
        while (end != -1)
        {
            target := target
                .add(self.Substring(start, end - start))
                .add(replacer);
            
            start := end + replaceeLen;
            
            end := self.indexOf(start, replacee)
        };
    
        if(start != 0)
        {
            ^ target.add(self.Substring(start))
        }
        else
        {
            ^ self
        }
    }
    
    bool isEmpty()
    {
        int length := self.Length;
        
        ^ length == 0
    }
}

// --- wideOp ---

public extension wideOp : WideString
{
    int Hashcode
    {
        get()
        {
            int len := self.Length;
        
            int h := extern elenart.core_utf16hashcode(self, len);
        
            ^ h
        }
    }
    
    indexOf(wide s)
        = self.indexOf(0, s);

    indexOf(char ch)
        = self.indexOf(0, ch);
                
    wide Substring(int index)
    {
        int length := self.Length;
        
        length := length - index;
        
        ^ self.Substring(index, length)
    }
     
    wide trimLeft(char ch)
    {
        int  index := 0;
        int  len := self.Length;
        char current := $0;
        int  chLen := ch.Length;
        
        while (index < len)
        {
            current := self[index];
            
            ifnot(current == ch)
                { ^ self.delete(0, index) };
            
            index += chLen
        };
        
        ^ emptyWideString
    }
    
    wide trimLeft()
        = self.trimLeft($32);
       
    wide trimRight(char ch)
    {
        int  len := self.Length;
        char current := $0;
        int  chLen := ch.Length;
                
        int  index := len - 1;
        int  prev := len;
        while (index >= 0)
        {
            current := self[index];
            
            ifnot (current == ch)
                { ^ self.delete(prev, len - prev) };
            
            prev := index;
            index -= chLen
        };
        
        ^ emptyWideString
    }
    
    wide trimRight()
        = self.trimRight($32);
       
    wide trim(char ch)
        = self.trimLeft(ch).trimRight(ch);
    
    wide trim()
        = self.trimLeft($32).trimRight($32);
    
    wide padLeft(char ch, int length)
    {
        int currentLength := self.Length;
        
        if (currentLength < length)
            { ^ WideString.fill(length - currentLength, ch).add(self) };
            
        ^ self
    }
    
    wide padRight(char ch, int length)
    {
        int currentLength := self.Length;

        if (currentLength < length)
            { ^ self.add(WideString.fill(length - currentLength, ch)) };
            
        ^ self
    }

    bool startingWith(wide s)
    {
        int n := self.indexOf(0, s);
        
        ^ 0 == n
    }
    
    bool endingWith(wide s)
    {
        int l1 := self.Length;
        int l2 := s.Length;
        int n := self.indexOf(l1 - l2, s);
        
        ^ n != -1
    }
    
    bool containing(wide s)
    {
        int l1 := self.Length;
        int l2 := s.Length;
        int n := self.indexOf(0, s);
        
        ^ n != -1
    }
    
    wide replaceFirst(wide replacee, wide replacer)
    {
        int replaceeLen := replacee.Length;
        int i := self.indexOf(0, replacee);
        
        if (i != -1)
        { 
            ^ self.delete(i, replaceeLen).insert(i, replacee)
        }
        else
        {
            ^ self
        }    
    }    
        
    replace(wide replacee, wide replacer)
    {
        wide target := emptyWideString;
        int  start := 0;
        int  end := self.indexOf(start, replacee);
        int  replaceeLen := replacee.Length;
        
        while (end != -1)
        {
            target := target
                .add(self.Substring(start, end - start))
                .add:replacer;
            
            start := end + replaceeLen;
            
            end := self.indexOf(start, replacee)
        };
    
        if (start != 0)
        { 
            ^ target.add(self.Substring(start)) 
        }
        else
        { 
            ^ self 
        }
    }

    bool isEmpty()
    {
        int length := self.Length;
        
        ^ length == 0
    }
}

public extension stringWeakOp
{
    fillString(s, int counter)
    {
        for (int i := 0, i < counter, i += 1)
        {
            self.write(s)
        };
            
        ^ self
    }

    writeCopies(line, counter)
        = self.fillString(line, cast int(counter));

    writePaddingLeft(line, char ch, int width)
    {
        var paddedStr := line.padLeft(ch, width);

        self.write(paddedStr);
            
        ^ self
    }
    
    writePaddingRight(line, char ch, int width)
    {
        var paddedStr := line.padRight(ch, width);

        self.write(paddedStr);
            
        ^ self
    }
    
    writePaddingLeft(line, int width)
    {
        var paddedStr := line.padLeft($32, width);

        self.write(paddedStr);
            
        ^ self
    }
    
    writePaddingRight(line, int width)
    {
        var paddedStr := line.padRight($32, width);

        self.write(paddedStr);
            
        ^ self
    }
    
    writePadding(line, char ch, int width)
    {
        int length := line.Length;        
        
        if (length < width)
        {
            int len := width - length;
            int leftLen := len / 2;
            
            self.fillString(ch, leftLen);
            self.write(line);
            self.fillString(ch, len - leftLen)
        }
        else
        { 
            self.write(line)
        };
            
        ^ self
    }    
}

// --- charOp ---

public extension charOp : CharValue
{
    int Hashcode
    {
        // Reads the object hash code into the output parameter
        get()
        {
            intern system'core_routines'__int.readHashCodeTo(self, ref int retVal);
            
            ^ retVal
        }
    }
    
    bool isWhitespace()
    {
        self =>
            $9  { ^ true }
            $32 { ^ true }
            $10 { ^ true }
            $13 { ^ true }
            $12 { ^ true };
            
        ^ false
    }
    
    bool isDigit()
    {
        int ch := intConvertor.convert(self);
        
        ^ (ch >= 30h) && (ch <= 39h)
    }
    
    bool isNewLine()
    {
        ^ self == $10
    }
    
    bool isLetter()
    {
        int ch := intConvertor.convert(self);
        
        if (ch >= 41h && ch <= 5Ah)
            { ^ true };
        
        if (ch >= 61h && ch <= 7Ah)
            { ^ true };
        
        if (ch >= 3ACh && ch <= 3CEh)
            { ^ true };
            
        if (ch >= 400h && ch <= 42Fh)
            { ^ true };
            
        ^ false
    }
    
    bool isLToken()
    {
        int ch := intConvertor.convert(self);
        
        if (ch >= 41h && ch <= 5Ah)
            { ^ true };
        
        if (ch >= 61h && ch <= 7Ah)
            { ^ true };
        
        if (ch >= 3ACh && ch <= 3CEh)
            { ^ true };
            
        if (ch >= 400h && ch <= 42Fh)
            { ^ true };
            
        ^ ch == 39
    }
}

// --- byteArrayHelper ---

public extension byteArrayHelper : Array<byte>
{
    [info:"Reads a short integer value at the specified position and saves it into an output variable"]
    read(int index, ref short retVal)
        => intern system'core_routines'__bytearray;

    /// Reads an integer value at the specified position and saves it into an output variable
    read(int index, ref int retVal)
        => intern system'core_routines'__bytearray;

    [info:"Reads a character value at the specified position and saves it into an output variable"]
    read(int index, ref char retVal)
        => intern system'core_routines'__bytearray;                
        
    move(int index, int length, int offset)
        => intern system'core_routines'__bytearray;
                
    /// Copies a sub array to the provided one at the specified position
    read(int index, int length, byte[] dump)
        => intern system'core_routines'__bytearray;
        
    /// Copies the byte array at the specified position
    write(int index, int length, byte[] dump)
        => intern system'core_routines'__bytearray;
        
    /// Copies the byte array at the specified position
    write(int index, short value)
        => intern system'core_routines'__bytearray;
        
    /// Copies the byte array at the specified position
    write(int index, int value)
        => intern system'core_routines'__bytearray;
        
    /// Copies the byte array at the specified position
    write(int index, char value)
        => intern system'core_routines'__bytearray;
        
    /// Copies the byte array at the specified position
    write(int index, long value)
        => intern system'core_routines'__bytearray;
        
    /// Copies the byte array at the specified position
    write(int index, real value)
        => intern system'core_routines'__bytearray;

    /// Fills the sub array with a specified value
    fill(int offset, int length, byte val)
    {
        int totalLength := self.Length;
        
        int i := offset;
        int m := offset + length;
        
        if (i > totalLength)
            { system'InvalidArgumentException.new().raise() };
        
        if (m > totalLength)
            { system'InvalidArgumentException.new().raise() };
            
        while(i < m)
        {
            self[i] :=  val;
            
            i := i + 1
        }
    }
}

// --- charArrayHelper ---

public extension charArrayHelper : Array<char>
{
    move(int index, int length, int offset)
        => intern system'core_routines'__chararray;

    [info:"Copies the array at the specified position"]    
    write(int index, int length, char[] array)
        => intern system'core_routines'__chararray;
}
