{
   level : system'Integer.new,

   state0 :
   {
      digit : function(machine, ch, tree) 
              { 
                 tree.append(calc'TreeLeaf.new(
                                            {
                                               order : machine.level.add(3),
                              
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
                                               order : machine.level.add(1),
                              
                                               evalNode : function(a,b) { a.add(b) }
                                            })) ; 

                machine.state0
             },
  
      minus : function(machine,ch,tree) 
             { 
                tree.append(calc'TreeNode.new(
                                            {
                                               order : machine.level.add(1),
                              
                                               evalNode : function(a,b) { a.subtract(b) }
                                            })) ; 
                                            
                machine.state0
             },
  
      star : function(machine,ch,tree) 
             { 
                tree.append(calc'TreeNode.new(
                                            {
                                               order : machine.level.add(2),
                              
                                               evalNode : function(a,b) { a.multiply(b) }
                                            })) ; 

                machine.state0
             },
  
      slash : function(machine,ch,tree) 
             { 
                tree.append(calc'TreeNode.new(
                                            {
                                               order : machine.level.add(2),
                              
                                               evalNode : function(a,b) { a.divide(b) }
                                            })) ; 

                machine.state0
             }
   },

   start_state : function(machine) { machine.state0 }
}
