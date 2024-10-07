import extensions;

// --- outputOp ---

static sync = new object();

public extension outputConcurrentOp
{
   printLineConcurrent(params object[] list)
   {
      lock(sync) {
         self.printLine(params list);
      }
   }
}