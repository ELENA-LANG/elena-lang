#import system.
#import forms.
#import extensions.

#class Window
{
    #field form.
    #field btmIncrement.
    #field btmRandom.
    #field txtNumber.
    
    // Contains the number as showed in TextBox
    #field number.
    
    #constructor new
    [        
        form := SDIDialog new.
        btmIncrement := Button new.
        btmRandom    := Button new.
        txtNumber    := Edit new.
        
        number := 0.
        
        form controls 
                += btmIncrement
                += btmRandom
                += txtNumber.
    
        form set &caption:"Rosseta Code".
        form set &x:100 &y:100.
        form set &width:160 &height:120.
        
        txtNumber set &x:7 &y:7.
        txtNumber set &width:140 &height:25.
        txtNumber set &caption:(number literal).
        
        btmIncrement set &x:7 &y:35.
        btmIncrement set &width:140 &height:25.
        btmIncrement set &caption:"Increment".
        btmIncrement set &onClick:args
            [ $self $onButtonIncrementClick. ].
            
        btmRandom set &x:7 &y:65.
        btmRandom set &width:140 &height:25.
        btmRandom set &caption:"Random".
        btmRandom set &onClick:args
            [ $self $onButtonRandomClick. ].
    ]
    
    #method $onButtonIncrementClick
    [
        number := number + 1.   
        $self changeTextBoxValue.
    ]
    
    #method $onButtonRandomClick
    [
        (messageDialog open &caption:"Inf" &question:"Really reset to random value?")?
        [
            number := randomGenerator eval:99999999.
            $self changeTextBoxValue.
        ].
    ]
    
    #method changeTextBoxValue
    [
        txtNumber set &caption:(number literal).
    ]
    
    #method => form.
}