
public interface Space
{
   int LiveCell(int k, int l, int state)
   {
      int j := -1;
      int counter := 0;
      for (int i := -1; i < 2; i += 1)
      {
            for (int j := -1; j < 2; j += 1)
            {
                int cell := self.at(k + i, l + j);
                if (cell == state)
                    { counter := counter + 1 }
            }
      };
        
      ^ counter
   }        
    
   abstract int Rows
   {
      /// Returns the space actual number of rows
      get();
   }
    
   abstract int Columns
   {
      /// Returns the space actual number of columns
      get();
   }
    
   abstract update(RuleSet rs);
    
   abstract int at(int i, int j);
}

public sealed class IntMatrixSpace : Space
{
   IntMatrix _matrix;
   int       _n;
   int       _m;
    
   constructor allocate(int n, int m)
   {
      _n := n;
      _m := m;
        
      _matrix := IntMatrix.allocate(n, m);

      for (int i := 0; i < _n; i+=1)
      {
         for (int j := 0; j < _m; j+=1)
         {
            _matrix.setAt(i, j, 0)
         }
      }
   }

   constructor allocate(int n, int m, RuleSet rs)
      <= allocate(n, m)
   {
      self.internalUpdate(rs, _matrix)
   }

   private internalUpdate(RuleSet rs, IntMatrix matrix)
   {
      for (int i := 0; i < _n; i += 1)
      {
         for(int j := 0; j < _m; j += 1)            
         {
            int cell := rs.proceed(self, i, j);
                    
            matrix.setAt(i, j, cell)
         }
      }
   }

   update(RuleSet rs)
   {
      IntMatrix newMatrix := IntMatrix.allocate(_n, _m);
        
      self.internalUpdate(rs, newMatrix);
        
      _matrix := newMatrix
   }

   int at(int k, int l)
   {
      int i := k;
      if(i < 0)
      {
         i := _n + i
      };
        
      if(i >= _n)
      {
         i := i.mod(_n)
      };
        
      int j := l;
      if(j < 0)
      {
         j := _m + j
      };
      if(j >= _m)
      {
         j := j.mod(_m)
      };
      
      ^ _matrix.at(i, j)
   }
    
   int Rows
   {
      get()
      {
         ^ _n
      }
   }
    
   int Columns
   {
      get()
      {
         ^ _m
      }
   }
}
