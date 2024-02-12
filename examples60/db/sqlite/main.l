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
        console.printLine(tableName,":");
        var table := self.executeQuery("SELECT * FROM " + tableName);
        
        // Header
        table.fields().forEach:(fieldName)
        {
            console.printPaddingRight(25, fieldName)
        };
        console.printLine();
        console.printPaddingRightChar(25 * table.fieldCount(), $45, "-");
        console.printLine();
        
        // Rows
        table.rows().forEach:(row)
        {
            table.fields().forEach:(fieldName)
            {
                console.printPaddingRight(25, row[fieldName])
            };
            console.printLine()
        };
        console.printLine();
    }
}

// --- program ---

public program()
{
    auto dbFile := File.assign(DBFileName);
    
    var cnn := DBConnection.new:dbFile;
    
    ifnot (dbFile.Available)
    {
        cnn.open();
  
        using (cnn)
        {
            console.write:"Database is being created";
            
            cnn.executeNonQuery:"CREATE Table Movies (Name TEXT, Director TEXT, Year INTEGER)";
            console.write:".";
                
            cnn.executeNonQuery:"INSERT INTO Movies (Name, Director, Year) VALUES ('The Dark Knight', 'Christopher Nolan', 2008)";
            cnn.executeNonQuery:"INSERT INTO Movies (Name, Director, Year) VALUES ('Cloverfield', 'Matt Reeves', 2008)";
            cnn.executeNonQuery:"INSERT INTO Movies (Name, Director, Year) VALUES ('Beverly Hills Chihuahua', 'Raja Gosnell', 2008)";
            console.write:".";
                
            cnn.executeNonQuery("CREATE TABLE [Users] " +
                                   "([ID] INTEGER  NOT NULL PRIMARY KEY AUTOINCREMENT, " +
                                   "[client] NVARCHAR(100)  NULL, " +
                                   "[date] DATE  NULL " +
                                   ")");
            console.write:".";
                                   
            cnn.executeNonQuery:"INSERT INTO Users (client, date) VALUES ('Alexandre', '2015-01-01')";
            cnn.executeNonQuery:"INSERT INTO Users (client, date) VALUES ('Alex', '2015-01-01')";
            console.write:".";
            
            console.writeLine:"Done"
        }                    
    };

    cnn.open();

    using (cnn)
    {
        console.printLine("Number of table in DB: ",cnn.numberOfTable());
    
        console.writeLine:"Tables:".writeLine();
        
        cnn.tables().forEach:(tableName)
        {
            cnn.printTable:tableName
        }
    };

    console.readChar()
}