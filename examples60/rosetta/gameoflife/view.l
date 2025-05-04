import cellular;
import system'text;

public extension presenterOp : Space
{
   print()
   {
      Console.setCursorPosition(0, 0);
     
      int columns := self.Columns;
      int rows := self.Rows;
     
      auto line := new TextBuilder();
      for(int i := 0; i < rows; i += 1)
      {
         line.clear();
         for(int j := 0; j < columns; j += 1)
         {
            int cell := self.at(i, j);
             
            line.write((cell == 0).iif(" ","o"));
         };
         
         Console.writeLine(line.Value)
      }
   }
}