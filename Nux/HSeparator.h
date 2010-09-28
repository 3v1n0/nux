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


#ifndef HSEPARATOR_H
#define HSEPARATOR_H

namespace nux { //NUX_NAMESPACE_BEGIN

class HSeparator: public AbstractSeparator
{
public:
    HSeparator();
    HSeparator(const Color& color, float Alpha0, float Alpha1, int Border);

    ~HSeparator();

    virtual long ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void Draw(GraphicsContext& GfxContext, bool force_draw);
    virtual void DrawContent(GraphicsContext& GfxContext, bool force_draw) {};
    virtual void PostDraw(GraphicsContext& GfxContext, bool force_draw) {};

protected:
    void InitializeWidgets();
    void InitializeLayout();
    void DestroyLayout();

private:
    // Inherited from BaseObject. Declared as private so they can't be accessed by client.
    virtual void SetMinimumSize(int w, int h) { BaseObject::SetMinimumSize(w, h); }
    virtual void SetMaximumSize(int w, int h) { BaseObject::SetMaximumSize(w, h); }
    virtual void SetMinMaxSize(int w, int h) { BaseObject::SetMinMaxSize(w, h); }
    virtual void SetMinimumWidth(int w) { BaseObject::SetMinimumWidth(w); }
    virtual void SetMaximumWidth(int w) { BaseObject::SetMaximumWidth(w); }
    virtual void SetMinimumHeight(int h) { BaseObject::SetMinimumHeight(h); }
    virtual void SetMaximumHeight(int h) { BaseObject::SetMaximumHeight(h); }

};

} //NUX_NAMESPACE_END

#endif // HSEPARATOR_H
