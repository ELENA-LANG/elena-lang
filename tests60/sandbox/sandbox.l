//import extensions;
//import system'collections;
//
//const string Reserved = "$^[]+";
//
//
//internal interface PatternRule
//{
//   abstract PatternRule gotoNext(Stack<PatternRule> parents);
//   
//   abstract PatternRule? makeStep(string s, int index, char ch, Stack<PatternRule> parents);
//}
//
//internal abstract LinkRule
//{
//   PatternRule _nextRule;
//
//   PatternRule gotoNext(Stack<PatternRule> parents)
//   {
//      if:nil(_nextRule) {
//         ^ parents.pop().gotoNext(parents);
//      };
//
//      ^ _nextRule;
//   }
//}
//
//internal abstract GroupRule : LinkRule
//{
//   PatternRule[] _rules;
//}
//
//internal NestedRule : LinkRule
//{
//   PatternRule _child;
//
//   PatternRule? makeStep(string s, int index, char ch, Stack<PatternRule> parents)
//   {
//      parents.push(self);
//      PatternRule next := _child.makeStep(s, index, ch, parents);
//      if:nil(next) {
//         parents.pop();
//      };
//
//      ^ next
//   }
//}
//
//internal sealed OptionalRule : NestedRule
//{
//   PatternRule? makeStep(string s, int index, char ch, Stack<PatternRule> parents)
//   {
//      parents.push(self);
//      PatternRule next := _child.makeStep(s, index, ch, parents);
//      if:nil(next) {
//         parents.pop();
//
//         ^ gotoNext(parents).makeStep(s, index, ch, parents)
//      };
//
//      ^ next
//   }
//}
//
//internal sealed CharRule : LinkRule
//{
//   char _ch;
//
//   constructor new(char ch)
//   {
//      _ch := ch;
//   }
//
//   PatternRule? makeStep(string s, int index, char ch, Stack<PatternRule> parents)
//   {
//      if (_ch == ch) {
//         ^ gotoNext(parents);
//      };
//
//      ^ nil;
//   }
//}
//
//internal sealed OrRule  : GroupRule
//{
//   PatternRule? makeStep(string s, int index, char ch, Stack<PatternRule> parents)
//   {
//      parents.push(self);
//
//      int len := _rules.Length;
//      for (int i := 0; i < len; i++) {
//         PatternRule next := _rules[i].makeStep(s, index, ch);
//         if:not:nil(next) {
//            ^ next;
//         };
//      };
//      
//      parents.pop();
//      
//      ^ nil
//   }
//}
//
//internal sealed RecursiveRule : NestedRule
//{
//   PatternRule gotoNext(Stack<PatternRule> parents)
//   {
//      ^ self
//   }
//
//   PatternRule? makeStep(string s, int index, char ch, Stack<PatternRule> parents)
//   {
//      parents.push(self);
//
//      PatternRule next := _child.makeStep(s, index, ch, parents);
//      if:nil(next) {
//         parents.pop();
//
//         ^ super.gotoNext(parents).makeStep(s, index, ch, parents);
//      }
//   }
//}
//
//
//
//
//internal interface PatternRule
//{
//   abstract int match(int state, string s, int index, char ch);
//}
//
//internal sealed CharRule : PatternRule
//{
//   char _ch;
//
//   int match(int state, string s, int index, char ch)
//   {
//      if (_ch == ch) {
//         ^ state + 1
//      };
//
//      ^ -1
//   }
//}
//
//internal abstract GroupRule : PatternRule
//{
//   PatternRule[] _rules;
//}
//
//internal sealed OrRule : GroupRule
//{
//   constructor load(PatternRule[] rules)
//   {
//      _rules := rules;
//   }
//
//   int match(int state, string s, int index, char ch)
//   {
//      int len := _rules.Length;
//      for (int i := 0; i < len; i++) {
//         int next := _rules[i].match(state, s, index, ch);
//         if (next != -1) {
//            ^ next
//         }
//      };
//
//      ^ -1
//   }
//}
//
//internal sealed RecursiveRule : PatternRule
//{
//   PatternRule _child;
//
//   int match(int state, string s, int index, char ch)
//   {
//      int next := _child.match(state, s, index, ch);
//      if (next == -1) {
//         ^ state + 1
//      };
//
//      ^ state  
//   }
//}
//
//internal sealed EOLRule : PatternRule
//{
//   int match(string s, int index, char ch)
//      = index == s.Length ? 0 : -1;
//}
//
//public sealed class RegEx
//{
//   PatternRule[] _rules;
//
//   constructor(string pattern)
//      = RegEx.prepare(pattern);
//
//   private bool isReserved(char ch)
//   {
//      ^ Reserved.indexOf(ch) != -1
//   }
//
//   private PatternRule? defineRule(char ch)
//   {
//      if (isReserved(ch))
//         { ^ nil };
//
//      ^ CharRule.new(ch)
//   }
//
//   private int parseOrGroup(string pattern, int index, int len, List<PatternRule> list)
//   {
//      int i := index;
//
//      List<PatternRule> orList := new List<PatternRule>();
//      while (i < len) {
//         char ch := pattern[i];
//         ch =>
//            $93 : { i++; $break; }
//              ! : { 
//                  PatternRule rule := defineRule(ch);
//                  if (rule != nil) {
//                     orList.append(rule)
//                  }
//                  else InvalidArgumentException.raise("pattern")
//               };
//
//         i += ch.Length
//      };
//
//      list.append(OrRule.load(orList.Value));
//
//      ^ i;
//   }
//
//   private PatternRule[] parsePattern(string pattern)
//   {
//      int len := pattern.Length;
//      int i := 0;
//
//      List<PatternRule> list := new List<PatternRule>();
//
//      if (pattern[0] == $94) {
//         i++;
//      }
//      else {
//         InvalidArgumentException.raise("pattern"); // !! temporally
//      };
//
//      while (i < len) {
//         char ch := pattern[i];
//         ch =>
//            $91 : 
//            {  
//               i := parseOrGroup(pattern, i + 1, len, list);
//
//               $continue;
//            }
//            $36 : { list.append(new EOLRule()) }
//              ! : { 
//               PatternRule rule := defineRule(ch);
//               if (rule != nil) {
//                  list.append(rule)
//               }
//               else InvalidArgumentException.raise("pattern")
//            };         
//
//         i += ch.Length;
//      };
//
//      ^ list.Value;
//   }   
//
//   constructor prepare(string pattern)
//   {
//      _rules := parsePattern(pattern);
//   }
//
//   bool match(string s)
//   {
//      int len := s.Length;
//      int index := 0;
//      int current := 0;
//      while (index < len) {
//         char ch := s[index];
//
//         current := _rules[current].match(s, index, ch);
//         if (current == -1) {
//            ^ false;
//         };
//
//         index += ch.Length;
//      };
//
//      // check ending pattern
//      ^ _rules[current].match(s, len, $0) == 0;
//   }
//}
//
//public program()
//{
////   auto regex := new RegEx("^[a-z]+$");
////
////   var b := regex.match("abcd");
////   var b2 := regex.match("ab3cd");
//
//   auto regex := new RegEx("^[ab]+$");
//
//   var b := regex.match("a");
//   var b2 := regex.match("ab");
//   var b3 := regex.match("b");   
//}


public program()
{
   int n := 2;

   int r := n shl:2;
   int r2 := r shr:2;
}