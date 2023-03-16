import extensions;

//const MonthNames = new string[]{"JANUARY","FEBRUARY","MARCH","APRIL","MAY","JUNE","JULY","AUGUST","SEPTEMBER","OCTOBER","NOVEMBER","DECEMBER"};

public template xtry::catch(try_block,catch_block) : __included(statementTemplates)
{
   try_block \\ on(catch_block);
}


public program()
{
   xtry
   {
      forward program();
   }
   catch(Exception err) {
     console.writeLine(err)
   }
   
}
  