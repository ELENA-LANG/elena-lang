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
        theForm onDestroy func:[ $self onDestroy ].
        theForm onPaint func:[ $self onPaint ].
        
        theForm create.
    ]

    onInit
    [
        system'external'elenagm InitD3D int:(theForm hwnd).
    ]

    onDestroy
    [
        system'external'elenagm CleanD3D int:(theForm hwnd).
    ]

    onPaint
    [
        system'external'elenagm Render3D int:(theForm hwnd).
    ]

    dispatch => theForm.
}

helloWindow = HelloWindow new.
