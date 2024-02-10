static ScriptEngine lscriptEngine
   = new ScriptEngine()
            .loadScript("[[ #grammar build ]]")
            .loadPath("~\scripts\grammar60.es")
            .loadPath("~\scripts\lscript60.es");
