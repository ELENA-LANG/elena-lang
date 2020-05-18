import xforms;

public class MainWindow
{
    theForm;
    
    constructor new()
    {
        theForm := (forward form)(self)
    }
    
    onExit(arg)
    {
        forward program.stop()
    }
    
    dispatch() => theForm;
}

public dynamicForm = xforms.loadPath("helloworld.xs");
                
public staticForm  = helloworld'stat'program;
