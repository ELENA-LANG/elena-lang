/// a program which parses and evaluates arithmetic expressions. 

import system'routines;
import extensions;
import extensions'text;

// --- Token ---

class Token
{
   object _value;
    
   int Level : rprop;
    
   constructor new(int level)
   {
      _value := new StringWriter();
      Level := level + 9;
   }
    
   append(ch)
   {
      _value.write(ch)
   }
    
   Number = _value.toReal();
}

// --- Node ---

class Node
{
   object Left  : prop;
   object Right : prop;
   int Level    : rprop;

   constructor new(int level)
   {
      Level := level
   }
}

// --- SummaryNode

class SummaryNode : Node
{
   constructor new(int level)
      <= super new(level + 1);
    
   Number = Left.Number + Right.Number;
}

// --- DifferenceNode ---

class DifferenceNode : Node
{
   constructor new(int level)
      <= super new(level + 1);
    
   Number = Left.Number - Right.Number;
}

// --- ProductNode ---

class ProductNode : Node
{
   constructor new(int level)
      <= super new(level + 2);
    
   Number = Left.Number * Right.Number;
}

// --- FractionNode ---

class FractionNode : Node
{
   constructor new(int level)
      <= super new(level + 2);
    
   Number = Left.Number / Right.Number;
}

// --- Expression ---

class Expression
{
   int Level  :rprop;
   object Top :prop;
    
   constructor new(int level)
   {
      Level := level
   }
    
   object Right
   {
      get() = Top;
        
      set(object node)
      {
         Top := node
      }
   }
    
   get Number() => Top;
}

// --- States ---

singleton operatorState
{
   eval(ch)
   {
      ch =>
         $40 : {      // (
            ^ weak self.newBracket().gotoStarting()  
         }
         ! : {
            ^ weak self.newToken().append(ch).gotoToken()
         }
   }
}

singleton tokenState
{
   eval(ch)
   {
      ch =>
         $41 : {      // )
            ^ weak self.closeBracket().gotoToken()
         }
         $42 : {      // *
            ^ weak self.newProduct().gotoOperator()
         }
         $43 : {      // +
            ^ weak self.newSummary().gotoOperator()
         }
         $45 : {      // -
            ^ weak self.newDifference().gotoOperator()
         }
         $47 : {      // /
            ^ weak self.newFraction().gotoOperator()
         }
         ! : {
            ^ weak self.append(ch)
         }
   }
}    

singleton startState
{
   eval(ch)
   {
      ch =>
         $40 : {      // (
            ^ weak self.newBracket().gotoStarting()
         }
         $45 : { // -
            ^ weak self.newToken().append("0").newDifference().gotoOperator()
         }
         ! : {
            ^ weak self.newToken().append(ch).gotoToken()
         }
   }
}    

// --- Scope ---

class Scope
{
   object _state;
   int    _level;
   object _parser;
   object _token;
   object _expression;
    
   constructor new(parser)
   {
      _state := startState;
      _level := 0;
      _expression := Expression.new(0);
      _parser := parser
   }
                                       
   newToken()
   {
      _token := _parser.appendToken(_expression, _level)
   }                                                
    
   newSummary()
   {
      _token := nil;
        
      _parser.appendSummary(_expression, _level)
   }                                                
    
   newDifference()
   {
      _token := nil;
        
      _parser.appendDifference(_expression, _level)
   }                                                
      
   newProduct()
   {
      _token := nil;
        
      _parser.appendProduct(_expression, _level)
   }                                                
    
   newFraction()
   {
      _token := nil;
        
      _parser.appendFraction(_expression, _level)
   }                                                

   newBracket()
   {
      _token := nil;
        
      _level := _level + 10;
        
      _parser.appendSubexpression(_expression, _level)
   }            

   closeBracket()
   {
      if (_level < 10)
         { InvalidArgumentException.new("Invalid expression").raise() };
        
      _level := _level - 10
   }            
                                    
   append(ch)
   {
      if(ch >= $48 && ch < $58)
      { 
         _token.append(ch)
      }
      else
      { 
         InvalidArgumentException.new("Invalid expression").raise()
      }
   }
            
   append(string s)
   {
      s.forEach::(ch){ self.append(ch) }
   }
                                                         
   gotoStarting()
   {
      _state := startState
   }
                                                                  
   gotoToken()
   {
      _state := tokenState
   }
                                                                  
   gotoOperator()
   {
      _state := operatorState
   }
    
   get Number() => _expression;
    
   dispatch() => _state;
}

// ---- Parser ----

class Parser
{
   appendToken(object expression, int level)
   {
      var token := Token.new(level);
        
      expression.Top := self.append(expression.Top, token);
        
      ^ token
   }

   appendSummary(object expression, int level)
   {
      var t := expression.Top;

      expression.Top := self.append(/*expression.Top*/t, SummaryNode.new(level))
   }

   appendDifference(object expression, int level)
   {
      expression.Top := self.append(expression.Top, DifferenceNode.new(level))
   }

   appendProduct(object expression, int level)
   {
      expression.Top := self.append(expression.Top, ProductNode.new(level))
   }

   appendFraction(object expression, int level)
   {
      expression.Top := self.append(expression.Top, FractionNode.new(level))
   }

   appendSubexpression(object expression, int level)
   {
      expression.Top := self.append(expression.Top, Expression.new(level))
   }

   append(object lastNode, object newNode)
   {
      if(nil == lastNode)
         { ^ newNode };
        
      if (newNode.Level <= lastNode.Level)
         { newNode.Left := lastNode; ^ newNode };
            
      var parent := lastNode;
      var current := lastNode.Right;
      while (nil != current && newNode.Level > current.Level)
         { parent := current; current := current.Right };
        
      if (nil == current)
      { 
         parent.Right := newNode 
      }
      else
      { 
         newNode.Left := current; parent.Right := newNode 
      };
            
      ^ lastNode
   }
    
   run(text)
   {
      var scope := Scope.new(self);

      text.forEach::(ch){ scope.eval(ch) };

      ^ scope.Number
   }
}

public program()
{
   var text := new StringWriter();
   var parser := new Parser();

   while (Console.readLine().writeTo(text).Length > 0)
   {
      try
      {
         Console.printLine("=",parser.run(text))
      }
      catch(Exception e)
      {
         Console.writeLine("Invalid Expression")
      };
        
      text.clear()
   }
}