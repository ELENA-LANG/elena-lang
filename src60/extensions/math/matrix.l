import system'routines;
import system'text;
import system'dynamic;

namespace math
{
   class MatrixException : Exception
   {
      constructor new(string s)
         <= super new(s);
   }

   internal singleton MatrixHelper
   {
      (RealMatrix, bool) getDecomposed(RealMatrix m)
      {
         // create matrix copy
         auto a := m.clone();
    
         real v := 0.0r;
         real t := 0.0r;
            
         a.readLengthsTo(out int n, out int m);
            
         if(n != m)
            { MatrixException.new("Invalid argument").raise() };
            
         // toggle tracks row swaps. +1 -> even, -1 -> odd. used by MatrixDeterminant
         bool even := true;
    
         // permutation matrix
         auto pm := new int[](n);
         pm.populate::(int i => i);
    
         // each column, j is counter for coulmns
         int  max_col := 0;
         real max := 0.0r;
            
         for (int j := 0; j < n; j += 1)
         {
            // find largest value in col j
            a.read(j, j, ref max);
            max := abs(max);
                
            max_col := j;
            for (int i := j + 1; i < n; i += 1)
            {
               a.read(i, j, ref v);
               v := abs(v);
               if(v > max)
               {
                  max := v;
                  max_col := i
               }
            };
      
            // if largest value not on pivot, swap rows
            if(max_col != j)
            {
               for(int k := 0; k < n; k += 1)
               {
                  a.read(j, k, ref t);
                  a.read(max_col, k, ref v);
    
                  a.setAt(j, k, v);
                  a.setAt(max_col, k, t);
               };
    
               // switch two rows in permutation matrix
               pm/*~intArrayOp*/.exchange(j, max_col);
    
               even := even.Inverted
            };
                            
            if(0.0r == max)
               { MatrixException.new("The matrix is singular!").raise() };
    
            a.read(j, j, ref t);
                
            for (int i := j + 1; i < n; i += 1)
            {
               a.read(i, j, ref v);
               v := v / t;
                    
               a.setAt(i, j, v);
    
               for (int k := j + 1; k < n; k += 1)
               {
                  a.read(i, j, ref v);
                  a.read(j, k, ref real c);
                  v := v * c;
    
                  a.read(i, k, ref c);
                  v := c - v;
                        
                  a.setAt(i, k, v)
               }
            };
         };
    
         ^ (a, even);
      }      
   }

   public extension matrixOp : RealMatrix
   {
      RealMatrix clone()
      {
         self.readLengthsTo(out int n, out int m);
            
         auto r := RealMatrix.allocate(n, m);
         for(int i := 0; i < n; i += 1)
         {
            for(int j := 0; j < m; j += 1)
            {
               self.read(i, j, ref real v);
                    
               r.setAt(i, j, v)
            }
         };
            
         ^ r
      }
        
      populate(Func<int,int,real> f)
      {
         self.readLengthsTo(out int n, out int m);
            
         for(int i := 0; i < n; i += 1)
         {
            for(int j := 0; j < m; j += 1)
            {
               self.setAt(i, j, f(i,j))
            }
         }    
      }
        
      RealMatrix add(RealMatrix b)
      {
         self.readLengthsTo(out int n, out int m);
         b.readLengthsTo(out int i, out int j);
            
         if:not(n == i)
            { MatrixException.new("Two matrices must have an equal number of rows and columns to be added").raise() };
            
         if:not(m == j)
            { MatrixException.new("Two matrices must have an equal number of rows and columns to be added").raise() };
                
         auto r := RealMatrix.allocate(n, m);
         for(int i := 0; i < n; i += 1)
         {
            for(int j := 0; j < m; j += 1)
            {
               self.read(i, j, ref real r1);
               b.read(i, j, ref real r2);
                    
               r.setAt(i, j, r1 + r2)
            }
         };
            
         ^ r
      }
        
      RealMatrix subtract(RealMatrix b)
      {
         self.readLengthsTo(out int n, out int m);
         b.readLengthsTo(out int i, out int j);
            
         if:not(n == i)
            { MatrixException.new("Two matrices must have an equal number of rows and columns to be added").raise() };
            
         if:not(m == j)
            { MatrixException.new("Two matrices must have an equal number of rows and columns to be added").raise() };
                
         auto r := RealMatrix.allocate(n, m);
         for(int i := 0; i < n; i += 1)
         {
            for(int j := 0; j < m; j += 1)
            {
               self.read(i, j, ref real r1);
               b.read(i, j, ref real r2);
                    
               r.setAt(i, j, r1 - r2)
            }
         };
            
         ^ r
      }    
        
      RealMatrix multiply(RealMatrix b)
      {
         self.readLengthsTo(out int n, out int j);
         b.readLengthsTo(out int m, out int p);
                    
         if:not(j == m)
            { MatrixException.new("Invalid argument").raise() };
    
         auto r := RealMatrix.allocate(n, p);
         for(int i := 0; i < n; i += 1)
         {
            for(int j := 0; j < p; j += 1)
            {
               real v := 0.0r;
               for(int k := 0; k < m; k += 1)
               {
                  self.read(i, k, ref real r1);
                  b.read(k, j, ref real r2);
                        
                  r1 := r1 * r2;
                  v := v + r1;
               };
                    
               r.setAt(i, j, v)
            }
         };
            
         ^ r
      }
            
      get real Determinant()
      {
         (RealMatrix a, bool even) := MatrixHelper.getDecomposed(cast RealMatrix(self));
         real deter := 1.0r;
         real v := 0.0r;
    
         if:not(even)
            { deter := deter * (-1.0r) };
            
         a.readLengthsTo(out int n, out int m);
            
         for (int i := 0; i < n; i += 1)
         {
            a.read(i, i, ref v);
                
            deter := deter * v
         };
                
         ^ deter
      }
                
      string toString()
      {
         auto output := new TextBuilder();
            
         self.readLengthsTo(out int n, out int m);
            
         for(int i := 0; i < n; i += 1)
         {
            for(int j := 0; j < m; j += 1)
            {
               self.read(i, j, out real r);
                    
               output.print(r,",");
            };
            int l := output.Length;
            output.Length := l - 1;
                
            output.write(NewLineConstant);
         };
            
         ^ output.Value
      }
   }   
}