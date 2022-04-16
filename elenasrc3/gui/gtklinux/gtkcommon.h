//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     GTK Common Header File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef GTKCOMMON_H
#define GTKCOMMON_H

#include <gtkmm.h>
#include "guicommon.h"

namespace elena_lang
{
   // --- Color ---
   struct Color
   {
      double red;
      double green;
      double blue;
      double alpha;

      bool operator == (Color& color)
      {
         if (red == color.red && green == color.green && blue == color.blue && alpha == color.alpha) {
            return true;
         }
         else return false;
      }

      bool operator != (Color& color)
      {
         if (red != color.red || green != color.green || blue != color.blue || alpha != color.alpha) {
            return true;
         }
         else return false;
      }

      void set(double red, double green, double blue)
      {
         this->red = red;
         this->green = green;
         this->blue = blue;
         this->alpha = 1.0;
      }

      void set(double red, double green, double blue, double alpha)
      {
         this->red = red;
         this->green = green;
         this->blue = blue;
         this->alpha = alpha;
      }

      Color()
      {
         set(0, 0, 0);
      }
      Color(double red, double green, double blue)
      {
         set(red, green, blue);
      }

      Color(double red, double green, double blue, double alpha)
      {
         set(red, green, blue, alpha);
      }
   };

}

#endif // GTKCOMMON_H
