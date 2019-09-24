import system'collections;
import system'io;

// --- ClosureTape ---

public mixin ClosureTape
{
    __string DynamicClosure[] theList;
    
    internal constructor allocate(int len)
        = new DynamicClosure[](len);

    constructor(params DynamicClosure[] args)
        <= new(params args);
            
    constructor new(params DynamicClosure[] args)
        <= allocate(args.Length)
    {
        self.setFields(params args)
    }

    constructor new (List<DynamicClosure> list)        
        <= allocate(list.Length)
    {
        int i := list.Length;
        while (i > 0)
        {
            i -= 1;
            
            theList[i] := list[i]
        }
    }
        
//    constructor new (List<DynamicClosure> list, int index, int length)        
//        <= new(length);
//    [
//        int i := index.
//        int c := 0.
//        while (c < length)
//        [
//            theList[c] := list[i].
//
//            c += 1.
//            i += 1.
//        ]
//    ]
        
    constructor new(List<object> list, int index, int length)        
        <= allocate(length)
    {
        int i := index;
        int c := 0;
        while (c < length)
        {
            theList[c] := list[i];

            c += 1;
            i += 1
        }
    }
        
    dispatch() => system'core_routines'__closuretape;
}

// --- Class ---

public mixin ClosureClass
{
    __string object[] items;
    
    internal constructor allocate(int n)
        = new object[](n);
    
//    constructor new(Array list)
//        <= newInternal(list length);
//    [
//        int i := 0.
//        int n := list length.
//        
//        while (i < n)
//        [
//            self setField(i, list getAt(i)).
//            
//            i := i + 1
//        ]
//    ]    
//    
//    constructor new(ArrayList list, int index, int length)
//        <= newInternal(length);
//    [
//        int i := 0.
//        while(i < length)
//        [
//            self setField(i,list getAt(index + i)).
//            
//            i := i + 1
//        ]
//    ]
    
    constructor new(params object[] args)
        <= allocate(args.Length)
    {
        int length := args.Length;
        for (int i := 0, i < length, i += 1)
        {
            items[i]:= args[i]
        }
    }
    
    constructor new(List<object> list, int index, int length)
        <= allocate(length)
    {
        for (int i := 0, i < length, i += 1)
        {
            int n := i + index;
            
            items[i]:= list.at(n)
        }
    }
    
    constructor create(List<Tuple<Message,ClosureTape>> methods)
        <= allocate(methods.Length * 2)
    {
        int length := methods.Length;
        for (int i := 0, i < length, i += 1)
        {
            var t := methods[i];
            
            var o1 := t.Item1;
            var o2 := t.Item2;

            items[i*2]:= o1;
            
            int k := i*2 + 1;
            items[k]:= o2;
                        
//            items[i*2]:= methods[i].Item1;
//            items[i*2 + 1]:= methods[i].Item2;
        }
    }
    
    dispatch() => system'core_routines'__closureclass;
}

// --- StreamTapeInterpreter ---


public singleton StreamTapeInterpreter
{
    build(byte[] buffer, int start, int end)
        => system'core_routines'__tape;
//    {
//        int size := 0;
//        int i := start;
//        while(i < end) {
//            byte command := buffer[i];
//            i += 1;
//            buffer.read(i, ref size);
//            i += 4;
//            if (command == 0FFh) {
//                UTF8Encoding.toString(i,size,buffer)
//            }
//            else {
//                extern elenart.LoadSymbolByBuffer(buffer, i, size)
//            };        
//            
//            i += size
//        }
//    }    
}
