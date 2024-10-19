import system'routines;
import system'io;
import extensions;
import sqlite;

const DBFileName = "data.db";

// --- dbTestOp ---

extension dbTestOp
{
    printTable(tableName)
    {
        Console.printLine(tableName,":");
        var table := self.executeQuery("SELECT * FROM " + tableName);
        
        // Header
        table.fields().forEach::(fieldName)
        {
            Console.printPaddingRight(25, fieldName)
        };
        Console.printLine();
        Console.printPaddingRightChar(25 * table.fieldCount(), $45, "-");
        Console.printLine();
        
        // Rows
        table.rows().forEach::(row)
        {
            table.fields().forEach::(fieldName)
            {
                Console.printPaddingRight(25, row[fieldName])
            };
            Console.printLine()
        };
        Console.printLine();
    }
}

// --- program ---

public program()
{
    auto dbFile := File.assign(DBFileName);
    
    var cnn := DBConnection.new(dbFile);
    
    ifnot (dbFile.Available)
    {
        cnn.open();
  
        using (cnn)
        {
            Console.write("Database is being created");
            
            cnn.executeNonQuery("CREATE Table Movies (Name TEXT, Director TEXT, Year INTEGER)");
            Console.write(".");
                
            cnn.executeNonQuery("INSERT INTO Movies (Name, Director, Year) VALUES ('The Dark Knight', 'Christopher Nolan', 2008)");
            cnn.executeNonQuery("INSERT INTO Movies (Name, Director, Year) VALUES ('Cloverfield', 'Matt Reeves', 2008)");
            cnn.executeNonQuery("INSERT INTO Movies (Name, Director, Year) VALUES ('Beverly Hills Chihuahua', 'Raja Gosnell', 2008)");
            Console.write(".");
                
            cnn.executeNonQuery("CREATE TABLE [Users] " +
                                   "([ID] INTEGER  NOT NULL PRIMARY KEY AUTOINCREMENT, " +
                                   "[client] NVARCHAR(100)  NULL, " +
                                   "[date] DATE  NULL " +
                                   ")");
            Console.write(".");
                                   
            cnn.executeNonQuery("INSERT INTO Users (client, date) VALUES ('Alexandre', '2015-01-01')");
            cnn.executeNonQuery("INSERT INTO Users (client, date) VALUES ('Alex', '2015-01-01')");
            Console.write(".");
            
            Console.writeLine("Done")
        }                    
    };

    cnn.open();

    using (cnn)
    {
        Console.printLine("Number of table in DB: ",cnn.numberOfTable());
    
        Console.writeLine("Tables:").writeLine();
        
        cnn.tables().forEach::(tableName)
        {
            cnn.printTable(tableName)
        }
    };

    Console.readChar()
}