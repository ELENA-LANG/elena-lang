{
   level : system'Integer.new,

   state0 :
   {
      digit : function(machine, ch, tree) 
              { 
                 tree.append(calc'TreeLeaf.new(
                                            {
                                               order : machine.level.add(3)
                                            }))
              }
   }
}
