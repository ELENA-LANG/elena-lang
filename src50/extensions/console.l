import system'routines;
import system'text;

// --- outputOp ---

public extension outputOp
{
    print(params object[] list)
    {
        int len := list.Length;
        for(int i := 0, i < len, i := i + 1)
        {
            object obj := list[i];
            
            self.write(obj.toString())
        }
    }
            
    printLine(params object[] list)
    {
        int len := list.Length;
        for(int i := 0, i < len, i := i + 1)
        {
            object obj := list[i];
            
            self.write(obj.toString())
        };        
        
        self.writeLine();
    }    
    
    printWide(params object[] list)
    {
        int len := list.Length;
        for(int i := 0, i < len, i := i + 1)
        {
            object obj := list[i];
            
            self.write(obj.toWide())
        }
    }
    
    printWideLine(params object[] list)
    {
        int len := list.Length;
        for(int i := 0, i < len, i := i + 1)
        {
            object obj := list[i];
            
            self.write(obj.toWide())
        };
        
        self.writeLine();
    }
}

public extension outputFormatterOp
{
    printPaddingRight(int width, params object[] list)
    {
        auto buffer := new TextBuilder();
        
        int len := list.Length;
        for (int i := 0, i < len, i += 1)
        {
            object obj := list[i];
            
            buffer.write(obj.toString())
        };
        
        self.writePaddingRight(buffer.Value, $32, width);
            
        ^ self
    }

    printPaddingLeft(int width, params object[] list)
    {
        auto buffer := new TextBuilder();
        
        int len := list.Length;
        for (int i := 0, i < len, i += 1)
        {
            object obj := list[i];
            
            buffer.write(obj.toString())
        };
        
        self.writePaddingLeft(buffer.Value, $32, width);
            
        ^ self
    }
}

public extension outputCharFormatterOp
{
    printPaddingRightChar(int width, char ch, params object[] list)
    {
        auto buffer := new TextBuilder();
        
        int len := list.Length;

        for(int i := 0, i < len, i += 1)
        {
            object obj := list[i];
            
            buffer.write(obj.toString())
        };
        
        self.writePaddingRight(buffer.Value, ch, width)
    }

    printPaddingLeftChar(int width, char ch, params object[] list)
    {
        auto buffer := new TextBuilder();
        
        int len := list.Length;

        for(int i := 0, i < len, i += 1)
        {
            object obj := list[i];
            
            buffer.write(obj.toString())
        };
        
        self.writePaddingLeft(buffer.Value, ch, width)
    }
}

//// --- formatterOp ---
//
//singleton helper
//{
//    readIndex(string s, ref int i, ref int retVal)
//    {
//        char ch := $0;
//        int  n := 0;
//        int  v := 0;
//        
//        s.read(i, ref ch);
//        while (ch != $125)
//        {
//            n := intConvertor.convert(ch);
//            if(n >= 30h && n <= 39h)
//            {              
//                n := n - 30h;
//                
//                v := v * 10;
//                v := v + n
//            }
//            else
//            { 
//                InvalidArgumentException.raise()
//            };
//              
//            i := i + 1;  
//            s.read(i, ref ch)
//        };
//        
//        retVal := v
//    }
//}
//
//public extension formatterOp
//{
//    printFormatted(string s, params object[] args)
//    {
//        auto buffer := new TextBuilder();
//        
//        int  len := s.Length;
//        int  i := 0;
//        char ch := $0;
//        int  chlen := 0;
//        int  index := 0;
//        
//        while (i < len)
//        {
//            s.read(i, ref ch);
//            
//            if (ch == $123)
//            {
//                i := i + 1;
//                
//                helper.readIndex(s, ref i, ref index);
//                
//                var arg := args[index];
//                buffer.write(arg.toString());
//                
//                chlen := 1
//            }
//            else
//            { 
//                buffer.write(ch); 
//                chlen := ch.Length;
//            };
//            
//            i := i + chlen
//        };
//        
//        self.write(buffer.Value);
//        
//        ^ self
//    }
//    
//    printLineFormatted(string s, params object[] args)
//    {
//        self.printFormatted(s, params args);
//        self.writeLine();
//    }
//}

// --- inputOp ---

public extension inputOp
{
    loadLineTo(var)
    {
        var line := self.readLine();
        
        line.convertTo:var;
        
        ^ var
    }
    
//    loadLine(params object[] args)
//    {
//        string line := self.readLine();
//        
//        Array params := line.splitBy($32);
//        
//        int len := args.Length;
//        for(int i := 0, i < len, i := i + 1)
//        {
//            params[i].convertTo(args[i]);
//        };
//        
//        ^ self
//    }
}
