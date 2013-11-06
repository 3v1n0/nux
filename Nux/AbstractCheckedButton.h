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


#ifndef ABSTRACTCHECKEDBUTTON_H
#define ABSTRACTCHECKEDBUTTON_H

#include "AbstractButton.h"

namespace nux
{
  class HLayout;
  class InputArea;
  class StaticText;

  //! AbstractCheckedButton class
  /*!
      Acts as a base class for CheckBox and radioButton.\n
  */
  class AbstractCheckedButton: public AbstractButton
  {
    NUX_DECLARE_OBJECT_TYPE(AbstractCheckedButton, AbstractButton);
  public:
    AbstractCheckedButton(const std::string &str, bool state = false, NUX_FILE_LINE_PROTO);
    virtual ~AbstractCheckedButton();

    //! Emitted when the button is clicked.
    sigc::signal<void, AbstractCheckedButton*> click;

    //! Emitted when the active state changes.
    /*!
        Emitted when the active state changes, as a result of a mouse click or an API call.\n
        \sa Activate, Deactivate.
    */
    sigc::signal<void, AbstractCheckedButton*> state_change;

    //! Set the label.
    /*!
        Set the label of this AbstractCheckedButton. If the \a label argument is an empty string, then the the AbstractCheckedButton label is destroyed,
        and the content of the AbstractCheckedButton is re-arranged accordingly.

        @param label The label of the AbstractCheckedButton.
    */
    void SetLabel(const std::string &checkbox_label);

    //!Return the label of this AbstractCheckedButton.
    /*!
        Return the label of this AbstractCheckedButton.

        @return The AbstractCheckedButton label string.
    */
    std::string GetLabel() const;

    //! Activate the check box.
    /*!
         Activate the check box.
    */
    virtual void Activate() = 0;

    //! Deactivate the check box.
    /*!
         Deactivate the check box.
    */
    virtual void Deactivate() = 0;

    virtual void SetLabelFontSize(int point);

  protected:
    virtual void Draw(GraphicsEngine &graphics_engine, bool force_draw) = 0;
    virtual void RecvClick(int x, int y, unsigned long button_flags, unsigned long key_flags) = 0;
    virtual long ComputeContentSize();

    HLayout   *hlayout_;
    InputArea *check_area_;

  private:
    //! Override of Area::SetMinimumHeight and made private.
    /*!
        Prevent changing the minimum height of the StaticText view.
    */
    virtual void SetMinimumHeight(int h){};

    //! Override of Area::SetMaximumHeight and made private.
    /*!
        Prevent changing the maximum height of the StaticText view.
    */
    virtual void SetMaximumHeight(int h){};
  };

}

#endif // ABSTRACTCHECKEDBUTTON_H
