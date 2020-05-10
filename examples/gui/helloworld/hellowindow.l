import xforms;

public class MainWindow
{
    theForm;
    
    constructor new()
    {
        theForm := /*'form*/xforms.load("<Form Height=""200"" Width=""200""> </Form>")
    }
    
    dispatch() => theForm;
}

//dynamicForm = Library new; evalSelf:
//                (:l) [
//                    scriptEngine 
//                        load path:"~\scripts\assembly.es";
//                        load path:"xforms.es";
//                        eval path:"helloworld.xs";
//                        eval:l
//                ];
//                first; eval.
                
//staticForm =    helloworld'stat'mainForm.
