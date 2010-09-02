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


#ifndef GROUPBOX2_H
#define GROUPBOX2_H

NAMESPACE_BEGIN_GUI

class Layout;

/*!
    A stretch factor of 0 or 1 for the GroupBox has no effect because the GroupBox is designed to tightly adjust
    to the size of its composition layout. This unlike the Panel widget for instance who does not force itself to 
    adjust to the size of its composition layout.
*/
class GroupBox2 : public ActiveInterfaceObject
{
public:
    GroupBox2(const TCHAR* Caption = TEXT(""));
    ~GroupBox2();

    virtual long ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void Draw(GraphicsContext& GfxContext, bool force_draw);
    virtual void DrawContent(GraphicsContext& GfxContext, bool force_draw);
    virtual void PostDraw(GraphicsContext& GfxContext, bool force_draw);

    void setLayout(smptr(Layout) layout);
    void setCaption(const TCHAR* Caption);

private:
    virtual void PreLayoutManagement();
    virtual long PostLayoutManagement(long LayoutResult);
    virtual void PositionChildLayout(float offsetX, float offsetY);

    bool bCaptionAvailable;
    smptr(CoreArea) m_CaptionArea;
    smptr(Layout) m_layout;

    static int CAPTION_X_MARGIN;
    static int X_MARGIN;
    static int Y_MARGIN;

    static Color GROUPBOX2_HEADER_BASE_COLOR;
    static Color GROUPBOX2_HEADER_TEXT_COLOR;
    static int TOP_HEADER_HEIGHT;
};
NAMESPACE_END_GUI

#endif // GroupBox2_H