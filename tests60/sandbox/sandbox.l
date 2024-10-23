//import system'collections;
//
//const string Reserved = "$^[]";
//
//internal interface PatternRule
//{
//   abstract int match(string s, int index, char ch);
//}
//
//internal sealed CharRule : PatternRule
//{
//   char _ch;
//
//   constructor new(char ch)
//   {
//      _ch := ch;
//   }
//
//   int match(string s, int index, char ch)
//   {
//      if (_ch == ch) {
//         ^ index + 1
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
//   int match(string s, int index, char ch)
//   {
//      int len := _rules.Length;
//      for (int i := 0; i < len; i++) {
//         int next := _rules[i].match(s, index, ch);
//         if (next != -1) {
//            ^ next
//         }
//      };
//
//      ^ -1
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
//            ! : { orList.append(defineRule(ch)) };
//
//         i := ch.Length
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
//   auto regex := new RegEx("^[ab]$");
//
//   var b := regex.match("a");
//   var b2 := regex.match("ab");
//   var b3 := regex.match("b");   
//}

public program()
{
   for (int i := 0; i < 4; i++) {
      i =>
         0 : { console.writeLine("0") }
         1 : { console.writeLine("1") }
         2 : { console.writeLine("2") }
         ! : { console.writeLine("!") }
   }
}
