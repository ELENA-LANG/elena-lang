import cellular;

const int maxX = 48;
const int maxY = 28;

sealed class Model
{
   Space   _space;
   RuleSet _ruleSet;
   bool    _started;

   Func<Space, object> OnUpdate : event;
    
   constructor newRandomset(RuleSet transformSet)
   {
      _space := IntMatrixSpace.allocate(maxY, maxX, randomSet);

      _ruleSet := transformSet;
        
      _started := false
   }
    
//   constructor newLoaded(RuleSet initSet, RuleSet transformSet)
//   {
//      _space := IntMatrixSpace.allocate(maxY, maxX, initSet);
//                        
//      _ruleSet := transformSet;
//        
//      _started := false
//   }
    
   private onUpdate()
   {
      OnUpdate?.(_space)
   }
    
   run()
   {
      if (_started)
      { 
         _space.update(_ruleSet) 
      }
      else
      {
         _started := true
      };
        
      self.onUpdate()
   }
}
