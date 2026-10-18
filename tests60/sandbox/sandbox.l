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

import system'collections;

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

public Program()
{
   //auto regEx := new RegEx("^abc$");
   //MatchResult result := regEx.match("abc");
   //MatchResult result := regEx.match("ab");
   //MatchResult result := regEx.match("abcd");
   
   //auto regEx := new RegEx("a");
   //MatchResult result := regEx.match("a");
   //MatchResult result := regEx.match("__aaa__");
   //MatchResult result := regEx.match("__xxx__");

   //auto regEx := new RegEx("^xa*$");
   //MatchResult result := regEx.match("x");
            
   //auto regEx := new RegEx("a*");
   //MatchResult result := regEx.match("a");
   //MatchResult result := regEx.match("x");
   //MatchResult result := regEx.match("xa");
   //MatchResult result := regEx.match("aa");
   //MatchResult result := regEx.match("__aaa__");
   //MatchResult result := regEx.match("__a_a_a__");
   //MatchResult result := regEx.match("__xxx__");

   //auto regEx := new RegEx("a+");
   //MatchResult result := regEx.match("a");
   //MatchResult result := regEx.match("aa");
   //MatchResult result := regEx.match("__aaa__");
   //MatchResult result := regEx.match("__xxx__");


   auto regEx := new RegEx("(?<file>fi=(?<name>\w+))");
   MatchResult result := regEx.match("fi=2");


   //auto regEx := new RegEx("(?<console>co)|(?<file>fi=(?<name>\w+))");
   //MatchResult result := regEx.match("fi=2");


   //auto regEx := new RegEx("(?<console>console;)|(?<file>file=(?<name>\w+;))");
   //MatchResult result := regEx.match("console;file=log;");
   //MatchResult result := regEx.match("console");
   
   if (result.Matched){
      auto consoleGroup := regEx.retrieveGroup(result, "console");
      if (consoleGroup != nil) {
         if:not:nil (var MatchGroup? r := *consoleGroup) {
            Console.printLine("start:", r.Start, " end:", r.End);
         }
         else {
            Console.writeLine("Several values are not allowed")
         };         
      };

      auto fileGroup := regEx.retrieveGroup(result, "file");
      if (fileGroup != nil) {
         if:not:nil (var MatchGroup? r := *fileGroup) {
            Console.printLine("start:", r.Start, " end:", r.End);
         }
         else {
            Console.writeLine("Several values are not allowed")
         };
      };
            
   };
   
   

//   printMatchResult(result);
   
//   auto group := regEx.retrieveGroup(result, "group");
//   if:not:nil (var MatchGroup? r := *group) {
//      Console.printLine("start:", r.Start, " end:", r.End);
//   }
//   else if:not:nil (MatchGroup[] matches := *group) {
//      matches.forEach::(r) 
//      {
//         Console.printLine("start:", r.Start, " end:", r.End);
//      }
//   };

   
   
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
//   
//   
//   auto regEx := new RegEx("abc");
//   auto regEx := new RegEx(".*abc");
//   auto regEx := new RegEx("(a)(b)(c)");
//   
//   MatchResult r := regEx.match("abc");
//   MatchResult r := regEx.match("__abc__");
   
/*   if (r.Matched) {
      Console.printLine("the expression is matched from ", r.Start, " to ", r.End);
   }
   else Console.printLine("the expression is not matched");*/
   
//   LogSetup.registerConsoleLogger(LogLevel.Info).injectLogger();
   
//   MyClassToLog.new().start().do().stop();
}
