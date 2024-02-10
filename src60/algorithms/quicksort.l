import system'routines;
import system'collections;

const int MAX_LEVELS = 64;

public singleton QuickSortAlgorithm
{
   sort(SortingAgent source, int start, int length, Func2 compf)
   {
      int beg[MAX_LEVELS];
      int end[MAX_LEVELS];

      beg[0] := start;
      end[0] := length;

      int i := 0;
      while (i >= 0) {
         int L := beg[i];
         int R := end[i];
         if (R > L + 1) {
            R--;

            var piv := source[L];
            while (L < R) {
               while ((!compf(source[R], piv)) && L < R)
                  R--;

               if (L < R) {
                  source[L] := source[R];
                  L++;                  
               };

               while (compf(source[L], piv) && L < R)
                  L++;

               if (L < R) {
                  source[R] := source[L];
                  R--;                  
               }                  
            };
            source[L] := piv;
            if (L - beg[i] > end[i] - R) {
               beg[i + 1] := L + 1;
               end[i + 1] := end[i];
               end[i] := L;
               i++;
            }
            else {
               beg[i + 1] := beg[i];
               end[i + 1] := L;
               beg[i] := L + 1;
               i++;
            };            
         }         
         else 
         {
            i--;
         }
      }
   }
}
