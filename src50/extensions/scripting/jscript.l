public singleton jscript
{
    eval(string script)
        = jscriptEngine.buildScript(script).compile();
            
    evalPath(string path)
        = jscriptEngine.buildScriptFile(path).compile();
        
    load(string path)
        = jscriptEngine.loadPath(path);
}