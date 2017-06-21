import xforms.

import system'drawing.
import system'winforms.

sealed class HelloWindow
{
    xform theForm.

    constructor new
    [
        theForm := Form new.
        
        theForm onInit func:[ $self onInit ].
        
        theForm create.
    ]

    onInit
    [
    ]
            
    dispatch => theForm.
}

helloWindow = HelloWindow new.
