// --- auxiliary code to simulate UPPERCASE code ---

import extensions;
import system'text;
import system'routines;
import system'calendar;
import extensions'routines;

public program = PROGRAM;

CONSOLE = Console;

abstract INDEXERCLASS : interface<Indexer>
{ 
   abstract ISAVAILABLE(); 
   abstract GETINDEX();
   abstract SETINDEX(INDEX);
   abstract NEWINDEX(OFFS);
   abstract GETVALUE();
   abstract GETLENGTH();
   
   bool Available = ISAVAILABLE();   
   int Index
   {
      get() = GETINDEX();
         
      set(int INDEX)
      {
         SETINDEX(INDEX);
      }
   }      
   appendIndex(int OFFS) { this self.Index := NEWINDEX(OFFS) }   
   set Value(o) { NotSupportedException.raise() }
   get Value() = GETVALUE();
   get int Length() { ^ GETLENGTH() }
}

PARALLELENUMERATOR(N) { ^ ParallelEnumerator.new(N) }

NEWDATE(Y,M,D) { ^ Date.new(Y,M,D) }
NEWTEXTBUILDER() { ^ new TextBuilder() }

GETMONTH(D) { ^ D.Month }
GETDAYOFWEEK(D) { ^D.DayOfWeek }
GETDAYSTR(D) { ^ D.Day.toPrintable() }

READCHAR() { Console.readChar() }
READINT(PROMPT) { ^ Console.write(PROMPT).readLine().toInt() }

WRITEPADDING(OUTPUT,S,SPACER,LEN) { OUTPUT.writePadding(S,SPACER,LEN) }
WRITEPADDINGLEFT(OUTPUT,S,SPACER,LEN) { OUTPUT.writePaddingLeft(S,SPACER,LEN) }
WRITELINE(OUTPUT) { OUTPUT.writeLine() }
WRITE(OUTPUT,S) { OUTPUT.write(S) }
WRITEARGS(OUTPUT,S1,S2) { OUTPUT.print(S1, S2) }
WRITECOPIES(OUTPUT,S,N) { OUTPUT.writeCopies(S,N) }

ALLOCATE(COUNT) { ^ Array.allocate(COUNT) }

CLEAR(S) { S.clear() }

POPULATE(OBJ, ACTION) { ^ OBJ.populate(ACTION) }
FOREACH(OBJ, ACTION) { ^ OBJ.forEach(ACTION) }

// --- calendar ---

const MONTHNAMES = new string[]{"JANUARY","FEBRUARY","MARCH","APRIL","MAY","JUNE","JULY","AUGUST","SEPTEMBER","OCTOBER","NOVEMBER","DECEMBER"};
const DAYNAMES = new string[]{"MO", "TU", "WE", "TH", "FR", "SA", "SU"};

// --- UPPERCASE CODE --

CALENDARMONTHPRINTER
{
   _DATE;
   _LINE;
   _MONTH;
   _YEAR;
   _ROW;
    
   constructor NEW(YEAR, MONTH)
   {
      _MONTH := MONTH;
      _YEAR := YEAR;
      _LINE := NEWTEXTBUILDER();
      _ROW := 0;
   }

   WRITETITLE()
   {
      _ROW := 0;
      _DATE := NEWDATE(_YEAR, _MONTH, 1);

      FOREACH(DAYNAMES, (NAME)
         { WRITEARGS(_LINE, " ", NAME) })
   }
    
   WRITECALLINE()
   {
      CLEAR(_LINE);

      if (GETMONTH(_DATE) == _MONTH)
      {
         WRITECOPIES(_LINE, "   ",(GETDAYOFWEEK(_DATE) == 0) ? 6 : (GETDAYOFWEEK(_DATE) - 1));

         do
         {
            WRITEPADDINGLEFT(_LINE, GETDAYSTR(_DATE), $32, 3);

            _DATE := _DATE.addDays(1)
         }
         until(GETMONTH(_DATE) != _MONTH || GETDAYOFWEEK(_DATE) == 1)
      };

      int LENGTH := _LINE.Length;
      if (LENGTH < 21)
         { WRITECOPIES(_LINE, " ", 21 - LENGTH) };

      _ROW += 1
   }
    
   indexer() = new INDEXERCLASS
   {
      ISAVAILABLE() = _ROW < 7;

      GETINDEX()= _ROW;
      
      SETINDEX(INDEX)
      {
         if (INDEX <= _ROW)
            { WRITETITLE() };
                 
         while (INDEX > _ROW)
            { WRITECALLINE() }
      }

      NEWINDEX(OFFS)
      {
         ^ _ROW + OFFS
      }
        
      GETLENGTH() { ^ 7 }

      GETVALUE() = self;              
   };
    
   PRINTTITLETO(OUTPUT)
   {
      WRITEPADDING(OUTPUT, MONTHNAMES[_MONTH - 1], $32, 21)
   }
   
   PRINTTO(OUTPUT)
   {
      WRITE(OUTPUT, *_LINE)
   }
}

CALENDARCLASS
{
   _YEAR;
   _ROWLENGTH;
    
   constructor NEW(YEAR)
   {
      _YEAR := YEAR;
      _ROWLENGTH := 3
   }
    
   PRINTTO(OUTPUT)
   {
      WRITEPADDING(OUTPUT, "[SNOOPY]", $32, _ROWLENGTH * 25); 
      WRITELINE(OUTPUT);
      WRITEPADDING(OUTPUT, _YEAR.toPrintable(), $32, _ROWLENGTH * 25); 
      WRITELINE(OUTPUT); WRITELINE(OUTPUT);
        
      var ROWCOUNT := 12 / _ROWLENGTH;
      var MONTHS := POPULATE(ALLOCATE(ROWCOUNT), (I =>
            POPULATE(ALLOCATE(_ROWLENGTH), (J =>
                     CALENDARMONTHPRINTER.NEW(_YEAR, I * _ROWLENGTH + J + 1)))));
        
      FOREACH(MONTHS, (ROW)
      {
         FOREACH(ROW, (MONTH)
         {
            MONTH.PRINTTITLETO(OUTPUT);
                    
            WRITE(OUTPUT, "   ")
         });
            
         WRITELINE(OUTPUT);
            
         FOREACH(PARALLELENUMERATOR(ROW), (LINE)
         {
            FOREACH(LINE, (PRINTER)
            {
               PRINTER.PRINTTO(OUTPUT);

               WRITE(OUTPUT, "   ")
            });

            WRITELINE(OUTPUT);
         })
      })
   }
}

// --- PROGRAM ---

PROGRAM()
{
   CALENDARCLASS CALENDER := CALENDARCLASS.NEW(READINT("ENTER THE YEAR:"));
    
   CALENDER.PRINTTO(CONSOLE);

   READCHAR();
}