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


#ifndef COLORGRADIENT_H
#define COLORGRADIENT_H

#include "HexRegExpValidator.h"
#include "IntegerValidator.h"
#include "DoubleValidator.h"

namespace nux
{

  class HLayout;
  class EditTextBox;

  class ColorMarkGroup
  {
  public:
    class ColorMark
    {
    public:
      ColorMark (DOUBLE kX, Color color, bool selected = false, bool excluded = false)
        :   m_IsSelected (selected)
        ,   m_IsExcluded (excluded)
      {
        x = kX;
        m_Color = color;
      }

      bool operator == (const ColorMark &knot)
      {
        return ( (x == knot.x) && (m_Color == knot.m_Color) );
      }

      bool operator != (const ColorMark &knot)
      {
        return ( (x != knot.x) || (m_Color != knot.m_Color) );
      }

      bool operator > (const ColorMark &knot) const
      {
        return (x > knot.x);
      }

      bool operator < (const ColorMark &knot) const
      {
        return (x < knot.x);
      }

      double GetX() const
      {
        return x;
      }
      Color GetColor() const
      {
        return m_Color;
      }
      void SetX (double d)
      {
        x = d;
      }
      void SetColor (Color color)
      {
        m_Color = color;
      }
    private:
      DOUBLE x;
      Color m_Color;
      bool m_IsSelected;
      bool m_IsExcluded;

      friend class ColorMarkGroup;
    };

  public:

    ColorMarkGroup()
    {
      // There must always be at least two points for the spline interpolation to work
      //        m_ColorMarkArray.push_back(ColorMark(0.0, 0.0));
      //        m_ColorMarkArray.push_back(ColorMark(1.0, 1.0));
    }

    ColorMarkGroup (const ColorMarkGroup &Other)
    {
      m_ColorMarkArray = Other.m_ColorMarkArray;
    }

    ColorMarkGroup &operator = (const ColorMarkGroup &Other)
    {
      Reset();
      m_ColorMarkArray = Other.m_ColorMarkArray;
      return *this;
    }

    //    void operator = (const ColorMarkGroup* Other)
    //    {
    //        if(Other == 0)
    //            return
    //        Reset();
    //        m_ColorMarkArray = Other->m_ColorMarkArray;
    //        //return *this;
    //    }

    void AddColorMark (DOUBLE x, Color color, bool selected = false)
    {
      std::vector<ColorMark>::iterator it;

      // Check if the exacte same knot is already in.
      it = std::find (m_ColorMarkArray.begin(), m_ColorMarkArray.end(), ColorMark (x, color) );

      if (it != m_ColorMarkArray.end() )
      {
        // already in
        return;
      }

      // Check if a knot is already in with the same x.
      for (it = m_ColorMarkArray.begin(); it != m_ColorMarkArray.end(); it++)
      {
        if ( (*it).x == x)
        {
          (*it).m_Color = color;
          (*it).m_IsSelected = selected;
          return;
        }
      }

      m_ColorMarkArray.push_back (ColorMark (x, color, selected) );
      std::sort (m_ColorMarkArray.begin(), m_ColorMarkArray.end(), std::less<ColorMarkGroup::ColorMark>() );
    }

    bool isKnotSelected (int i) const
    {
      if (i >= GetNumColorMark() || i < 0)
      {
        return false;
      }

      return m_ColorMarkArray[i].m_IsSelected;
    }

    const ColorMark &operator [] (int i) const
    {
      return m_ColorMarkArray[i];
    }

    ColorMark &operator [] (int i)
    {
      return m_ColorMarkArray[i];
    }

    void EraseKnot (int i)
    {
      if (m_ColorMarkArray.size() <= 2)
      {
        // There must always be at least two points for the spline interpolation to work
        return;
      }

      if (i >= GetNumColorMark() || i < 0)
      {
        return;
      }

      std::vector<ColorMark>::iterator it = m_ColorMarkArray.begin();
      it += i;
      m_ColorMarkArray.erase (it);
    }

    void EraseSelectedKnot()
    {
      if (m_ColorMarkArray.size() <= 2)
      {
        // There must always be at least two points for the spline interpolation to work
        return;
      }

      // Traverse the array and erase selected knots.
      bool finish = false;

      do
      {
        // loop many times to avoid erasing iterators while traversing the array. This is not efficient!
        bool found = false;
        std::vector<ColorMark>::iterator it;

        for (it = m_ColorMarkArray.begin(); it != m_ColorMarkArray.end(); it++)
        {
          if ( (*it).m_IsSelected)
          {
            m_ColorMarkArray.erase (it);
            found = TRUE;
            break;
          }
        }

        if (!found)
          finish = TRUE;
      }
      while ( (!finish) && (m_ColorMarkArray.size() > 2) );
    }

    void SelectKnot (int i, bool b)
    {
      m_ColorMarkArray[i].m_IsSelected = b;
    }

    t_u32 GetNumSelectedKnot()
    {
      t_u32 n = 0;
      std::vector<ColorMark>::iterator it;

      for (t_u32 i = 0; i < (t_u32) m_ColorMarkArray.size(); i++)
      {
        if (m_ColorMarkArray[i].m_IsSelected)
          n++;
      }

      return n;
    }

    void SelectAllKnot()
    {
      std::vector<ColorMark>::iterator it;

      for (it = m_ColorMarkArray.begin(); it != m_ColorMarkArray.end(); it++)
      {
        (*it).m_IsSelected = TRUE;
      }
    }

    void UnSelectAllKnot()
    {
      std::vector<ColorMark>::iterator it;

      for (it = m_ColorMarkArray.begin(); it != m_ColorMarkArray.end(); it++)
      {
        (*it).m_IsSelected = false;
      }
    }

    const std::vector<double> GetXArray()
    {
      std::vector<double> array;
      std::vector<ColorMark>::iterator it;

      for (it = m_ColorMarkArray.begin(); it != m_ColorMarkArray.end(); it++)
      {
        if (! (*it).m_IsExcluded)
          array.push_back ( (*it).x);
      }

      return array;
    }


    const std::vector<Color> GetColorArray()
    {
      std::vector<Color> array;
      std::vector<ColorMark>::iterator it;

      for (it = m_ColorMarkArray.begin(); it != m_ColorMarkArray.end(); it++)
      {
        if (! (*it).m_IsExcluded)
          array.push_back ( (*it).m_Color);
      }

      return array;
    }

    int GetNumColorMark() const
    {
      return (int) m_ColorMarkArray.size();
    }
    ColorMark GetColorMark (int i) const
    {
      return m_ColorMarkArray[i];
    }

    /*!
    Erase all knots.
    */
    void Reset()
    {
      m_ColorMarkArray.clear();
      //        AddKnot(0,0);
      //        AddKnot(1,1);
    }
  private:
    std::vector<ColorMark> m_ColorMarkArray;
  };


  class ColorGradient : public View //public ValuatorAbstraction
  {
  public:
    enum ColorMode
    {
      COLORMODE_GRADIENT = 0,
      COLORMODE_HUE
    };

    ColorGradient (float Value = 0, float MinValue = 0.0f, float MaxValue = 1.0f, NUX_FILE_LINE_PROTO);
    virtual ~ColorGradient();

    virtual long ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    void DrawMarker (GraphicsEngine &GfxContext);
    virtual void Draw (GraphicsEngine &GfxContext, bool force_draw);
    virtual void DrawContent (GraphicsEngine &GfxContext, bool force_draw);
    virtual void PostDraw (GraphicsEngine &GfxContext, bool force_draw);

    /////////////////
    //  RECEIVERS  //
    /////////////////
    void SetRange (float min_value, float max_value);
    void SetValue (float value);
    float GetValue() const;
    float GetMinValue() const
    {
      return m_min;
    }
    float GetMaxValue() const
    {
      return m_max;
    }

    void SetBackgroundColor (const Color &color);
    const Color GetBackgroundColor() const;

    ////////////////
    //  EMITTERS  //
    ////////////////
    void OnReceiveMouseDown (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void OnReceiveMouseUp (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void OnReceiveMouseDrag (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);
    void OnKeyboardFocus();
    void OnLostKeyboardFocus();
    void OnValidateKeyboardEntry (EditTextBox *textbox, const NString &text);

    bool IsCtrlKeyPressed() const
    {
      return (m_CTRL_KEY ? true : false);
    }

    // signals
    sigc::signal<void, ColorGradient *> sigValueChanged;
    sigc::signal<void, float> sigFloatChanged;
    sigc::signal<void, float> sigMouseDown;
    sigc::signal<void, float> sigMouseUp;
    sigc::signal<void, float> sigMouseDrag;
    sigc::signal<void, float> sigSetTypedValue;
    //sigc::signal<void, const char*> sigValidateKeyboarEntry;

    void EmitFloatChangedSignal();

    void Reset();

    int GetNumColorMark() const
    {
      return (int) m_ColorMarkGroup.GetNumColorMark();
    }
    ColorMarkGroup::ColorMark GetColorMark (int i) const
    {
      return m_ColorMarkGroup.GetColorMark (i);
    }
    void AddColorMark (DOUBLE x, Color color, bool selected = false);

    void SetColorFormat (Color::Format cf);

  protected:
    void InitializeWidgets();
    void InitializeLayout();
    void DestroyLayout();

  protected:

    ColorMarkGroup m_ColorMarkGroup;

    HLayout        *hlayout;
    EditTextBox    *m_ValueString;
    CoreArea       *m_Percentage;
    Color m_BackgroundColor;

    Color::Format m_color_format;

    long m_CTRL_KEY;

    float m_Value;
    float m_min, m_max;
    HexRegExpValidator m_HexRegExp;
    IntegerValidator m_IntRegExp;
    DoubleValidator m_DoubleRegExp;
  };

}

#endif // COLORGRADIENT_H
