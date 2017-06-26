import xforms.

import system'drawing.
import system'winforms.

sealed class HelloWindow
{
    xform theForm.

    constructor new
    [
        theForm := Form new.
        
        theForm width := 1280.
        theForm height := 720.
        
        theForm onInit func:[ $self onInit ].
        theForm onDestroy func:[ $self onDestroy ].
        theForm onPaint func:[ $self onPaint ].
        
        theForm create.
    ]

    onInit
    [
        system'external'elenagm InitD3D int:(theForm hwnd).
        
        int parent := system'external'elenagm NewWidget int:0 int:1.
        
        system'external'elenagm SetLocation int:parent int:30 int:30.
        system'external'elenagm SetSize int:parent int:70 int:15.
        
        int text := system'external'elenagm NewWidget int:parent int:2.
        system'external'elenagm SetText int:text wide:"Hello Label"w.
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
