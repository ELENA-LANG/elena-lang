//import extensions;
//import extensions'scripting;
//   
//public program()
//{
//    var t := new ScriptEngine()
//            .loadScript("[[ #grammar build ]]")
//            .loadPath("~\scripts\grammar60.es")
//            .loadPath("~\scripts\tscript60.es");
//            
//    var o := t.buildScript("public program() { console.writeLine(2) }");
//    
//    o.eval();
//}

import system'dynamic;

public program()
{
   var prop_x := new MessageName("x");
   var prop_y := new MessageName("y");

   var s := Struct.new(prop_x, 1, prop_y, 2);

   var x := s.x;
   var y := s.y;
}