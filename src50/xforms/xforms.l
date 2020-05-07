import extensions'scripting;
import forms;

preloaded used = new object[]{SDIDialog};

static ScriptEngine xformsEngine
    = new ScriptEngine()
            .loadPath("~\scripts\grammar.es")
            .loadPath("~\scripts\xforms.es");

public singleton xforms
{
    load(string script)
    {
        var tape := xformsEngine.buildScript(script).compiled().program;
        
        ^ tape();
    }
}