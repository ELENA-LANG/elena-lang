import system'collections;
import system'routines;
import extensions;
import extensions'routines;
import extensions'text;
 
class Employee
{
    string Name       : prop;
    string ID         : prop;
    int    Salary     : prop;
    string Department : prop;
 
    string toPrintable()
        = new StringWriter()
            .writePaddingRight(Name, 25)
            .writePaddingRight(ID, 12)
            .writePaddingRight(Salary.toPrintable(), 12)
            .write(Department);
}
 
extension reportOp
{
    topNPerDepartment(n)
        = self.groupBy::(x => x.Department ).selectBy::(x)
        {
            ^ new {
                Department = x.Key;
 
                Employees
                    = x.orderBy::(f,l => f.Salary > l.Salary ).top(n).summarize(new ArrayList());
            }
        };
}
 
public program()
{
    var employees := new Employee[]
    {
        new Employee{ this Name := "Tyler Bennett"; this ID := "E10297"; this Salary:=32000; this Department:="D101";},
        new Employee{ this Name := "John Rappl"; this ID := "E21437"; this Salary:=47000; this Department:="D050";},
        new Employee{ this Name := "George Woltman"; this ID := "E00127"; this Salary:=53500; this Department:="D101";},
        new Employee{ this Name := "Adam Smith"; this ID := "E63535"; this Salary:=18000; this Department:="D202";},
        new Employee{ this Name := "Claire Buckman"; this ID := "E39876"; this Salary:=27800; this Department:="D202";},
        new Employee{ this Name := "David McClellan"; this ID := "E04242"; this Salary:=41500; this Department:="D101";},
        new Employee{ this Name := "Rich Holcomb"; this ID := "E01234"; this Salary:=49500; this Department:="D202";},
        new Employee{ this Name := "Nathan Adams"; this ID := "E41298"; this Salary:=21900; this Department:="D050";},
        new Employee{ this Name := "Richard Potter"; this ID := "E43128"; this Salary:=15900; this Department:="D101";},
        new Employee{ this Name := "David Motsinger"; this ID := "E27002"; this Salary:=19250; this Department:="D202";},
        new Employee{ this Name := "Tim Sampair"; this ID := "E03033"; this Salary:=27000; this Department:="D101";},
        new Employee{ this Name := "Kim Arlich"; this ID := "E10001"; this Salary:=57000; this Department:="D190";},
        new Employee{ this Name := "Timothy Grove"; this ID := "E16398"; this Salary:=29900; this Department:="D190";}
    };
 
    employees.topNPerDepartment(2).forEach::(info)
    {
        Console.printLine("Department: ",info.Department);
 
        info.Employees.forEach(PrintingLn);
 
        Console.writeLine("---------------------------------------------")
    };
 
    Console.readChar()
}