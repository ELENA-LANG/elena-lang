// --- default namespaces ---
import system'io;
import system'routines;
import system'collections;
import extensions;
import extensions'routines;
import extensions'text;

// --- EField ---

extension fieldOp
{
    MessageName Unserialized
        = new MessageName(cast string(self));
}

// --- Record ---
class Record
{
    theName;
    theAge;
    thePhone;
    
    constructor()
    {
        theName := new StringWriter();
        theAge := Integer.new();
        thePhone := new StringWriter();
    }

    // !! temporal
    Name(v)
    {
        self.Name := v;
    }
    Age(v)
    {
        self.Age := v;
    }
    Phone(v)
    {
        self.Phone := v;
    }

    Name
    {
        get() = theName.Value;
        
        set(v)
        {
            v.convertTo(theName)
        }
    }

    Age
    {
        get() = theAge.Value;
        
        set(v)
        {
            v.convertTo(theAge)
        }
    }

    Phone
    {
        get() = thePhone.Value;
        
        set(v)
        {
            v.convertTo(thePhone)
        }
    }

    string toPrintable()
        = new StringWriter()
                .printLine("Name:",theName)
                .printLine("Age:",theAge)
                .printLine("Phone:",thePhone);
}

// --- Program ---

public program() 
{
    if (program_arguments.Length == 1)
        { console.write:"Please provide the path to the file to view"; AbortException.raise() };

    var db := new List().append(new Record());
    
    File.assign(program_arguments[1]).forEachLine:(line)
    {
        if (line.Length == 0)
        { 
            db.append(new Record()) 
        }
        else
        {
            // split record line                    
            var keyValue := line.splitBy:":";
            var name  := keyValue[0];
            var value := keyValue[1];

            // update field
            MessageName subj := name.Unserialized;
            
            subj.setPropertyValue(db.LastMember, value)
        }
    };
    
    // print the list
    db.forEach:printingLn;

    console.readChar()
}