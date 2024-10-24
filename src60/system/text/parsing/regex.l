import system;
import system'collections;

const string Reserved = "$^[]+*-?";
const string Postfix = "+*?";

internal interface PatternRule
{
   indexed internal abstract setNext(PatternRule rule);

   abstract PatternRule? gotoNext(Stack<PatternRule> parents);
   
   abstract PatternRule? makeStep(string s, int index, char ch, Stack<PatternRule> parents);

   abstract bool isMatched(Stack<PatternRule> parents);
}

internal abstract LinkRule : PatternRule
{
   PatternRule _nextRule;

   bool isMatched(Stack<PatternRule> parents)
      = false;

   indexed internal setNext(PatternRule rule)
   {
      _nextRule := rule
   }

   PatternRule? gotoNext(Stack<PatternRule> parents)
   {
      if:nil(_nextRule) {
         ^ parents.pop().gotoNext(parents);
      };

      ^ _nextRule;
   }
}

internal abstract GroupRule : LinkRule
{
   PatternRule[] _rules;
}

internal NestedRule : LinkRule
{
   PatternRule _child;

   constructor new(PatternRule child)
   {
      _child := child
   }

   PatternRule? makeStep(string s, int index, char ch, Stack<PatternRule> parents)
   {
      parents.push(self);
      PatternRule next := _child.makeStep(s, index, ch, parents);
      if:nil(next) {
         parents.pop();
      };

      ^ next
   }
}

internal sealed OptionalRule : NestedRule
{
   constructor new(PatternRule child)
   {
      _child := child
   }

   PatternRule? makeStep(string s, int index, char ch, Stack<PatternRule> parents)
   {
      parents.push(self);
      PatternRule next := _child.makeStep(s, index, ch, parents);
      if:nil(next) {
         parents.pop();

         ^ gotoNext(parents).makeStep(s, index, ch, parents)
      };

      ^ next
   }
}

internal sealed CharRule : LinkRule
{
   char _ch;

   constructor new(char ch)
   {
      _ch := ch;
   }

   PatternRule? makeStep(string s, int index, char ch, Stack<PatternRule> parents)
   {
      if (_ch == ch) {
         ^ gotoNext(parents);
      };

      ^ nil;
   }
}

internal sealed CharRangeRule : LinkRule
{
   char _chFrom;
   char _chTill;

   constructor new(char from, char till)
   {
      _chFrom := from;
      _chTill := till;
   }

   PatternRule? makeStep(string s, int index, char ch, Stack<PatternRule> parents)
   {
      if (ch >= _chFrom && ch <= _chTill) {
         ^ gotoNext(parents);
      };

      ^ nil;
   }
}

internal sealed OrRule  : GroupRule
{
   constructor load(PatternRule[] rules)
   {
      _rules := rules
   }

   PatternRule? makeStep(string s, int index, char ch, Stack<PatternRule> parents)
   {
      parents.push(self);

      int len := _rules.Length;
      for (int i := 0; i < len; i++) {
         PatternRule next := _rules[i].makeStep(s, index, ch, parents);
         if:not:nil(next) {
            ^ next;
         };
      };
      
      parents.pop();
      
      ^ nil
   }
}

internal sealed RecursiveRule : NestedRule
{
   constructor new(PatternRule child)
   {
      _child := child
   }

   bool isMatched(Stack<PatternRule> parents)
      = super.gotoNext(parents).isMatched(parents);

   PatternRule? gotoNext(Stack<PatternRule> parents)
   {
      ^ self
   }

   PatternRule? makeStep(string s, int index, char ch, Stack<PatternRule> parents)
   {
      parents.push(self);

      PatternRule next := _child.makeStep(s, index, ch, parents);
      if:nil(next) {
         parents.pop();

         ^ super.gotoNext(parents).makeStep(s, index, ch, parents);
      };

      ^ next
   }
}

internal sealed singleton EOLRule : PatternRule
{
   indexed internal setNext(PatternRule rule)
   {
      InvalidOperationException.raise("$ must be the last one in the pattern");
   }

   PatternRule? makeStep(string s, int index, char ch, Stack<PatternRule> parents)
      = nil;

   PatternRule? gotoNext(Stack<PatternRule> parents)
      = nil;

   bool isMatched(Stack<PatternRule> parents)
      = true;
}

public sealed class RegEx
{
   PatternRule _startRule;

   constructor(string pattern)
      = RegEx.prepare(pattern);

   private bool isReserved(char ch)
   {
      ^ Reserved.indexOf(ch) != -1
   }

   private bool isPostfix(char ch)
   {
      ^ Postfix.indexOf(ch) != -1
   }

   private PatternRule? defineRule(char ch)
   {
      ch =>
         $36: { ^ EOLRule };

      if (isReserved(ch))
         { ^ nil };

      ^ CharRule.new(ch)
   }

   private PatternRule parseGroup(string pattern, ref int index, int len)
   {
      int i := index + 1;

      List<PatternRule> orList := new List<PatternRule>();
      while (i < len) {
         char ch := pattern[i];
         int chLen := ch.Length;
         ch =>
            $93 : { i++; :break; }
              ! : {
                  PatternRule rule := nil;
                  if (pattern[i + chLen] == $45) {
                     i += chLen;
                     i++;

                     char lastCh := pattern[i];
                     chLen := lastCh.Length;

                     rule := CharRangeRule.new(ch, lastCh);
                  }
                  else rule := defineRule(ch);

                  if (rule != nil) {
                     orList.append(rule)
                  }
                  else InvalidArgumentException.raise("pattern")
               };

         i += chLen
      };

      index := i;

      if (orList.Length == 1) {
         ^ orList.First
      };

      ^ OrRule.load(orList.Value)
   }

   private PatternRule parsePattern(string pattern)
   {
      int len := pattern.Length;
      int i := 0;

      if (pattern[0] == $94) {
         i++;
      }
      else InvalidArgumentException.raise("pattern"); // !! temporally

      List<PatternRule> list := new List<PatternRule>();
      while (i < len) {
         char ch := pattern[i];

         if (isPostfix(ch)) {
            if(list.Empty)
               InvalidArgumentException.raise("pattern");

            PatternRule rule := nil;
            ch =>
               $63: { rule := OptionalRule.new(list.Last) }
               $42: { rule := RecursiveRule.new(list.Last) }
               $43: {
                  PatternRule last := list.Last;
                  rule := RecursiveRule.new(last);
                  list[list.Length - 1] := NestedRule.new(last);
                  list.append(rule);
               };
            
            list[list.Length - 1] := rule;
         }
         else {
            bool continueMode := false;
            PatternRule rule := nil;
            if (ch == $91) {
               rule := parseGroup(pattern, ref i, len);
               continueMode := true
            }
            else rule := defineRule(ch);
   
            if:nil(rule) 
               InvalidArgumentException.raise("pattern");

            list.append(rule);

            if (continueMode) {
               :continue
            }
         };

         i += ch.Length;
      };

      int listLen := list.Length - 1;
      for (int i := 0; i < listLen; i++) {
         list[i].setNext(list[i + 1])
      };

      ^ list.First
   }   

   constructor prepare(string pattern)
   {
      _startRule := parsePattern(pattern);
   }

   bool match(string s)
   {
      Stack<PatternRule> parents := new Stack<PatternRule>();

      int len := s.Length;
      int index := 0;
      PatternRule current := _startRule;
      while (index < len) {
         char ch := s[index];

         current := current.makeStep(s, index, ch, parents);
         if:nil(current) {
            ^ false
         };

         index += ch.Length;
      };

      ^ current.isMatched(parents)
   }
}
