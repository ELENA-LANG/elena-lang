import system'routines;
import extensions;

class GameMaster
{
   field _numbers;
   field _attempt;

   constructor()
   {
      // generate secret number
      var randomNumbers := new int[]{1,2,3,4,5,6,7,8,9}.randomize(9);

      _numbers := randomNumbers.Subarray(0, 4);
      _attempt := new Integer(1);
   }

   ask()
   {
      var row := Console.print("Your Guess #",_attempt," ?").readLine();
        
      ^ row.toArray()
   }

   proceed(guess)
   {
      int cows := 0;
      int bulls := 0;

      if (guess.Length != 4)
      {
         bulls := -1 
      }
      else
      {
         try
         {
            for (int i := 0; i < 4; i+=1) {
               var ch := guess[i];
               var number := ch.toString().toInt();
                    
               // check range
               if:not (number > 0 && number < 10)
                  { InvalidArgumentException.raise() };
                    
               // check duplicates
               var duplicate := guess.seekEach::(x => (x == ch)&&(x.equalReference(ch).Inverted));
               if (nil != duplicate)
               {
                  InvalidArgumentException.raise()
               };
                    
               if (number == _numbers[i])
               { 
                  bulls += 1 
               }
               else
               {
                  if (_numbers.ifExists(number))
                     { cows += 1 }
                  }
            }
         }
         catch(Exception e)
         {
            bulls := -1
         }
      };

      bulls =>
        -1 : { Console.printLine("Not a valid guess."); ^ true }
         4 : { Console.printLine("Congratulations! You have won!"); ^ false }
         ! : {
            _attempt.append(1);
                 
            Console.printLine("Your Score is ",bulls," bulls and ",cows," cows");
                 
            ^ true
         }
   }
}

public program()
{
   var gameMaster := new GameMaster();

   var process := $lazy gameMaster.proceed(gameMaster.ask());

   process.doWhile();
    
   Console.readChar()
}