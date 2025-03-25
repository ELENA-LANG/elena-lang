import system'threading;
import cellular;

const int DELAY = 50;

singleton gameOfLifeRuleSet : RuleSet
{
   int proceed(Space s, int x, int y)
   {
      int cell := s.at(x, y);
      int number := s.LiveCell(x, y, 1); // NOTE : number of living cells around the self includes the cell itself
      
      if (cell == 0 && number == 3)
      {
         ^ 1 
      }
      else if (cell == 1 && (number == 4 || number == 3))
      {
         ^ 1 
      }
      else
      {
         ^ 0
      }
   }
}

playGame(Model model)
{
   Console.clear();
    
   model.OnUpdate := (Space sp){ sp.print() };

   until (Console.KeyAvailable)
   {
      model.run();
        
      threadControl.sleep(DELAY)
   };

   Console.readChar()
}

//public test
//[
//    playGame(Model newLoaded(LoadedSet load(testSet), gameOfLifeRuleSet)).
//]

public program()
{
    playGame(Model.newRandomset(gameOfLifeRuleSet))
}