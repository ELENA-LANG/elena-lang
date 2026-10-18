import system'collections;
import system'routines;

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
   Stack<Tuple<int, int>> _bookmarks;
   
   Stack<PatternRule> Parents: rprop(_parents);
   bool               eorHit: prop(_hit);
   
   constructor()
   {
      _bookmarks := new Stack<Tuple<int, int>>();
      
      _hit := false;
      _parents := new Stack<PatternRule>();
   }
   
   startGroup(int key, int index)
   {
      _bookmarks.push(new Tuple<int, int>(key, index));
   }

   stopGroup(int key, int index)
   {
      _bookmarks.push(new Tuple<int, int>(-key, index));
   }

   Tuple<int, int> popBookmark()
      = _bookmarks.pop();   
      
   bool emptyBookmarks()
      =_bookmarks.Length == 0;

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

   abstract PatternRule? gotoNext(int index, ParserContext context);   
   
   internal abstract bool match(char ch, ParserContext context);
   internal abstract bool matchNext(char ch, ParserContext context);
   internal abstract PatternRule? peekNext(ParserContext context);   
   
   PatternRule? makeStep(string s, int index, char ch, ParserContext context)
   {
      if (match(ch, context)) {
         ^ gotoNext(index, context);
      };

      ^ nil;      
   }
}

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
   

   PatternRule? gotoNext(int index, ParserContext context)
   {
      if:nil(_nextRule) {
         if (context.empty()) {
            ^ nil
         };         
         
         ^ context.pop().gotoNext(index, context);
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
         ^ gotoNext(index, context);
      };

      ^ nil;      
   }      
}

internal sealed NonCaptureGroup : NestedRule
{
   constructor new(PatternRule child)
   {
      _child := child
   }
}

internal sealed CaptureGroup : NestedRule
{
   int _key;
   
   constructor new(int key, PatternRule child)
   {
      _key := key;
      _child := child
   }
   
   PatternRule? makeStep(string s, int index, char ch, ParserContext context)
   {
      if (_child.match(ch, context)) {
         context.startGroup(_key, index);
         
         context.push(self);
         PatternRule? next := _child.makeStep(s, index, ch, context);
         if:not:nil(next) {
            ^ next;
         };
         context.pop();
      };
      
      ^ nil      
   }  
   
   PatternRule? gotoNext(int index, ParserContext context)
   {
      context.stopGroup(_key, index);
      
      ^ super.gotoNext(index, context);
   }
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
         ^ gotoNext(index, context)?.makeStep(s, index, ch, context)
      };
      
      if (_child.match(ch, context)) {
         context.push(self);
         PatternRule? next := _child.makeStep(s, index, ch, context);
         if:nil(next) {
            context.pop();
            
            ^ gotoNext(index, context)?.makeStep(s, index, ch, context);
         };
      
         ^ next   
      };
      
      ^ gotoNext(index, context)?.makeStep(s, index, ch, context);
   }
}

internal sealed RecursiveRule : OptionalBaseRule
{
   constructor new(PatternRule child)
      <= super new(child)
   {
   }

   PatternRule? gotoNext(int index, ParserContext context)
   {
      ^ self
   }

   PatternRule? makeStep(string s, int index, char ch, ParserContext context)
   {
      if (matchNext(ch, context)) {
         ^ super.gotoNext(index, context)?.makeStep(s, index, ch, context);
      };
      
      if (_child.match(ch, context)) {
         context.push(self);
   
         PatternRule? next := _child.makeStep(s, index, ch, context);
         if:nil(next) {
            context.pop();
            
            ^ super.gotoNext(index, context)?.makeStep(s, index, ch, context);
         };
   
         ^ next         
      };
      
      ^ super.gotoNext(index, context)?.makeStep(s, index, ch, context);
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
      
   PatternRule? gotoNext(int index, ParserContext context)
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
      
   PatternRule? gotoNext(int index, ParserContext context)
      = nil;
}

public sealed class MatchGroup : record(int GroupKey, int Start, int End);

public sealed class MatchResult
{
   Tuple<int, int>[] Matches : rprop(_matches);
   MatchGroup[] Groups       : rprop(_groups);  

   get bool Matched()
      = _matches.Length > 0;
         
   bool cast()
      = self.Matched;

   internal constructor new(Array<Tuple<int, int>> matches, MatchGroup[] groups)
   {
      _matches := matches;
      _groups := groups
   }
}

public sealed class RegEx
{
   Map<string, int> _groupNames;
   
   PatternRule _startRule;
   int         _mode;
   int         _lastKey; 

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
          $83: { ^ InvertedRule.new(new WhiteSpaceChar()) }
         $117: { ^ CharRangeRule.new($65, $90) };
         
      ^ nil   
   }

   private registerName(int key, string name)
   {
      _groupNames[name] := key;
   }

   private string readGroupName(string pattern, ref int index)
   {
      int i := index + 1;
      int start := i;
      while (pattern[i].isLetter()) {
         i++;
      };
      
      index := i;
      
      ^ pattern.Substring(start, i - start);
   }   

   private PatternRule parseBrackets(string pattern, ref int index, int len)
   {
      int i := index + 1;
      
      string? name := nil;
      bool noncapturing := false;
      if (pattern[i]== $63) {
         i++;
         if (pattern[i]== $58) {
            noncapturing := true;
            i++;
         }
         else if (pattern[i] == $60) {
            name := readGroupName(pattern, ref i);
            if (pattern[i] == $62) {
               i++;
            }
            else RegExException.new(i).raise();
         }
         else RegExException.new(i).raise();
      };
      
      PatternRule exprRule := parseExpression(pattern, ref i, len, $41);
      PatternRule groupRule;
      if:not(noncapturing) {
         _lastKey := _lastKey + 1;
         if:not:nil(name) {
            registerName(_lastKey, name);
         };
         
         groupRule := CaptureGroup.new(_lastKey, exprRule);
      }
      else groupRule := NonCaptureGroup.new(exprRule);

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
            PatternRule? rule := nil;
            ch ???
               $91: {
                  rule := parseGroup(pattern, ref i, len);
                  ch := pattern[i];
               }
               $40: {
                  rule := parseBrackets(pattern, ref i, len);
                  ch := pattern[i];
               }
               : { rule := defineRule(pattern, len, ref i, ref ch) };

            if:nil(rule) 
                RegExException.new(i).raise();

            list.append(rule!);
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
      _groupNames := new Map<string, int>();
      _mode := 0;
      _lastKey := 0;
      _startRule := parsePattern(pattern);
   }

   bool checkGroup(MatchResult result, string name)
   {
      int key := _groupNames[name];
      
      ^ result.Groups.ifExists(key, (val, x => x.GroupKey == val))
   }

   Union<MatchGroup, MatchGroup[]> retrieveGroup(MatchResult result, string name)
   {
      if:not(_groupNames.containsKey(name)) {
         ^ nil
      };
      
      MatchGroup? group := nil;
      List<MatchGroup>? list := nil;
                  
      auto groups := result.Groups;
      int key := _groupNames[name];
      int len := groups.Length;
      for (int i := 0; i < len; i++) {
         if (groups[i].GroupKey == key) {
            if:nil(group) {
               group := groups[i];
            }
            else {
               if:nil(list) {
                  list := new List<MatchGroup>();
                  list!.append(group);
               };
               
               list!.append(groups[i]);
            };
         };
      };
      
      ^ (list != nil) ? (*list) : group;
   }

   private MatchGroup[] createGroups(string s, ParserContext context)
   {
      List<MatchGroup> groups := new List<MatchGroup>();
      Stack<Tuple<int, int>> ends := new Stack<Tuple<int, int>>();
      while (!context.emptyBookmarks()) {
         auto pair := context.popBookmark();
         if (pair.Item1 < 0) {
            ends.push(pair);
         }
         else {
            auto end := ends.pop();
            int key := -(end.Item1);
            if (key == pair.Item1) {
               int endIndex := end.Item2;
               char ch := s[endIndex];
               
               groups.append(new MatchGroup(pair.Item1, pair.Item2, endIndex + ch.Length));
            }
         }
      };
      
      ^ *groups
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
      
      ^ MatchResult.new(*matches, createGroups(s, context));
   }

   retoverload MatchResult match(string s)
      = retrieveResult(s);

   bool match(string s)
   {
      MatchResult r := retrieveResult(s);

      ^ r.Matched;
   }
}
