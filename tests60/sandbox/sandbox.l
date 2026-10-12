//import system'dynamic;
//import extensions'logging;
//import logger;
//
//public class MyClassToLog
//{
//   readonly ILogger _logger;
//   
//   constructor new(ILogger logger) : injectable_constructor()
//   {
//      _logger := logger;
//      _logger.writeEntry(LogLevel.Info, "Creating...");
//   }
//   
//   start()
//   {
//      _logger.writeEntry(LogLevel.Info, "Starting...");
//   }
//   
//   failMe()
//   {
//      Exception.raise("It fails me");
//   }   
//   
//   do()
//   {
//      _logger.writeEntry(LogLevel.Info, "Doing...");
//      
//      try
//      {
//         failMe()
//      }
//      catch(Exception ex)
//      {
//         _logger.writeFormattedEntry(LogLevel.Error, "Failed: {0}", ex);
//      }
//   }
//   
//   stop()
//   {
//      _logger.writeEntry(LogLevel.Info, "Stopping...");
//   }
//}

import system'text'parsing;
import extensions;

import system'routines;

//import system'collections;

/*
singleton A
{
   Union<int, long>? get(bool? f)
   {
      if:nil(f) {
         ^ nil
      }
      else if (f) {
         ^ 12
      }
      else {
         ^ 10000l
      }
   }
}*/

//printMatchResult(MatchResult result)
//{
//   if (result.Matched) {
//      Console.writeLine("match:");
//      foreach(auto t; in result.Matches) {
//         Console.printLine("from ", t.Item1, " till ", t.Item2);
//      }
//   }
//   else Console.writeLine("does not match");
//}

singleton A
{
   retoverload int? get()
      = /*nil*/0;   
      
   get()
      = "none";
   
}

public Program()
{
//   Reference<int?> r := new Reference<int?>(nil);
   
//   int n := *r ?? 2;
   
//   int/*?*/ n := A.get();
   
   //A a := new A();
   /*int*/int r := /*a*/A.get(/*false*/);
   //int n := *r ?? 0;
   //int n := r.Value;
   
//   //auto regEx := new RegEx("^abc$");
//   //MatchResult result := regEx.match("abc");
//   //MatchResult result := regEx.match("ab");
//   //MatchResult result := regEx.match("abcd");
//   
//   //auto regEx := new RegEx("a");
//   //MatchResult result := regEx.match("a");
//   //MatchResult result := regEx.match("__aaa__");
//   //MatchResult result := regEx.match("__xxx__");
//
//   //auto regEx := new RegEx("^xa*$");
//   //MatchResult result := regEx.match("x");
//            
//   //auto regEx := new RegEx("a*");
//   //MatchResult result := regEx.match("a");
//   //MatchResult result := regEx.match("x");
//   //MatchResult result := regEx.match("xa");
//   //MatchResult result := regEx.match("aa");
//   //MatchResult result := regEx.match("__aaa__");
//   //MatchResult result := regEx.match("__a_a_a__");
//   //MatchResult result := regEx.match("__xxx__");
//
//   auto regEx := new RegEx("a+");
//   //MatchResult result := regEx.match("a");
//   //MatchResult result := regEx.match("aa");
//   //MatchResult result := regEx.match("__aaa__");
//   //MatchResult result := regEx.match("__xxx__");
//
//
//
   //auto regEx := new RegEx("(abc|bc)+");
//   auto regEx := new RegEx("(?<group>abc|bc)+");
   //MatchResult result := regEx.match("abc");
   //MatchResult result := regEx.match("bc");
//   MatchResult result := regEx.match("abcabcabcbc");

//   printMatchResult(result);
   
/*   auto group := regEx.retrieveGroup(result, "group");
   if:not:nil (MatchGroup g := *group) {
      Console.writeLine("1 group");
   }
   else if:not:nil (MatchGroup[] matches := *group) {
      matches.forEach((r => 
      {
         Console.printLine("start:", r.First, " end:", r.End);
      }))
   };*/


//   auto regex3 := new RegEx("^a*$");
//   Assert.ifFalse(regex3.match("b"));
   
   
   
   
   //auto regEx := new RegEx("\D+");
   //auto regEx := new RegEx("a[^abc]a");
   //auto regEx := new RegEx("a*|x");
   //auto regEx := new RegEx("a*");
   //auto regEx := new RegEx("a");
   //auto regEx := new RegEx("^a*$");
   //auto regEx := new RegEx("^a+$");
   //auto regEx := new RegEx("^a$");
   //auto regEx := new RegEx("a+");
   //auto regEx := new RegEx("a?");
   //auto regEx := new RegEx("...");
   //auto regEx := new RegEx("[abc]+");
   //auto regEx := new RegEx("[abc]?");
   //auto regEx := new RegEx("\(a\)");
   
   //auto regEx := new RegEx("aa");
   //auto regEx := new RegEx("a");
   
   //MatchResult result := regEx.match("x");
   //MatchResult result := regEx.match("ax");
   //MatchResult result := regEx.match("aa");
   //MatchResult result := regEx.match(" hello my friend ");
   //MatchResult result := regEx.match(" 12-23-3456 ");
   //MatchResult result := regEx.match("b");
   //MatchResult result := regEx.match("a");
   //MatchResult result := regEx.match(" (a) (b) (c)");

   //MatchResult result := regEx.match("abcd");
   //MatchResult result := regEx.match("axa");
   //MatchResult result := regEx.match("aaa");
   //MatchResult result := regEx.match("xxx");
   //printMatchResult(result);
         
/*   printMatchResult(regEx.match("a"));
   printMatchResult(regEx.match(""));
   printMatchResult(regEx.match("xxx"));
   printMatchResult(regEx.match("abb"));*/
   
//   auto regEx := new RegEx("a|b");
//   bool b1 := regEx.match("a");
//   bool b2 := regEx.match("b");
//   bool b3 := regEx.match("c");
   
   
//   auto regEx := new RegEx("abc");
//   auto regEx := new RegEx(".*abc");
//   auto regEx := new RegEx("(a)(b)(c)");
   
//   MatchResult r := regEx.match("abc");
//   MatchResult r := regEx.match("__abc__");
   
/*   if (r.Matched) {
      Console.printLine("the expression is matched from ", r.Start, " to ", r.End);
   }
   else Console.printLine("the expression is not matched");*/
   
//   LogSetup.registerConsoleLogger(LogLevel.Info).injectLogger();
   
//   MyClassToLog.new().start().do().stop();
}

