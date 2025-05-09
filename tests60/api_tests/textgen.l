import ltests;
import textgen;
import extensions;

// == TEXTGEN tests ==
// =======================

// --- ForLoopTest ---

public textGen_ForLoopTest() : testCase()
{
   var actual := 5.generateFrom("for(var n := 0; n < self; n := n + 1) { <={n}=> }");

   Assert.ifEqual(actual, "01234");
   Console.writeLine(".");
}