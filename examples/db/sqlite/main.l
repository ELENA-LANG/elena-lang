#define system.
#define system'routines.
#define system'io.
#define extensions.
#define sqlite.

#symbol(const) DBFileName = "data.db".

// --- dbTestOp ---

#class(extension)dbTestOp
{
    #method printTable : aTableName
    [
        console writeLine:aTableName:":".
        #var aTable := self executeQuery:("SELECT * FROM " + aTableName).
        
        // Header
        aTable fields run &each:aFieldName
        [
            console write:aFieldName &paddingRight:25.
        ].
        console writeLine.
        console write:"-" &paddingRight:(25 * aTable fieldCount) &with:#45.
        console writeLine.
        
        // Rows
        aTable rows run &each:aRow
        [
            aTable fields run &each:aFieldName
            [
                console write:(aRow@aFieldName) &paddingRight:25.
            ].
            console writeLine.
        ].
        console writeLine.
    ]
}

// --- program ---

#symbol program =
[
    #var dbFile := File new &path:DBFileName.
    
    #var cnn := DBConnection new &path:dbFile.
    
    (dbFile is &available)!
    [
        cnn open.
        
        console write:"Database is being created".
        
        cnn executeNonQuery:"CREATE Table Movies (Name TEXT, Director TEXT, Year INTEGER)".
        console write:".".
            
        cnn executeNonQuery:"INSERT INTO Movies (Name, Director, Year) VALUES ('The Dark Knight', 'Christopher Nolan', 2008)".
        cnn executeNonQuery:"INSERT INTO Movies (Name, Director, Year) VALUES ('Cloverfield', 'Matt Reeves', 2008)".
        cnn executeNonQuery:"INSERT INTO Movies (Name, Director, Year) VALUES ('Beverly Hills Chihuahua', 'Raja Gosnell', 2008)".
        console write:".".

        cnn executeNonQuery:("CREATE TABLE [Users] " +
                               "([ID] INTEGER  NOT NULL PRIMARY KEY AUTOINCREMENT, " +
                               "[client] NVARCHAR(100)  NULL, " +
                               "[date] DATE  NULL " +
                               ")").
        console write:".".
                               
        cnn executeNonQuery:"INSERT INTO Users (client, date) VALUES ('Alexandre', '2015-01-01')".
        cnn executeNonQuery:"INSERT INTO Users (client, date) VALUES ('Alex', '2015-01-01')".        
        console write:".".
        
        cnn close.        
        console writeLine:"Done".
    ].

    cnn open.

    console writeLine:"Number of table in DB: ":(cnn numberOfTable).

    console writeLine:"Tables:" writeLine.
    
    cnn tables run &each: aTableName 
        [ 
            cnn printTable:aTableName.
        ].

    cnn close.

    console readChar.
].