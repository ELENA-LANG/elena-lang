import extensions'scripting.

class MainWindow
{
    object theForm.
    
    constructor new
    [
        theForm := 'form.
    ]
    
    dispatch => theForm.
}

dynamicForm = Library new; evalSelf:
                (:l) [
                    scriptEngine 
                        load path:"~\scripts\assembly.es";
                        load path:"xforms.es";
                        eval path:"helloworld.xs";
                        eval:l
                ];
                first; eval; 
                eval.

//  staticForm =    helloworld'stat'mainForm.
