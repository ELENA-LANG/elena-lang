import system'text;
import extensions;

public program()
{
    auto engine := new extensions'scripting'ScriptEngine();
    engine.loadPath("grammar1.es");
}