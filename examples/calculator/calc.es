{
   state0 :
   {
      digit : function(machine, ch, tree) 
              { 
                 tree.append(calc'TreeLeaf.new(
                                            {
                                               order : 0,

                                               evalToken : function(ch) { system'realConvertor.convert(ch) }
                                            })
                                          .append(ch)); 
                 machine.state1
              }
   },

   state1 : 
   {
      digit : function(machine,ch,tree) { tree.last_node.append(ch); machine.state1 },
  
      plus : function(machine,ch,tree) 
             { 
                tree.append(calc'TreeNode.new(
                                            {
                                               order : 2,

                                               evalNode : function(a,b) { a.add(b) }
                                            })) ; 

                machine.state0
             }
   },

   start_state : function(machine) { machine.state0 }
}
