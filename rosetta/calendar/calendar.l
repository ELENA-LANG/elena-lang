#define system.
#define system'text.
#define system'calendar.
#define extensions.
#define extensions'math.
#define extensions'routines.

// --- calendar ---

#symbol MonthNames = ("JANUARY","FEBRUARY","MARCH","APRIL","MAY","JUNE","JULY","AUGUST","SEPTEMBER","OCTOBER","NOVEMBER","DECEMBER").

#class CalendarMonthPrinter
{
    #field theDate.
    #field theLine.
    #field theMonth.
    #field theYear.
    
    #constructor new &Year:aYear &Month:aMonth
    [
        theMonth := aMonth.
        theYear := aYear.
        theLine := TextBuffer new.
    ]
    
    #method firstLine : aDay
    [        
        theDate := Date new &Year:theYear &Month:theMonth &Day:aDay.
        theLine append:(MonthNames @(theDate Month - 1)).
    ]
    
    #method nextLine
        = (theDate Month != theMonth) 
            ? [ false ]
            ! [
                theLine clear.
                
                control do:
                [
                    theLine append:" " append:(theDate Day Literal).
                    
                    theDate := theDate add &Days:1.
                ]         
                &until:[(theDate Month != theMonth)or:[theDate DayOfWeek == 0]].
                
                ^ true.
            ].
    
    #method Enumerator =
    {
        set &type'index:anIndex [ self firstLine:(anIndex + 1) ]
        
        next [ ^ self nextLine. ]
        
        get = self.
    }.
    
    #method printTo : anOutput
    [
        anOutput write:(theLine Literal).
    ]
}

#class Calendar
{
    #field theYear.
    #field theRowLength.
    
    #constructor new : aYear
    [
        theYear := aYear type'int.
        theRowLength := 3.
    ]
    
    #method printTo:anOutput
    [
        #var aRowCount := 12 / theRowLength.
        #var Months := matrixControl new &m:aRowCount &n:theRowLength &function: &&:i:j [ CalendarMonthPrinter new &Year:theYear &Month:(i * theRowLength + j + 1) ].
        
        control foreach:Months &do: &&:aRow
        [
            controlEx for:(ParallelEnumerator new:aRow) &do: &&:aLine
            [
                control foreach:aLine &do: &&:aPrinter
                [
                    aPrinter printTo:anOutput.
                ].
                
                anOutput writeLine.
            ]
        ].
        
    ]
}

// --- program ---

#symbol program =
[
    #var aCalender := Calendar new:1969.
    
    aCalender printTo:console.
].