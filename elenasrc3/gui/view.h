//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     IDE View class header File
//                                              (C)2021, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef VIEW_H
#define VIEW_H

#include "guieditor.h"

namespace elena_lang
{
   // --- StyleInfo ---

   struct StyleInfo
   {
      Color  foreground;
      Color  background;
      wstr_t faceName;
      int    characterSet;
      int    size;
      bool   bold;
      bool   italic;
   };

   // --- ViewStyles ---
   struct ViewStyles
   {
      Style* items;
      pos_t  count;

      int    marginWidth;
      int    lineHeight;

      void assign(pos_t count, StyleInfo* styles, int lineHeight, int marginWidth, FontFactoryBase* factory);
      void release();

      ViewStyles()
      {
         lineHeight = marginWidth = 0;
         items = nullptr;
         count = 0;
      }
   };

   // --- TextViewModel ---
   class TextViewModel : public TextViewModelBase
   {
      ViewStyles    styles;

   public:
      bool          changed;
      int           fontSize;

      Style* getStyle(pos_t index) const override
      {
         return &styles.items[index];
      }

      int getMarginWidth() const override
      {
         return styles.marginWidth;
      }

      int getLineHeight() const override
      {
         return styles.lineHeight;
      }

      bool isAssigned() const override { return docView != nullptr; }

      void resize(Point size) override
      {
         if (docView)
            docView->resize(size);
      }

      void validate(CanvasBase* canvas) override
      {
         for (pos_t i = 0; i < styles.count; i++) {
            canvas->validateStyle(&styles.items[i]);

            if (styles.items[i].valid && styles.lineHeight < styles.items[i].lineHeight)
               styles.lineHeight = styles.items[i].lineHeight;
         }
      }

      void setStyles(pos_t count, StyleInfo* styleInfos, int lineHeight, int marginWidth, FontFactoryBase* factory);

      TextViewModel();
      virtual ~TextViewModel();
   };
}

#endif