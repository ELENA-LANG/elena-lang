import system'routines;
import system'collections;

// --- DBRow ---

public sealed class DBRow
{
   object[] _row;
   DBTable  _table;
   
   internal constructor newTableRow(object[] row, DBTable table)
   {
      _row := row;
      _table := table
   }
   
   at(int n)
      = _row.at(n);
   
   at(string fieldName)
   {
      int index := _table.indexOfField(fieldName);
                     
      if (-1 == index)
         { DBException.new("Field is unknown").raise() };
         
      ^ _row[index]
   }
}

// --- DBTable ---

public sealed class DBTable
{
   string[]     _fieldNames;
   List<DBRow>  _rows;
   
   constructor new(string[] names)
   {
      _fieldNames := names;
      _rows := new List<DBRow>()
   }
   
   /*internal*/ appendRow(object[] row)
   {
      _rows.append(DBRow.newTableRow(row, self))
   }
   
   internal int indexOfField(string fieldName)
   {
      ^ _fieldNames.indexOfElement(fieldName)
   }
   
   DBRow[] rows() = _rows.Value;
   
   string[] fields() = _fieldNames;
   
   int fieldCount() = _fieldNames.Length;
}