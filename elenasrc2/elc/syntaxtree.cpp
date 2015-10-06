//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Engine Syntax Tree class implementation
//
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "syntaxtree.h"

using namespace _ELENA_;

// --- SyntaxWriter ---

void SyntaxWriter :: newNode(LexicalType type, ref_t argument)
{
   // writer node
   _writer.writeDWord(type);
   _writer.writeDWord(argument);
}

void SyntaxWriter :: closeNode()
{
   _writer.writeDWord(-1);
}

// --- SyntaxReader::Node ---

SyntaxReader::Node :: Node(SyntaxReader* reader, size_t position, LexicalType type, ref_t argument)
{
   this->reader = reader;
   this->position = position;

   this->type = type;
   this->argument = argument;
}

// --- SyntaxReader ---

SyntaxReader::Node SyntaxReader::Root()
{
   _reader.seek(0);

   LexicalType type = (LexicalType)_reader.getDWord();
   ref_t arg = _reader.getDWord();

   return Node(this, _reader.Position(), type, arg);
}