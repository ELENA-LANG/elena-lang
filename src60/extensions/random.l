import system'routines;

struct Seed
{
   int z1;
   int z2;
   int z3;
   int z4;
}

public sealed struct RandomGenerator
{
   Seed _seed;
    
   internal constructor new()
   {
      extern GetRandomSeedLA(_seed);
   }

   private int eval(int maxValue)
   {
      int number := self.eval().Absolute;

      ^ 0 + number.mod(maxValue);
   }

   private int eval()
   {
      int number := extern GetRandomIntLA(_seed);

      ^ number;
   }

   int nextInt(minValue, maxValue)
        = minValue + self.eval(maxValue - minValue);
    
   int nextInt()
   {
      int nextValue := self.eval();
        
      ^ new IntNumber(nextValue)
   }

   int nextInt(int maxValue)
      = self.eval(maxValue);
        
   real nextReal()
   {
      int next := self.eval(1000000);
        
      real retVal := RealConvertor.convert(next);
        
      retVal := retVal / 1000000.0r;
        
      ^ new RealNumber(retVal)
   }
}

// --- randomGenerator ---
public static RandomGenerator randomGenerator = RandomGenerator.new();

// -- ERandomizer --

public extension randomOp
{
   randomize(int count)
   {
      var list := self.clone();
        
      int n := cast int(count);
      int i := 0;
        
      int max := self.Length;
        
      while (i < n)
      {
         var j := randomGenerator.nextInt(max);
            
         list.exchange(i, j);
            
         i := i + 1
      };
        
      ^ list
   }
}
