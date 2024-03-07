import extensions;
import system'routines;
import extensions'threading;
import system'threading;
 
static sync = new object();
 
extension op
{
    sleepSort()
    {
        self.forEach::(n)
        {
            threadControl.start(
            {
                threadControl.sleep(1000 * n);
 
                lock(sync)
                {
                    console.printLine(n)
                }
            })
        }
    }
}
 
public program()
{
//    /*program_arguments.skipping(1)*/"1 2".selectBy(mssgconst toInt<intConvertOp>[1]).toArray().sleepSort();
 
            threadControl.start(
            {
                threadControl.sleep(1000);
 
                lock(sync)
                {
                    console.printLine(1)
                }
            });

    console.readChar()
}