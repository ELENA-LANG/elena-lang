static ScriptEngine xformsEngine
   = new ScriptEngine()
            .loadScript("[[ #grammar build ]]")
            .loadPath("~\scripts\grammar60.es")
            .loadPath("~\scripts\xforms60.es");

public singleton xforms
{

}