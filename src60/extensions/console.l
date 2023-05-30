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

//    printWide(params object[] list)
//    {
//        int len := list.Length;
//        for(int i := 0, i < len, i := i + 1)
//        {
//            object obj := list[i];
//            
//            self.write(obj.toWide())
//        }
//    }
//    
//    printWideLine(params object[] list)
//    {
//        int len := list.Length;
//        for(int i := 0, i < len, i := i + 1)
//        {
//            object obj := list[i];
//            
//            self.write(obj.toWide())
//        };
//        
//        self.writeLine();
//    }
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

public extension inputOp
{
   loadLineTo(var)
   {
      var line := self.readLine();
        
      line.convertTo:var;
        
      ^ var
   }
    
   loadLine(params object[] args)
   {
      string line := self.readLine();

      Array params := line.splitBy($32);

      int len := args.Length;
      for(int i := 0, i < len, i := i + 1)
      {
         params[i].convertTo(args[i]);
      };

      ^ self
   }
}
