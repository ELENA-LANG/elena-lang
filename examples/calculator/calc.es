var numberstrategy =
{
   order : 0,

   evalToken : function(ch) { return system'realConvertor.convert(ch); }
};

var sumstrategy =
{
   order : 2,

   evalNode : function(a,b) { return a.add(b); }
};

var state0 = system'Variable new.
var state1 = system'Variable new.

state0.write(
{
   digit : function(ch,tree) { tree.append(calc'TreeLeaf.new(numberstrategy).append(ch)); return state1; }
});

state1.write(
{
   digit : function(ch,tree) { tree.last_node.append(ch)); return state1; },
  
   plus : function(ch,tree) { tree.append(calc'TreeNode.new(sumstrategy) ; return state0; )
});

return state0;
