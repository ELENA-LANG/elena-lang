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
