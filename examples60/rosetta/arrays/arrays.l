/// basic array syntax

import system'collections;
import extensions;

const staticArray := new int[]{1, 2, 3};

public program()
{
    var array := Array.allocate(3);
    array[0] := 1;
    array[1] := 2;
    array[2] := 3;
    
    int stackAllocatedArray[3];
    stackAllocatedArray[0] := 1;
    stackAllocatedArray[1] := 2;
    stackAllocatedArray[2] := 3;            
    
    var dynamicArray := new system'collections'ArrayList();
    dynamicArray.append:1;
    dynamicArray.append:2;
    dynamicArray.append:4;
 
    dynamicArray[2] := 3;    
    
    console.writeLine(array[0]);
    console.writeLine(stackAllocatedArray[1]);
    console.writeLine(dynamicArray[2]);
}