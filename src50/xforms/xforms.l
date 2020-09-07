import extensions'scripting;
import forms;

preloaded used = new object[]{ SDIDialog, PropVariant, Label, Button, Edit };

static ScriptEngine xformsEngine
    = new ScriptEngine()
            .loadScript("[[ #grammar build ]]")
            .loadPath("~\scripts\grammar.es")
            .loadPath("~\scripts\xforms.es");

public singleton xforms
{
    load(string script)
    {
        var tape := xformsEngine.buildScript(script).compile().program;
        
        ^ tape;
    }
    
    execute(string script, owner)
    {
        var tape := xformsEngine.buildScript(script).compile().program;
        
        ^ tape(owner);
    }
    
    loadPath(string path)
    {
        var tape := xformsEngine.buildScriptFile(path).compile().program;
        
        ^ tape;
    }
    
    executePath(string path, owner)
    {
        var tape := xformsEngine.buildScriptFile(path).compile().program;
        
        ^ tape(owner);
    }    
}