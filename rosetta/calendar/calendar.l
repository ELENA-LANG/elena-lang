#define system.
#define system'text.
#define system'routines.
#define system'calendar.
#define extensions.
#define extensions'math.
#define extensions'routines.

// --- calendar ---

#symbol MonthNames = ("JANUARY","FEBRUARY","MARCH","APRIL","MAY","JUNE","JULY","AUGUST","SEPTEMBER","OCTOBER","NOVEMBER","DECEMBER").
#symbol DayNames = ("MO", "TU", "WE", "TH", "FR", "SA", "SU").

#class CalendarMonthPrinter
{
    #field theDate.
    #field theLine.
    #field theMonth.
    #field theYear.
    #field theRow.
    
    #constructor new &year:aYear &month:aMonth
    [
        theMonth := aMonth.
        theYear := aYear.
        theLine := TextBuffer new.
        theRow := Integer new &int:0.
    ]

    #method writeTitle
    [        
        theRow << 0.
        theDate := Date new &year:(theYear int) &month:(theMonth int) &day:1.
        control run:DayNames &forEach: aName
            [ theLine write:" " write:aName ].
    ]
    
    #method writeLine
    [
        theLine clear.
        
        (theDate month == theMonth)
            ? [
                theLine::stringOp write:"   " &length:(((theDate dayOfWeek) => 0 ? [ 7 ] ! [ theDate dayOfWeek ]) subtract:1 int).
                
                control do:
                [
                    theLine::stringOp write:(theDate day) &paddingLeft:3 &with:" ".
                    
                    theDate := theDate add &days:1.
                ]         
                &until:[(theDate month != theMonth)or:[theDate dayOfWeek == 1]].
            ].
        
        #var(type:int) aLength := theLine length.
        (aLength < 21)
            ? [ theLine::stringOp write:" " &length:(21 - aLength). ].
            
        theRow += 1.            
    ]
    
    #method iterator = Iterator
    {
        available = theRow < 7.

        #method readIndex &vint:anIndex [ anIndex << theRow int. ]

        write &index:anIndex 
        [
            (anIndex <= theRow)
                ? [ self writeTitle. ].
                
            #loop (anIndex > theRow) ?
                [ self writeLine. ].
        ]

        get = self.
    }.
    
    #method printTitleTo : anOutput
    [
        anOutput::stringOp write:(MonthNames @(theMonth - 1)) &padding:21 &with:" ".
    ]
   
    #method printTo : anOutput
    [
        anOutput write:(theLine literal).
    ]
}

#class Calendar
{
    #field(type:int) theYear.
    #field(type:int) theRowLength.
    
    #constructor new : aYear
    [
        theYear := aYear int.
        theRowLength := 3.
    ]
    
    #method printTo:anOutput
    [
        anOutput::stringOp write:"[SNOOPY]" &padding:(theRowLength * 25) &with:" ". 
        anOutput writeLine.        
        anOutput::stringOp write:theYear &padding:(theRowLength * 25) &with:" ". 
        anOutput writeLine writeLine.
        
        #var(type:int) aRowCount := 12 / theRowLength.
        #var Months := Matrix new &n:aRowCount &m:theRowLength &every_ij: 
            (&index:i &index:j) [ CalendarMonthPrinter new &year:(theYear int) &month:((i * theRowLength + j + 1) int) ].

        control run:Months &forEach: aRow
        [
            control run:aRow &forEach: aMonth
            [
                aMonth printTitleTo:anOutput.
                    
                anOutput write:"   ".
            ].
            
            anOutput writeLine.
            
            control run:(ParallelEnumerator new:aRow) &forEach: aLine
            [
                control run:aLine &forEach: aPrinter
                [
                    aPrinter printTo:anOutput.

                    anOutput write:"   ".
                ].

                anOutput writeLine.
            ].
        ].        
    ]
}

// --- program ---

#symbol program =
[
    #var aCalender := Calendar new:(consoleEx write:"ENTER THE YEAR:" readLine:(Integer new)).
    
    aCalender printTo:consoleEx.
    
    consoleEx readChar.
].