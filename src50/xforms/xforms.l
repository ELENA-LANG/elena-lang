import extensions'scripting;
import forms;

preloaded used = new object[]{ SDIDialog, Variant, Label };

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
    
    loadPath(string path)
    {
        var tape := xformsEngine.buildScriptFile(path).compiled().program;
        
        ^ tape();
    }
}