import system'routines;
import system'collections;
import extensions'text;
import extensions;

extension lexicalOp
{
   get LexicalType()
   {
      if (self >= $48 && self <= $57)
         { ^ mssg digit };

      if (self >= $97 && self <= $122)
         { ^ mssg letter };

      self =>
         $32 : { ^ mssg whitespace }
         $40 : { ^ mssg opening }
         $41 : { ^ mssg closing }
         $42 : { ^ mssg star }
         $43 : { ^ mssg plus }
         $44 : { ^ mssg comma }
         $45 : { ^ mssg minus }
         $46 : { ^ mssg dot }
         $47 : { ^ mssg slash }
         ! : { ^ mssg unknown }
   }
}


class DerivationTree
{
   _root;
   _last;

   get Value() => _root;

   append(node)
   {
      _last := node;

      _root := _root + node;
   }

   newArgument()
   {
      _last := _root.newArgument()
   }

   constructor()
   {
      _root := new BaseTreeNode{ this Level := -1; add(node) = node; }
   }
}

// --- BaseTreeNode ---

class BaseTreeNode
{
   _function;
    
   int Level : prop;
    
   add(node)
   {
      if (self.Level < node.Level)
      {
         self.append(node);
            
         ^ self
      }
      else
      {
         node.append(self);
            
         ^ node
      }
   }

   constructor new(level,function)
   {
      Level := level;
      _function := function
   }
    
   constructor new(level)
   {
      Level := level
   }
}

// --- TreeNode ---

class TreeNode : BaseTreeNode
{
   _left;
   _right;

   newArgument()
   {
      if (nil == _right)
      {
         ^ _right.newArgument()
      }
      else
      {
         ^ _left.newArgument()
      }
   }

   append(node)
   {
      if (nil == _left)
      { 
         _left := node 
      }
      else
      {
         if (nil == _right)
         { 
            _right := node 
         }
         else
         { 
            _right := _right + node 
         }
      }
   }
    
   get Value()
   {
      if (nil == _left)
         { InvalidArgumentException.raise() };
        
      if (nil == _right)
         { InvalidArgumentException.raise() };
        
      ^ _function(_left.Value, _right.Value)
   }
    
   constructor new(baseLevel, function)
      <= super new(baseLevel, function);
}

// --- TreeLeaf ---

class TreeLeaf : BaseTreeNode
{
   _token;

   appendContent(ch)
   {
      _token.append(ch)
   }

   Value = _function(_token.Value);
            
   constructor new(level, function)
      <= super new(level,function)
   {
      _token := new StringWriter();
   }
    
   constructor new(level)
      <= super new(level)
   {
      _token := new StringWriter();
   }
}

//// --- FunctionNode ---
//
//class FunctionNode : TreeLeaf
//{
//   List _argNodes;
//    
//   newArgument()
//   {
//      _argNodes.append(nil);
//        
//      ^ self
//   }
//    
//   append(node)
//   {
//      int last := _argNodes.Length - 1;
//          
//      var lastNode := _argNodes[last];
//      if (lastNode == nil)
//      {
//         _argNodes[last] := node
//      }
//      else
//      {
//         _argNodes[last] := _argNodes[last] + node
//      }
//   }
//    
//   get Value()
//   {
//      object[] args := new object[](_argNodes.Length);
//      for (int i := 0, i < args.Length, i+=1) {
//            args[i] := _argNodes[i].Value
//      };
//        
//      var symbol := new Symbol(_token.Value);
//        
//      ^ symbol(inlinearg args);
//   }
//        
//   constructor new(baseLevel)
//      <= super new(baseLevel)
//   {
//      _argNodes := new List();
//      _argNodes.append(nil);
//        
//      _token.append("extensions'math'");
//   }
//}

// --- StateMachine ---

class StateMachine
{
   _states;
   _currentState;
   _derivationTree;
   _leaf;
   _function;
    
   int _level;

   private init()
   {
      _derivationTree := new DerivationTree();
      _currentState := _states.State0;
        
      _level := 0
   }

   constructor assign(states)
   {
      _states := states;
        
      self.init()
   }

   get generic()
   {
      MessageName mssg := __received.MessageName;
      Message prop := mssg.getPropertyMessage();
        
      _currentState := prop(_states);
   }

   incLevel(diff)
   {
      _level := _level + diff
   }

   decLevel(diff)
   {
      _level := _level - diff
   }

   newArgument()
   {
      _derivationTree.newArgument()
   }

   invalidate()
   {
      _derivationTree := nil
   }

   get Value()
   {
      var value := nil;
        
      if (nil == _derivationTree)
      { 
         value := "Invalid expression" 
      }
      else
      {
         value := _derivationTree.Value
            //\ back("Invalid expression")
      };
            
      ^ value
   }

   newNumberLeaf(leafLevel)
   {
      _leaf := TreeLeaf.new(_level + leafLevel, mssgconst toReal<realConvertOp>[1]);
        
      _derivationTree.append(_leaf)
   }
//    
//   newFunctionLeaf(leafLevel)
//   {
//      _leaf := FunctionNode.new(_level + leafLevel);
//        
//      _derivationTree.append(_leaf)
//   }
    
   appendContent(ch)
   {
      _leaf.appendContent(ch)
   }

   newNode(nodeLevel, operator)
   {
      var function;
      operator =>
         "+" : { function := mssgconst add[2]}
         "-" : { function := mssgconst subtract[2]}        
         "*" : { function := mssgconst multiply[2]}
         "/" : { function := mssgconst divide[2]};
            
      _derivationTree.append(TreeNode.new(_level + nodeLevel, function));
   }

   dispatch() => _currentState;
}

// --- parserOp ---

extension parserOp
{
   evaluatedWith(states)
   {
      var stateMachine := StateMachine.assign(states);
      try
      {
         self.forEach::(ch)
         {
            var type := ch.LexicalType;
        
            type(stateMachine, ch)
         }
      }
      catch(Exception e)
      {
         Console.printLine(e);
            
         stateMachine.invalidate()
      };
        
      ^ stateMachine.Value
   }
}
