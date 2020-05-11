import xforms;

public class MainWindow
{
    theForm;
    
    constructor new()
    {
        theForm := forward form
    }
    
    dispatch() => theForm;
}

public dynamicForm = xforms.loadPath("helloworld.xs");
                
public staticForm  = helloworld'stat'program();
