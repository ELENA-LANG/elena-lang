#import system'text.
#import system'routines.
#import system'calendar.
#import extensions.
#import extensions'math.
#import extensions'routines.

// --- calendar ---

const MonthNames = ("JANUARY","FEBRUARY","MARCH","APRIL","MAY","JUNE","JULY","AUGUST","SEPTEMBER","OCTOBER","NOVEMBER","DECEMBER").
const DayNames = ("MO", "TU", "WE", "TH", "FR", "SA", "SU").

class CalendarMonthPrinter
{
    object theDate.
    object theLine.
    object theMonth.
    object theYear.
    object theRow.
    
    constructor new &year:aYear &month:aMonth
    [
        theMonth := aMonth.
        theYear := aYear.
        theLine := TextBuffer new.
        theRow := Integer new &int:0.
    ]

    writeTitle
    [        
        theRow set:0.
        theDate := Date new &year:(theYear int) &month:(theMonth int) &day:1.
        DayNames run &each: aName
            [ theLine writeLiteral:" ":aName ].
    ]
    
    writeLine
    [
        theLine clear.
        
        (theDate month == theMonth)
            ? [
                theLine write:"   " &length:(((theDate dayOfWeek) => 0 ? [ 7 ] ! [ theDate dayOfWeek ]) - 1).
                
                control do:
                [
                    theLine write:(theDate day literal) &paddingLeft:3 &with:#32.
                    
                    theDate := theDate add &days:1.
                ]         
                &until:[(theDate month != theMonth)or:[theDate dayOfWeek == 1]].
            ].
        
        int aLength := theLine length.
        (aLength < 21)
            ? [ theLine write:" " &length:(21 - aLength). ].
            
        theRow += 1.            
    ]
    
    iterator = Iterator
    {
        available = theRow < 7.

        readIndex &vint:anIndex [ anIndex set:theRow. ]

        write &index:anIndex 
        [
            (anIndex <= theRow)
                ? [ $owner writeTitle. ].
                
            while (anIndex > theRow)
                [ $owner writeLine. ].
        ]

        get = $owner.
    }.
    
    printTitleTo : anOutput
    [
        anOutput write:(MonthNames @(theMonth - 1)) &padding:21 &with:#32.
    ]
   
    printTo : anOutput
    [
        anOutput write:(theLine literal).
    ]
}

class Calendar
{
    int theYear.
    int theRowLength.
    
    constructor new : aYear
    [
        theYear := aYear int.
        theRowLength := 3.
    ]
    
    printTo:anOutput
    [
        anOutput write:"[SNOOPY]" &padding:(theRowLength * 25) &with:#32. 
        anOutput writeLine.        
        anOutput write:(theYear literal) &padding:(theRowLength * 25) &with:#32. 
        anOutput writeLine writeLine.
        
        var aRowCount := 12 / theRowLength.
        var Months := Array new &length:aRowCount set &every:(&index:i)
            [ Array new &length:theRowLength set &every:(&index:j)
                [ CalendarMonthPrinter new &year:(theYear int) &month:((i * theRowLength + j + 1) int) ]].
        
        Months run &each: aRow
        [
            aRow run &each: aMonth
            [
                aMonth printTitleTo:anOutput.
                    
                anOutput write:"   ".
            ].
            
            anOutput writeLine.
            
            ParallelEnumerator new:aRow run &each: aLine
            [
                aLine run &each: aPrinter
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

program =
[
    var aCalender := Calendar new:(console write:"ENTER THE YEAR:" readLine:(Integer new)).
    
    aCalender printTo:console.
    
    console readChar.
].