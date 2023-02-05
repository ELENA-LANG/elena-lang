/*
A+B - in programming contests, classic problem, which is given so contestants can gain familiarity with online judging system being used.

A+B is one of few problems on contests, which traditionally lacks fabula.

Problem statement
Given 2 integer numbers, A and B. One needs to find their sum.

    Input data
    Two integer numbers are written in the input stream, separated by space. 
    (-1000 <= A,B <= +1000) 

    Output data
    The required output is one integer: the sum of A and B. 
*/

import system'routines;
import extensions;

public program()
{
    console.printLine(console.readLine()
                              .split()
                              .selectBy(mssgconst toInt<convertorOp>[1])
                              .summarize())
}

public program_alt()
{
    var A := Integer.new();
    var B := Integer.new();
 
    console.loadLine(A,B).printLine(A + B)
}