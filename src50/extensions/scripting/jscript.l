public singleton jscript
{
    eval(string script)
        = jscriptEngine.buildScript(script).compiled();
            
    evalPath(string path)
        = jscriptEngine.buildScriptFile(path).compiled();
        
    load(string path)
        = jscriptEngine.loadPath(path);
}