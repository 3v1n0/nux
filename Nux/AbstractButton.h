/*
 * Copyright 2010 Inalogic® Inc.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License, as
 * published by the  Free Software Foundation; either version 2.1 or 3.0
 * of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the applicable version of the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of both the GNU Lesser General Public
 * License along with this program. If not, see <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */

#ifndef ABSTRACTBUTTON_H
#define ABSTRACTBUTTON_H

#include "View.h"
#include "NuxCore/Property.h"

namespace nux
{
  class StaticText;

  enum ButtonVisualState
  {
    VISUAL_STATE_PRESSED = 0,
    VISUAL_STATE_NORMAL = 1,
    VISUAL_STATE_PRELIGHT = 2,
    VISUAL_STATE_DISABLED = 3
  };

  //! The base class of button type widgets.
  /*!
      AbstractButton is the direct base class of Button, CheckBox and RadioButton. It is a base class for ToggleButton.
  */
  class AbstractButton : public View
  {
    NUX_DECLARE_OBJECT_TYPE(AbstractButton, View);
  public:
    AbstractButton(NUX_FILE_LINE_PROTO);
    virtual ~AbstractButton();

    sigc::signal<void, AbstractButton*> visual_state_change;

    //! Returns the visual state of the AbstractButton
    /*!
        Returns the visual state of the AbstractButton.

        @return The visual state of the AbstractButton.
    */
    ButtonVisualState GetVisualState();

    //! Return the active state of the button.
    /*!
        Return the active state of the button. A Button has a non-persistent active state. It always returns false.
        CheckBox, RadionButton and ToggleButton return true if they are active.

        @return True if the button object is in an active state.
    */
    bool Active() const;

    //! Activate the button.
    /*!
         Activate the button. If this object is a Button, then it has no persistent state and the function does nothing.
    */
    virtual void Activate() = 0;

    //! Deactivate the button.
    /*!
         Deactivate the button. If this object is a Button, then it has no persistent state and the function does nothing.
    */
    virtual void Deactivate() = 0;

    //! Set the state of the button.
    /*!
        Set the sate of the button. The button is either activated or deactivated.

        @param active The button state.
    */
    virtual void SetActive(bool active);

    //! Set the text color.
    /*!
        Set the text color.

        @param color the text color.
    */
    void SetLabelColor(const Color &color);

    //! Get the text color.
    /*!
        Get the text color.

        @return The text color.
    */
    Color GetLabelColor();

    //! Get the button visual state.
    /*!
        Get the button visual state.

        @return The button visual state.
    */
    ButtonVisualState GetVisualState() const;

    virtual void SetLabelFontSize(int point);
    int GetLabelFontSize() const;

  protected:
    //! The state of the AbstractButton. 
    bool active_;

    bool mouse_pressed_;

    //! Visual state of the button object.
    ButtonVisualState visual_state_;


    // Common attribute for all the classes that inherit from AbstractButton.
    std::string label_;
    Color label_color_;
    StaticText *static_text_;
    int label_font_size_;
    bool same_size_as_content_;
    int pre_layout_width_;
    int pre_layout_height_;

    virtual void RecvMouseUp    (int x, int y, unsigned long button_flags, unsigned long key_flags);
    virtual void RecvMouseDown  (int x, int y, unsigned long button_flags, unsigned long key_flags);
    virtual void RecvMouseMove  (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);
    virtual void RecvMouseEnter(int x, int y, unsigned long button_flags, unsigned long key_flags);
    virtual void RecvMouseLeave(int x, int y, unsigned long button_flags, unsigned long key_flags);
    virtual void RecvClick      (int x, int y, unsigned long button_flags, unsigned long key_flags) = 0;
  };
}

#endif // ABSTRACTBUTTON_H
