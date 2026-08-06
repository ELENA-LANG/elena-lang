import extensions;
import system'collections;
import system'routines;

public Program()
{   
   auto a1 := ArrayList.load(new int[] { 1, 2, 3 });
   auto a2 := ArrayList.load(new int[] { 1, 2, 3 });
   
   var b := a1.sequenceEqual(a2.asIndexable());
}
