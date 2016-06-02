{
   state0 :
   {
      digit : function(machine, ch, tree) 
              { 
                 machine.state1
              }
   },

   state1 : 
   {
      digit : function(machine,ch,tree) { machine.state1 },
   },

   start_state : function(machine) { machine.state0 }
}
