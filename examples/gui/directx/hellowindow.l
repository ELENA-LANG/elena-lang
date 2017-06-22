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
        theForm OnDestroy func:[ $self onDestroy ].
        
        theForm create.
    ]

    onInit
    [
        system'external'elenagm InitD3D hwnd:theForm.
    ]

    onDestroy
    [
        system'external'elenagm CleanD3D.
    ]

    dispatch => theForm.
}

helloWindow = HelloWindow new.
