// based on msdn random sample

import extensions;

// --- program ---

const int rows = 4;
const int cols = 6;

const int runCount = 1000000;
const int distGroupCount = 20;

public program()
{
   auto intGroupSize := (1.0 + IntNumber.MaxValue) / distGroupCount;

   var intCounts := new int[](distGroupCount);
   var realCounts := new int[](distGroupCount);

   Console.printLine("Random reals:");
   for(int i := 0; i < rows; i += 1)
   {
      for(int j := 0; j <= cols; j += 1)
      {
         Console.printPaddingRight(12, randomGenerator.nextReal());
      };
        
      Console.writeLine()
   };

   Console.writeLine();
   Console.writeLine("Random integers:");
   for(int i := 0; i < rows; i += 1)
   {
      for(int j := 0; j <= cols; j += 1)
      {
         Console.printPaddingRight(12, randomGenerator.nextInt());
      };
        
      Console.writeLine()
   };

   Console.writeLine();
   Console.writeLine("Distribution");

   for(int i := 0; i < runCount; i += 1)
   {
      int index := (randomGenerator.nextInt(0, IntNumber.MaxValue) / intGroupSize).toInt();
      if(index < distGroupCount)
         { intCounts[index] := intCounts[index] + 1 };
        
      index := (randomGenerator.nextReal() * distGroupCount).toInt();
      if(index < distGroupCount)
         { realCounts[index] := realCounts[index] + 1 }
   };    

   for(int i := 0; i < distGroupCount; i += 1)
   {
      Console.printPaddingLeft(12, (i * intGroupSize).toInt(),"-");
      Console.printPaddingLeft(12, ((i + 1) * intGroupSize - 1).toInt());
      Console.printPaddingLeft(12, intCounts[i]);

      Console.printPaddingLeft(14, i.toReal() / distGroupCount,"-");
      Console.printPaddingLeft(12, (i + 1).toReal() / distGroupCount);
      Console.printPaddingLeft(12, realCounts[i]);
                
      Console.writeLine()
   }
}
