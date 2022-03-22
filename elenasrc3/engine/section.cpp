//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains the body of ELENA Engine Data Section
//		classes.
//                                                  (C)2021, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "section.h"

using namespace elena_lang;

Section* Section :: readSection(StreamReader& reader)
{
   Section* section = new Section();

   pos_t length = reader.getPos();
   section->load(reader, length);

   MapHelper::readReferenceMap<RelocationMap,pos_t>(&reader, section->_references);

   return section;
}

void Section :: writeSection(Section* section, StreamWriter& writer)
{
   pos_t length = section->length();
   writer.writePos(length);
   writer.write(section->_buffer, length);

   MapHelper::writeReferenceMap<RelocationMap,pos_t>(&writer, section->_references);
}
