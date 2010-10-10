/*
 * Copyright 2010 Inalogic Inc.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version 3, as
 * published by the  Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the applicable version of the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of both the GNU Lesser General Public
 * License version 3 along with this program.  If not, see
 * <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jay.taoko_AT_gmail_DOT_com>
 *
 */


#ifndef STATICTEXTBOX_H
#define STATICTEXTBOX_H

namespace nux
{

  class Validator;

  class StaticTextBox : public View //public CoreArea
  {
  public:
    StaticTextBox (const TCHAR *Caption, NUX_FILE_LINE_PROTO);
    ~StaticTextBox();
    virtual long ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void Draw (GraphicsContext &GfxContext, bool force_draw);
    virtual void DrawContent (GraphicsContext &GfxContext, bool force_draw);
    virtual void PostDraw (GraphicsContext &GfxContext, bool force_draw);

    // API
    void SetText (const TCHAR &Caption);
    void SetText (const TCHAR *Caption);
    void SetText (const tstring &Caption);
    void SetText (const NString &Caption);
    const TCHAR *GetText() const;
    t_u32 GetTextSize() const
    {
      return (t_u32) m_Text.Length();
    }

    //! Change the widget minimum width whenever the text is set.
    /*!
        Change the widget minimum width whenever the text is set.
        @param b If true, the widget minimum width is set to match the size of the text
    */
    void SetMinWidthMatchText (bool b);
    //! Return true if the widget with changes to match the text width.
    bool GetMinWidthMatchText() const;

    void SetTextColor (Color color)
    {
      m_TextColor = color;
    };
    Color GetTextColor() const
    {
      return m_TextColor;
    };

    void SetTextBackgroundColor (const Color &color)
    {
      m_BackgroundColor = color;
    }
    Color GetTextBackgroundColor() const
    {
      return m_BackgroundColor;
    }

    void SetTextAlignment (TextAlignment alignment)
    {
      m_TextAlignment = alignment;
    }
    TextAlignment GetTextAlignment()
    {
      return m_TextAlignment;
    }

    void SetDrawBackground (bool b)
    {
      m_bDrawBackground = b;
    }
    void SetBackground (AbstractPaintLayer *bkg);

    /////////////////
    //  SIGNALS    //
    /////////////////

    virtual void SetFont (IntrusiveSP<FontTexture> Font);
  protected:
    void AdjustMinWidthToMatchText();

    NString m_Text;
    Color m_TextColor;
    Color m_BackgroundColor;
    tstring m_temporary_caption;
    Validator *m_validator;
    TextAlignment m_TextAlignment;
    AbstractPaintLayer *m_Background;
    bool m_bMinimumWidthMatchText;
    bool m_bDrawBackground;
    //! If true, blend the characters alpha value with the destination and write the result to the destination buffer.
    bool m_WriteAlpha;

  public:
    virtual void SetGeometry (const Geometry &geo)
    {
      Area::SetGeometry (geo);
      ComputeChildLayout();
    }
  };

}

#endif // STATICTEXTBOX_H
