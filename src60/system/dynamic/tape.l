import system;

const /*object[]*/ commands = new object[]{
    invoke0,invoke1,invoke2,invoke3,invoke4,invoke5,invoke6,invoke7,invoke8,invoke9,
    invoke10,invoke11,invoke12,invoke13,invoke14,invoke15,invoke16,invoke17,invoke18,invoke19,
    invoke20,invoke21,invoke22,invoke23,invoke24,invoke25,invoke26,invoke27,invoke28,invoke29,
    invoke30,invoke31,invokeV1,loadString,loadTerminator};

public singleton StreamTapeInterpreter
{
   build(byte[] buffer, int len) : external(system'core_routines'__tape_eval);
}