//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     IDE View class body File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "view.h"

using namespace elena_lang;

// --- ViewStyles ---

void ViewStyles :: assign(pos_t count, StyleInfo* styles, int lineHeight, int marginWidth, FontFactoryBase* factory)
{
   this->count = count;
   this->items = new Style[count];
   this->marginWidth = marginWidth;
   this->lineHeight = lineHeight;

   for(size_t i = 0; i < count; i++) {
      FontBase* font = factory->createFont(styles[i].faceName, styles[i].size, styles[i].characterSet,
         styles[i].bold, styles[i].italic);

      this->items[i] = Style(styles[i].foreground, styles[i].background, font);
   }
}

void ViewStyles :: release()
{
   if (count > 0)
      delete[] items;

   count = 0;
}

// --- TextViewModel ---

void TextViewModel :: setStyles(pos_t count, StyleInfo* styleInfos, int lineHeight, int marginWidth, FontFactoryBase* factory)
{
   styles.release();
   styles.assign(count, styleInfos, lineHeight, marginWidth, factory);
   changed = true;
   lineNumbersVisible = false;
}

TextViewModel :: TextViewModel()
{
   changed = true;
   fontSize = 10; // !! temporal
   docView = nullptr;
}

TextViewModel :: ~TextViewModel()
{
   styles.release();
}
