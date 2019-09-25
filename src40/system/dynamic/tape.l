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

singleton invoke0 { dispatch() => system'core_routines'__invoke0; }
singleton invoke1 { dispatch() => system'core_routines'__invoke1; }
singleton invoke2 { dispatch() => system'core_routines'__invoke2; }
singleton invoke3 { dispatch() => system'core_routines'__invoke3; }
singleton invoke4 { dispatch() => system'core_routines'__invoke4; }
singleton invoke5 { dispatch() => system'core_routines'__invoke5; }
singleton invoke6 { dispatch() => system'core_routines'__invoke6; }
singleton invoke7 { dispatch() => system'core_routines'__invoke7; }
singleton invoke8 { dispatch() => system'core_routines'__invoke8; }
singleton invoke9 { dispatch() => system'core_routines'__invoke9; }
singleton invoke10 { dispatch() => system'core_routines'__invoke10; }
singleton invoke11 { dispatch() => system'core_routines'__invoke11; }
singleton invoke12 { dispatch() => system'core_routines'__invoke12; }
singleton invoke13 { dispatch() => system'core_routines'__invoke13; }
singleton invoke14 { dispatch() => system'core_routines'__invoke14; }
singleton invoke15 { dispatch() => system'core_routines'__invoke15; }
singleton invoke16 { dispatch() => system'core_routines'__invoke16; }
singleton invoke17 { dispatch() => system'core_routines'__invoke17; }
singleton invoke18 { dispatch() => system'core_routines'__invoke18; }
singleton invoke19 { dispatch() => system'core_routines'__invoke19; }
singleton invoke20 { dispatch() => system'core_routines'__invoke20; }
singleton invoke21 { dispatch() => system'core_routines'__invoke21; }
singleton invoke22 { dispatch() => system'core_routines'__invoke22; }
singleton invoke23 { dispatch() => system'core_routines'__invoke23; }
singleton invoke24 { dispatch() => system'core_routines'__invoke24; }
singleton invoke25 { dispatch() => system'core_routines'__invoke25; }
singleton invoke26 { dispatch() => system'core_routines'__invoke26; }
singleton invoke27 { dispatch() => system'core_routines'__invoke27; }
singleton invoke28 { dispatch() => system'core_routines'__invoke28; }
singleton invoke29 { dispatch() => system'core_routines'__invoke29; }
singleton invoke30 { dispatch() => system'core_routines'__invoke30; }
singleton invoke31 { dispatch() => system'core_routines'__invoke31; }
singleton invokeV1 { }
singleton loadString { dispatch() => system'core_routines'__loadString; }
singleton freeVArg {}

const commands = new::(
    invoke0,invoke1,invoke2,invoke3,invoke4,invoke5,invoke6,invoke7,invoke8,invoke9,
    invoke10,invoke11,invoke12,invoke13,invoke14,invoke15,invoke16,invoke17,invoke18,invoke19,
    invoke20,invoke21,invoke22,invoke23,invoke24,invoke25,invoke26,invoke27,invoke28,invoke29,
    invoke30,invoke31,invokeV1,loadString,freeVArg);

public singleton StreamTapeInterpreter
{
    build(byte[] buffer, int len)
        => system'core_routines'__tape;
//    {
//        int i := 0;
//        byte command := 0;
//        int size := 0;
//        while (i < len) {
//            command := buffer[i];
//            i += 4;
//            
//            size := buffer[i];
//            i += 4;
//            
//            commands[i](size,buffer,i);
//            
//            i += size
//        }            
//    }
}
