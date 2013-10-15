#define std'dictionary'*.
#define std'basic'*.
#define sys'dates'*.
#define gui'common'*.
#define gui'dictionary'*.

#define forms'*    = gui'forms'*.
#define controls'* = gui'controls'*.

#class Window (forms'SDIDialog)
{
    #field theLabel.
    #field theTime.
    #field theTimer.
    #field theStart.
    #field theStop.

    #method initializing : Args
    [
        super initializing:Args.

        //#var aDate := Now~ETimeConvertor literal.

        theLabel  := controls'StaticLabel::self.
        theTime := controls'StaticLabel::self.
        theTimer  := controls'Timer::self.
        theStart := controls'Button::self.
        theStop := controls'Button::self.

        // Form
        self
            dimensions'set &width:500 &height:100
            coordinates'set &x:350 &y:250
            caption'set:"Timer".

        // Label
        theLabel
            dimensions'set &width:100 &height:20
            coordinates'set &x:12 &y:10
            caption'set:"Current Time:"
            open.
        
        theTime
            dimensions'set &width:500 &height:20
            coordinates'set &x:12 &y:30
            open.
        
        // Button
        theStart
            dimensions'set &width:65 &height:20
            coordinates'set &x:410 &y:10
            caption'set:"Start"
            open.
        
        theStop
            dimensions'set &width:65 &height:20
            coordinates'set &x:410 &y:10
            caption'set:"Stop".

        theTimer
            timer_interval'set:1000.

        theStart event_handle'append:
        {
            clicking'eval
            [
                theTimer open.

                theStart close.
                theStop open.
            ]
        }.

        theStop event_handle'append:
        {
            clicking'eval
            [
                theTimer close.

                theStart open.
                theStop close.
            ]
        }.

        theTimer event_handle'append:
        {
            timing'eval
            [
                theTime caption'set:(WideStrValue &&calendar:Now &:etimeformatter).
            ]
        }.
    ]
}