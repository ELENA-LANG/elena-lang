var numberstrategy =
{
   order : 0,

   evalToken : function(ch) { system'realConvertor.convert(ch) }
};

var sumstrategy =
{
   order : 2,

   evalNode : function(a,b) { a.add(b) }
};

var state0 = system'Variable new.
var state1 = system'Variable new.

state0.write(
{
   digit : function(ch,tree) { tree.append(calc'TreeLeaf.new(numberstrategy).append(ch)); state1 }
});

state1.write(
{
   digit : function(ch,tree) { tree.last_node.append(ch)); state1 },
  
   plus : function(ch,tree) { tree.append(calc'TreeNode.new(sumstrategy) ; state0 )
});

state0
