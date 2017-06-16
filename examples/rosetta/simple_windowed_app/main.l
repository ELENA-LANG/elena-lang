import forms.
import extensions.

class Window
{
    object form.
    object lblClicks.
    object btmClickMe.
    
    //Store how much clicks the user doed
    object clicksCount.
    
    constructor new
    [        
        form := SDIDialog new.
        lblClicks := Label new.
        btmClickMe := Button new.
        
        clicksCount := 0.
        
        form controls; append:lblClicks.    
        form controls; append:btmClickMe.
    
        form set caption:"Rosseta Code".
        form set x:100 y:100.
        form set width:160 height:80.
    
        lblClicks set x:10 y:2.
        lblClicks set width:160 height:20.
        lblClicks set caption:"Clicks: 0".
        
        btmClickMe set x:7 y:20.
        btmClickMe set width:140 height:30.
        btmClickMe set caption:"Click me".
        btmClickMe set onClick: (:args)
            [ $self $onButtonClick ]
    ]
    
    $onButtonClick
    [
        clicksCount := clicksCount + 1.
        lblClicks set caption:("Clicks: " + clicksCount literal).
    ]
    
    dispatch => form.
}