static sync = new object();

public program()
{
   sync.test();

   lock(sync) {
   }
}
