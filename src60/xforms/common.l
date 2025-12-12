import extensions'scripting;
import forms;

internal used : preloaded = new object[]{ XForm, Label, Button, Edit };

static ScriptEngine XFormsEngine
    = new ScriptEngine()
            .loadScript("[[ #grammar build ]]")
            .loadPath("~\scripts\grammar.es")
            .loadPath("~\scripts\xforms.es");

public singleton XForms
{
    load(string script)
    {
        var tape := XFormsEngine.buildScript(script).compile().program;
        
        ^ tape;
    }
    
    execute(string script, owner)
    {
        var tape := XFormsEngine.buildScript(script).compile().program;
        
        ^ tape(owner);
    }
    
    loadPath(string path)
    {
        var tape := XFormsEngine.buildScriptFile(path).compile().program;
        
        ^ tape;
    }
    
    executePath(string path, owner)
    {
        var tape := XFormsEngine.buildScriptFile(path).compile().program;
        
        ^ tape(owner);
    }    
}