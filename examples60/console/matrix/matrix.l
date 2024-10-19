// based on http://cboard.cprogramming.com/cplusplus-programming/30001-determinant-calculation.html prelude sample

// --- default namespaces ---
import extensions;
import extensions'math;
import system'routines;

// --- factory ---

class Matrix
{
    RealMatrix theMatrix;
    
    constructor new(RealMatrix matrix)
    {
        theMatrix := matrix
    }
    
    constructor create(int n, string name)
        <= new (RealMatrix.allocate(n,n).populate::(int i,int j)
            { 
                ^ Console.print("Enter ",name,"[",i,",",j,"]=").readLine().toReal()
            });
    
    constructor newIdentityOf(int n)
        <= new(RealMatrix.allocate(n,n).populate::(int i, int j =>
            cast RealNumber((i == j) ? 1.0r : 0.0r )));
    
    add(matrix)
        = Matrix.new(theMatrix.add(cast RealMatrix(matrix)));

    subtract(matrix)
        = Matrix.new(theMatrix.subtract(cast RealMatrix(matrix)));

    multiply(matrix)
        = Matrix.new(theMatrix.multiply(cast RealMatrix(matrix)));
    
    Determinant
        = theMatrix.Determinant;
    
    dispatch() => theMatrix;
}

// ---- Program ---

public singleton Control
{
    Prompt = "Choose an option
[1] - Sum Matrix
[2] - Subtract Matrix
[3] - Multiply Matrix
[4] - Matrix Determinant
[5] - Identity Matrix
[6] - Exit
Your choice:";
    
    proceed(choice)
    {
        choice =>
            "1" : {
                Console.write("Enter the order of matrix: ");
                var N := Console.readLine().toInt();
                
                var matrixA := Matrix.create(N, "A");
                
                Console.writeLine();
                
                var matrixB := Matrix.create(N,"B");
                
                Console.writeLine().writeLine("Output:");

                RealMatrix sum := matrixA + matrixB;

                ^ sum.toString()
            }
            "2" : {
                Console.write("Enter the order of matrix: ");
                var N := Console.readLine().toInt();
                
                var matrixA := Matrix.create(N, "A");
                
                Console.writeLine();
                
                var matrixB := Matrix.create(N,"B");
                
                Console.writeLine().writeLine("Output:");

                RealMatrix diff := matrixA - matrixB;

                ^ diff.toString()
            }
            "3" : {
                Console.write("Enter the order of matrix: ");
                var N := Console.readLine().toInt();
                
                var matrixA := Matrix.create(N, "A");
                
                Console.writeLine();
                
                var matrixB := Matrix.create(N,"B");
                
                Console.writeLine().writeLine("Output:");

                RealMatrix product := matrixA * matrixB;

                ^ product.toString()
            }
            "4" : {
                Console.write("Enter the order of matrix: ");
                var N := Console.readLine().toInt();
                
                var matrixA := Matrix.create(N, "A");
                                
                Console.writeLine().writeLine("Output:");

                ^ matrixA.Determinant
            }
            "5" : {
                Console.write("Enter the order of matrix: ");
                var N := Console.readLine().toInt();
                
                RealMatrix identity := Matrix.newIdentityOf(N);

                ^ identity.toString()
            }
            "6" : {
                forward program.stop()
            }
    }
}
