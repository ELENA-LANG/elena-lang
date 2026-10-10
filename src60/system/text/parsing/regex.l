import system'collections;

const string Reserved = "$^[]+*?()\";
const string Postfix = "+*?";

const int StrictStart = 1;
const int StrictEnd = 2;

public RegExException : Exception
{
   constructor new(int index)
   {
      this message := $"Cannot parse the regular expression at {index}";
   }
}

internal sealed ParserContext
{
//   Reference<int> _groupCounter;

//   Stack<Tuple<int, int>> _bookmarks;
   
   Stack<PatternRule> Parents: rprop(_parents);
   bool               eorHit: prop(_hit);
   
   constructor()
   {
      //_bookmarks := new Stack<Tuple<int, int>>();
      
      _hit := false;
      _parents := new Stack<PatternRule>();
   }
   
//   startGroup(int key, int index)
//   {
//      _bookmarks.push(new Tuple<int, int>(key, index));
//   }
//
//   stopGroup(int key, int index)
//   {
//      _bookmarks.push(new Tuple<int, int>(-key, index));
//   }
   
/*   
   int newGroup()
   {
      _groupCounter.append(1);
      
      ^ *_groupCounter
   }
*/   
   
   push(PatternRule item)
      => _parents;
   
   PatternRule pop()
      = _parents.pop();   
      
   PatternRule peek()
      = _parents.peek();
      
   bool empty()      
      = _parents.Length == 0;
}

internal interface PatternRule
{
   indexed internal abstract setNext(PatternRule rule);

   abstract PatternRule? gotoNext(ParserContext context);   
   
   internal abstract bool match(char ch, ParserContext context);
   internal abstract bool matchNext(char ch, ParserContext context);
   internal abstract PatternRule? peekNext(ParserContext context);   
   
   PatternRule? makeStep(string s, int index, char ch, ParserContext context)
   {
      if (match(ch, context)) {
         ^ gotoNext(context);
      };

      ^ nil;      
   }
}

/*
internal singleton VirtualRule : PatternRule
{
   indexed internal setNext(PatternRule rule)
      = NotSupportedException.raise();
   
   bool match(char ch)
      = true;
   
   PatternRule? gotoNext(ParserContext context)
   {
      ^ context.pop().gotoNext(context);
   }   
}
*/

internal abstract LinkRule : PatternRule
{
   PatternRule _nextRule;

   indexed internal setNext(PatternRule rule)
   {
      _nextRule := rule
   }

   internal bool matchNext(char ch, ParserContext context)
   {
      if:nil(_nextRule) {
         if (context.empty()) {
            ^ false
         };         
         
         ^ context.peek().matchNext(ch, context);
      };
      
      ^ _nextRule.match(ch, context);
   }

   internal PatternRule? peekNext(ParserContext context)
   {
      if:nil(_nextRule) {
         if (context.empty()) {
            ^ nil
         };         
         
         ^ context.peek();
      };

      ^ _nextRule;
   }
   

   PatternRule? gotoNext(ParserContext context)
   {
      if:nil(_nextRule) {
         if (context.empty()) {
            ^ nil
         };         
         
         ^ context.pop().gotoNext(context);
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
   
   internal bool match(char ch, ParserContext context)
      => _child; 

   PatternRule? makeStep(string s, int index, char ch, ParserContext context)
   {
      context.push(self);
      if (_child.match(ch, context)) {
         PatternRule? next := _child.makeStep(s, index, ch, context);
         if:not:nil(next) {
            ^ next;
         };
      };
      if (!context.empty())
         context.pop();
      
      ^ nil
   }
}

internal sealed AnyRule : LinkRule
{
   constructor()
   {
   }

   internal bool match(char ch, ParserContext context)
      = (ch != $10 && ch != $0);
}

internal sealed AnyWordChar : LinkRule
{
   constructor()
   {
   }

   internal bool match(char ch, ParserContext context)
   {
      if (ch.isLetter() || ch.isDigit()) {
         ^ true
      };
      
      ^ false
   }
}

internal sealed WhiteSpaceChar : LinkRule
{
   constructor()
   {
   }

   internal bool match(char ch, ParserContext context)
      = ch.isWhitespace();
}

internal sealed CharRule : LinkRule
{
   char _ch;

   constructor new(char ch)
   {
      _ch := ch;
   }

   internal bool match(char ch, ParserContext context)
      = _ch == ch;
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

   internal bool match(char ch, ParserContext context)
      = ch >= _chFrom && ch <= _chTill;
}

internal sealed OrRule : GroupRule
{
   constructor load(PatternRule[] rules)
   {
      _rules := rules
   }

   internal bool match(char ch, ParserContext context)
   {
      int len := _rules.Length;
      for (int i := 0; i < len; i++) {
         if (_rules[i].match(ch, context)) {
            ^ true
         }         
      };

      ^ false      
   }

   PatternRule? makeStep(string s, int index, char ch, ParserContext context)
   {
      context.push(self);

      int len := _rules.Length;
      for (int i := 0; i < len; i++) {
         PatternRule? next := _rules[i].makeStep(s, index, ch, context);
         if:not:nil(next) {
            ^ next;
         };
      };
      
      context.pop();
      
      ^ nil
   }
}

internal sealed InvertedRule : NestedRule
{
   constructor new(PatternRule child)
   {
      _child := child
   }   
   
   internal bool match(char ch, ParserContext context)
      = ch != 0 && _child.match(ch, context) == false;      

   PatternRule? makeStep(string s, int index, char ch, ParserContext context)
   {
      if (match(ch, context)) {
         ^ gotoNext(context);
      };

      ^ nil;      
   }      
}

/*internal sealed NoCaptureGroup : GroupRule
{
   constructor load(PatternRule[] rules)
   {
      _rules := rules
   }
}*/

internal sealed CaptureGroup : NestedRule
{
   constructor new(PatternRule child)
   {
      _child := child
   }
   
//   bool match(string s, int index, char ch)
//      = _rules[0].match(s, index, ch);
//      
//   PatternRule? makeStep(string s, int index, char ch, ParserContext context)
//   {
//      if (match(s, index, ch)) {
//         context.push(self);
//         
//         context.startGroup(_key, index);
//      }
//   }
//   
//   PatternRule? gotoNext(int index, ParserContext context)
//   {
//      context.stopGroup(_key, index);
//      
//      ^ super.gotoNext(index, context);
//   }
}

internal abstract OptionalBaseRule : NestedRule
{
   bool? _endingRule;
   
   protected constructor new(PatternRule child)
   {
      _child := child;
      _endingRule := nil
   }   

   private bool checkIfEndingRule(ParserContext context)
   {
      if (_endingRule != nil) {
         ^ _endingRule
      };
      
      PatternRule? next := peekNext(context);
      if:not:nil(next) {
         _endingRule := next.match($0, context);
      }
      else _endingRule := true;
      
      ^ _endingRule!;
   }
   

   internal bool match(char ch, ParserContext context)
   {
      if (matchNext(ch, context) || _child.match(ch, context)) {
         ^ true
      };
      
      ^ checkIfEndingRule(context);
   }
}

internal sealed OptionalRule : OptionalBaseRule
{
   constructor new(PatternRule child)
      <= super new(child)
   {
   }

   PatternRule? makeStep(string s, int index, char ch, ParserContext context)
   {
      if (matchNext(ch, context)) {
         ^ gotoNext(context)?.makeStep(s, index, ch, context)
      };
      
      if (_child.match(ch, context)) {
         context.push(self);
         PatternRule? next := _child.makeStep(s, index, ch, context);
         if:nil(next) {
            context.pop();
            
            ^ gotoNext(context)?.makeStep(s, index, ch, context);
         };
      
         ^ next   
      };
      
      ^ gotoNext(context)?.makeStep(s, index, ch, context);
   }
}

internal sealed RecursiveRule : OptionalBaseRule
{
   constructor new(PatternRule child)
      <= super new(child)
   {
   }

   PatternRule? gotoNext(ParserContext context)
   {
      ^ self
   }

   PatternRule? makeStep(string s, int index, char ch, ParserContext context)
   {
      if (matchNext(ch, context)) {
         ^ super.gotoNext(context)?.makeStep(s, index, ch, context);
      };
      
      if (_child.match(ch, context)) {
         context.push(self);
   
         PatternRule? next := _child.makeStep(s, index, ch, context);
         if:nil(next) {
            context.pop();
            
            ^ super.gotoNext(context)?.makeStep(s, index, ch, context);
         };
   
         ^ next         
      };
      
      ^ super.gotoNext(context)?.makeStep(s, index, ch, context);
   }
}

internal sealed singleton EORRule : PatternRule
{
   indexed internal setNext(PatternRule rule)
   {
      InvalidOperationException.raise("$ must be the last one in the pattern");
   }

   internal bool match(char ch, ParserContext context)
      = ch == $0;
      
   internal bool matchNext(char ch, ParserContext context)
      = false;

   PatternRule? makeStep(string s, int index, char ch, ParserContext context)
   {
      context.eorHit := true;
      
      ^ self
   }
      
   internal PatternRule? peekNext(ParserContext context)
      = nil;
      
   PatternRule? gotoNext(ParserContext context)
      = nil;
}

internal sealed singleton EOLRule : PatternRule
{
   indexed internal setNext(PatternRule rule)
   {
      InvalidOperationException.raise("$ must be the last one in the pattern");
   }

   internal bool match(char ch, ParserContext context)
      = ch == $0;

   internal bool matchNext(char ch, ParserContext context)
      = false;

   PatternRule? makeStep(string s, int index, char ch, ParserContext context)
      = (match(ch, context)) ? self : nil;

   internal PatternRule? peekNext(ParserContext context)
      = nil;
      
   PatternRule? gotoNext(ParserContext context)
      = nil;
}

public sealed class MatchResult
{
   Tuple<int, int>[] Matches      : rprop(_matches);

   get bool Matched()
      = _matches.Length > 0;
         
   bool cast()
      = self.Matched;

   internal constructor new(Array<Tuple<int, int>> matches)
   {
      _matches := matches;
   }
}

public sealed class RegEx
{
   PatternRule _startRule;
   int         _mode;

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

   private PatternRule? defineRule(char ch, bool allowSpecialSymbols)
   {
      if (allowSpecialSymbols) {
         ch ???
            $46: { ^ new AnyRule() };
   
         if (isReserved(ch))
            { ^ nil };
      };

      ^ CharRule.new(ch)
   }

   private PatternRule? defineEscRule(char ch)
   {
      if (isReserved(ch)) {
         ^ CharRule.new(ch)
      };
      
      ch ???
         $119: { ^ new AnyWordChar() }
          $87: { ^ InvertedRule.new(new AnyWordChar()) }
         $100: { ^ CharRangeRule.new($48, $57) }
          $68: { ^ InvertedRule.new(CharRangeRule.new($48, $57)) }
         $115: { ^ new WhiteSpaceChar() }
          $83: { ^ InvertedRule.new(new WhiteSpaceChar()) };
         
      ^ nil   
   }

   private PatternRule parseBrackets(string pattern, ref int index, int len)
   {
      int i := index + 1;
      
      PatternRule exprRule := parseExpression(pattern, ref i, len, $41);
      
      PatternRule groupRule := CaptureGroup.new(exprRule);

//      if (i == len)
//         InvalidArgumentException.raise("pattern");

      index := i;

      ^ groupRule
   }

   private PatternRule defineRule(string pattern, int len, ref int index, ref char ch)
   {
      int i := index;
//      int chLen := ch.Length;

      PatternRule? rule := nil;
      if (ch == $92) {
         i++;
         ch := pattern[i];
         rule := defineEscRule(ch);
      }
      else rule := defineRule(ch, true);

      index := i;

      ^ rule!;
   }

   private PatternRule parseGroup(string pattern, ref int index, int len)
   {
      bool inverted := false;
      int i := index + 1;
      if (i < len && pattern[i] == $94) {
         inverted := true;
         i++;
      };      

      List<PatternRule> orList := new List<PatternRule>();
      while (i < len) {
         char ch := pattern[i];
         int chLen := ch.Length;
         
         PatternRule? rule := nil;
         if (ch == $93) {
            :break;
         }
         else if(i + chLen + 1 < len && pattern[i + chLen] == $45 && pattern[i + chLen + 1] != $93) {
            i += chLen;
            i++;

            char lastCh := pattern[i];
            chLen := lastCh.Length;

            rule := CharRangeRule.new(ch, lastCh);

            ch := lastCh
         }
         else rule := defineRule(ch, false);
         
         if (rule != nil) {
            orList.append(rule)
         }
         else RegExException.new(i).raise();

         i += chLen
      };

      index := i;

      PatternRule rule;
      if (orList.Length == 1) {
         rule := orList.First
      }
      else rule := OrRule.load(orList.Value);
      
      if (inverted) {
         rule := InvertedRule.new(rule);
      };

      ^ rule
   }

//   private PatternRule parseLine(string pattern, ref int index, int len, char terminator)
//   {
//      auto list := new List<PatternRule>();
//      parseLine(list, pattern, ref index, len, terminator);
//      
//      ^ linkRules(list)
//   }

   private parseLine(List<PatternRule> list, string pattern, ref int index, int len, char terminator)
   {
      int i := index;
      while (i < len) {
         char ch := pattern[i];

         if (ch == terminator || ch == $124) {
            :break;
         }
         else if (isPostfix(ch)) {
            if(list.Empty)
               RegExException.new(i).raise();

            PatternRule rule;
            ch ???
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
//            bool continueMode := false;
            PatternRule? rule := nil;
            ch ???
               $91: {
                  rule := parseGroup(pattern, ref i, len);
                  ch := pattern[i];
//                  continueMode := true
               }
               $40: {
                  rule := parseBrackets(pattern, ref i, len);
                  ch := pattern[i];
//                  continueMode := true
               }
               : { rule := defineRule(pattern, len, ref i, ref ch) };
//   
            if:nil(rule) 
                RegExException.new(i).raise();

            list.append(rule!);

/*            if (continueMode) {
               :continue
            }*/
         };

         i += ch.Length;
      };

      index := i;
   }

   private PatternRule parseExpression(string pattern, ref int index, int len, char terminator/*, GroupType type*/)
   {
      List<PatternRule>? orGroup := nil;
      List<PatternRule> groupList := new List<PatternRule>();
      
      int i := index;
      while (i < len) {
         parseLine(groupList, pattern, ref i, len, terminator);         
         char ch := $0;
         if (i < len)
            ch := pattern[i];

         if (ch == $124) {
            if:nil(orGroup)
               orGroup := new List<PatternRule>();
            
            orGroup!.append(linkRules(groupList));
            
            groupList.clear();
            
            i++;
         }
         else if (ch != $0 && ch != terminator) {
            RegExException.new(i).raise()
         }
         else {            
            :break;
         }
      };
      
      PatternRule exprRule;
      if:not:nil(orGroup) {
         if (groupList.Length > 0) {
            orGroup.append(linkRules(groupList));
         };         
         
         exprRule := OrRule.load(*orGroup);
      }
      else exprRule := linkRules(groupList);

      index := i;

      ^ exprRule
   }

   private PatternRule linkRules(List<PatternRule> list)
   {
      int listLen := list.Length - 1;
      for (int i := 0; i < listLen; i++) {
         list[i].setNext(list[i + 1])
      };

      ^ list.First
   }

   private PatternRule parsePattern(string pattern)
   {
      int len := pattern.Length;
      int i := 0;

      List<PatternRule> list := new List<PatternRule>();
      if (pattern[0] == $94) {
         _mode := _mode | StrictStart;
         i++;
      };

      list.append(NestedRule.new(parseExpression(pattern, ref i, len, $36)));
      if (i < len && pattern[i] == $36) {
         list.append(EOLRule);
         _mode := _mode | StrictEnd;
         i++;
      }
      else list.append(EORRule);

      if (i < len)
         RegExException.new(i).raise();

      ^ linkRules(list)
   }   

   constructor prepare(string pattern)
   {
      _mode := 0;
      _startRule := parsePattern(pattern);
   }

   private MatchResult retrieveResult(string s)
   {
      bool strictStart := _mode.anyMask(StrictStart);
      bool strictStop := _mode.anyMask(StrictStart);
      
      auto matches := new List<Tuple<int, int>>();
      ParserContext context := new ParserContext();
      PatternRule? current := _startRule;
      
      int len := s.Length;
      int start := 0;
      int index := 0;
      while (index < len) {
         if:nil(current) {
            if (strictStart)
               :break;
            
            start := index;
            current := _startRule;
         };

         char ch := s[index];
         current := current?.makeStep(s, index, ch, context);
         if:nil(current) {
            if (index > start) {
               :continue;
            }
         }
         else if (current == EORRule) {
            if (context.eorHit) {
               context.eorHit := false;               
               matches.append(new Tuple<int,int>(start, index));
            }
            else matches.append(new Tuple<int,int>(start, index + ch.Length));
            current := nil
         };
         
         index += ch.Length;
      };

      PatternRule? result := current?.makeStep(s, len, $0, context) ?? nil;
      if:not:nil(result)
         matches.append(new Tuple<int,int>(start, index));
      
      ^ MatchResult.new(*matches);
   }

   retoverload MatchResult match(string s)
      = retrieveResult(s);

   bool match(string s)
   {
      MatchResult r := retrieveResult(s);

      ^ r.Matched;
   }
}
