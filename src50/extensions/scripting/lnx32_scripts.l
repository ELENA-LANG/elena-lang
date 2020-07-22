static ScriptEngine jscriptEngine
    = new ScriptEngine()
            .loadScript("[[ #grammar build ]]")
            .loadPath("~/scripts/grammar.es")
            .loadPath("~/scripts/jscript.es");

static ScriptEngine lscriptEngine
    = new ScriptEngine()
            .loadScript("[[ #grammar build ]]")
            .loadPath("~/scripts/grammar.es")
            .loadPath("~/scripts/lscript.es");

