import xforms.

import system'drawing.
import system'winforms.

sealed class HelloWindow
{
    form theForm.

    constructor new
    [
        theForm := Form new.
        
        theForm width := 1280.
        theForm height := 720.
        
        var label1 := Label new:theForm.
        label1 x := 40.
        label1 y := 40.
        label1 width := 200.
        label1 height := 15.
        label1 caption := "Hello World!".
        
        theForm create.
    ]

    dispatch => theForm.
}

helloWindow = HelloWindow new.
