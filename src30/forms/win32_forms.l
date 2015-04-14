#define system.
#define system'winforms.

// --- SDI ---

#class(limited) SDIForm :: BaseForm
{
    #field(type:win_sdiwindow)theHandle.
    
    #method sdiform = $self.
    
    #method $beforeOpen
    [
        theHandle := SDIWindowHandle new &wide:theCaption &int:theX &int:theY &int:theWidth &int:theHeight.
    ]

    #method handle = theHandle handle.
            
    #method open
    [
        (nil == theHandle)
            ? [ $self $beforeOpen. ].
            
        theHandle setVisible &int:SW_SHOW.
    ]
}

// --- SDIDialog ---

#class SDIDialog :: SDIForm
{
}
