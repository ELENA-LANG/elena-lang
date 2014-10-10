#define system.
#define system'text.
#define system'dynamic.

// --- ETextSerializer ---

#class(role) textserializable
{
    #method literal
    [
        #var aResult := TextBuffer new.
        #var anObject := self.

        aResult << anObject Name.
        #var(type:int) aCounter := anObject fieldCount int.
        (0 < aCounter) ?
        [       
            aResult << "{".

            control forrange &int:0 &int:(aCounter-1) &do: (&int:i)
            [
                (0 == i) 
                    ! [ aResult << "," ].
                
                #var aMember := anObject getFieldAt &index:i.

                aResult << aMember~textserializable literal.
            ].

            aResult << "}".
        ].

        ^ aResult literal.
    ]
}