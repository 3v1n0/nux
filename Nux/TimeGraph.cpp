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


#include "Nux.h"

#include "NuxGraphics/GLSh_DrawFunction.h"
#include "NuxGraphics/GLDeviceFactory.h"
#include "NuxGraphics/GLDeviceObjects.h"

#include "TimeGraph.h"

#include "VLayout.h"
#include "HLayout.h"
#include "Button.h"
#include "StaticTextBox.h"

NAMESPACE_BEGIN_GUI

TimeGraph::Graph::Graph(Color PrimaryColor, Color SecondaryColor)
:   m_HasBeenUpdated(false)
{
    m_PrimaryColor = PrimaryColor;
    m_SecondaryColor = SecondaryColor;
}

TimeGraph::Graph::Graph(const Graph &graph)
{
    m_PrimaryColor = graph.m_PrimaryColor;
    m_SecondaryColor = graph.m_SecondaryColor;
    m_HasBeenUpdated = graph.m_HasBeenUpdated;
    m_ValueList = graph.m_ValueList; 
}

TimeGraph::Graph& TimeGraph::Graph::operator = (const TimeGraph::Graph &graph)
{
    if(this == &graph)
        return *this;

    m_PrimaryColor = graph.m_PrimaryColor;
    m_SecondaryColor = graph.m_SecondaryColor;
    m_HasBeenUpdated = graph.m_HasBeenUpdated;
    m_ValueList = graph.m_ValueList;

    return *this;
}

TimeGraph::Graph::~Graph()
{
    Clear();
}

void TimeGraph::Graph::Update(float Value)
{
    m_ValueList.push_front(Value);
    if(m_ValueList.size() >= sBufferSize)
    {
        m_ValueList.pop_back();
    }
    m_HasBeenUpdated = false;
}

void TimeGraph::Graph::Clear()
{
    m_ValueList.clear();
    m_HasBeenUpdated = false;
}

void TimeGraph::Graph::Reset()
{

}

t_u32 TimeGraph::sBufferSize = 2048;

TimeGraph::TimeGraph(const TCHAR* Title)
:   m_minY(0.0f)
,   m_maxY(1.0f)
,   m_FunctionCallback(0)
,   m_DynValueCount(0)
,   m_DynValueReceived(false)
,   m_Option(SHOW_GRAPH)
,   m_Title(Title)
{
    InitializeLayout();
    InitializeWidgets();

    m_arg0 = 0;
    m_arg1 = 0;
    m_arg2 = 0;
    m_arg3 = 0;

    AddGraph(Color(0xFF9AD61F), Color(0x50191919));
    AddGraph(Color(0xFF00FF00), Color(0x5000FF00));
    AddGraph(Color(0xFFFF0022), Color(0x50BB0022));
    //AddGraph(Color(0xFFFF0000), Color(0x99FF0000));
}

TimeGraph::~TimeGraph()
{
    DestroyLayout();
}

void TimeGraph::InitializeWidgets()
{
    SetMinimumSize(200, 100);
    SetBaseSize(200, 100);

    if(!m_Title.IsEmpty())
        m_GraphTitle->SetText(m_Title);

    Texture = GetThreadGLDeviceFactory()->CreateTexture(256, 4, 0, BITFMT_R8G8B8A8);
    m_DrawFunctionShader = new GLSh_DrawFunction();
//     NString Path = INL_FINDRESOURCELOCATION(TEXT("Data/UITextures/FunctionGraphBackground.tga"));
//     NTexture2D BackgroundTexture;
//     BackgroundTexture.Update(Path.GetTCharPtr());
// 
//     TexCoordXForm texxform;
//     texxform.SetTexCoordType(TexCoordXForm::OFFSET_COORD);
//     texxform.SetWrap(TEXWRAP_REPEAT, TEXWRAP_REPEAT);
//     m_BackgroundLayer = new TextureLayer(BackgroundTexture.GetDeviceTexture(), texxform, Color::White);

    m_ScrollTimerFunctor = new TimerFunctor();
    m_ScrollTimerFunctor->OnTimerExpired.connect(sigc::mem_fun(this, &TimeGraph::GraphTimerInterrupt));
    m_ScrollTimerHandler = GetThreadTimer().AddTimerHandler(1000, m_ScrollTimerFunctor, this);

    NTextureData image;
    MakeCheckBoardImage(image.GetSurface(0), 64, 64, Color(0xff323232), Color(0xff535353), 8, 8);
    NTexture2D CheckboardPattern;
    CheckboardPattern.Update(&image);

    TexCoordXForm texxform;
    texxform.SetTexCoordType(TexCoordXForm::OFFSET_COORD);
    texxform.SetWrap(TEXWRAP_REPEAT, TEXWRAP_REPEAT);
    m_BackgroundLayer = new TextureLayer(CheckboardPattern.GetDeviceTexture(), texxform, Color::White);

    m_ValueIcon->SetMinMaxSize(40, 16);
    m_GraphIcon->SetMinMaxSize(16, 16);
    m_GraphBarIcon->SetMinMaxSize(16, 16);


    m_GraphBarIcon->OnMouseClick.connect(sigc::mem_fun(this, &TimeGraph::RecvShowBarGraphics));
    m_GraphIcon->OnMouseClick.connect(sigc::mem_fun(this, &TimeGraph::RecvShowCurveGraphics));
    m_ValueIcon->OnMouseClick.connect(sigc::mem_fun(this, &TimeGraph::RecvShowValue));

    m_GraphTitle->SetFont(GFontBold);
    m_hlayout->AddActiveInterfaceObject(smptr(SpaceLayout)(new SpaceLayout(40, 40, 0, BASEOBJECT_MAXHEIGHT)), 1);
    m_hlayout->AddActiveInterfaceObject(m_GraphTitle, 1);
    m_hlayout->AddActiveInterfaceObject(m_ValueIcon, 0);
    m_hlayout->AddActiveInterfaceObject(m_GraphBarIcon, 0);
    m_hlayout->AddActiveInterfaceObject(m_GraphIcon, 0);
    m_hlayout->SetHorizontalInternalMargin(4);

    m_hlayout2->AddLayout(smptr(SpaceLayout)(new SpaceLayout(40, 40, 0, BASEOBJECT_MAXHEIGHT)), 1);
    m_hlayout2->AddActiveInterfaceObject(m_GraphArea, 1);
    m_hlayout2->AddLayout(smptr(SpaceLayout)(new SpaceLayout(50, 40, 0, BASEOBJECT_MAXHEIGHT)), 1);

    m_vlayout->AddLayout(m_hlayout, 0);
    m_vlayout->AddLayout(m_hlayout2, 1);
    m_vlayout->SetVerticalInternalMargin(2);

    SetCompositionLayout(m_vlayout);
    SetMinimumSize(200, 120);
}

void TimeGraph::InitializeLayout()
{
    m_vlayout = smptr(VLayout)(new VLayout());
    m_hlayout = smptr(HLayout)(new HLayout());
    m_hlayout2 = smptr(HLayout)(new HLayout(TEXT("QWERTY")));
    m_button = smptr(Button)(new Button(TEXT("Value"), false));
    m_GraphTitle = smptr(StaticTextBox)(new StaticTextBox(TEXT("Time Graph")));
    m_GraphArea = smptr(CoreArea)(new CoreArea());
    m_GraphBarIcon = smptr(CoreArea)(new CoreArea());
    m_GraphIcon = smptr(CoreArea)(new CoreArea());
    m_ValueIcon = smptr(CoreArea)(new CoreArea());
}

void TimeGraph::DestroyLayout()
{
    GetThreadTimer().RemoveTimerHandler(m_ScrollTimerHandler);
    INL_SAFE_DELETE(m_ScrollTimerFunctor);
    INL_SAFE_DELETE(m_DrawFunctionShader);
    INL_SAFE_DELETE(m_BackgroundLayer);
}

void TimeGraph::GraphTimerInterrupt(void* v)
{
    m_arg0 += 0.001f;

    for(t_u32 i = 0; i < m_DynValueArray.size(); i++)
    {
        if(m_DynValueArray[i].m_HasBeenUpdated == false)
        {
            if(m_DynValueArray[i].m_ValueList.size() != 0)
            {
                float prev = (*m_DynValueArray[i].m_ValueList.begin());
                prev /= 2.0f;
                if(Abs(prev) < 0.2f)
                    prev = 0.0f;
                if(i == 0)
                    m_DynValueArray[i].Update(GetGraphicsThread()->GetFrameRate());
                if(i == 1)
                    m_DynValueArray[i].Update(RandomUInt(25) + 25);
                if(i == 2)
                    m_DynValueArray[i].Update(30*(std::sin(m_arg0) + 1) + RandomUInt(10));
                if(i == 3)
                    m_DynValueArray[i].Update(30*(std::sin(m_arg0) + 1)+5);
            }
            else
            {
                m_DynValueArray[i].Update(0.0f);
            }
        }
    }
    m_DynValueReceived = false;
    //m_ScrollTimerHandler = GetThreadTimer().AddTimerHandler(100, m_ScrollTimerFunctor, this);
    NeedRedraw();
}

void TimeGraph::RecvShowBarGraphics(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
    m_Option = SHOW_COLUMN;
    NeedRedraw();
}

void TimeGraph::RecvShowCurveGraphics(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
    m_Option = SHOW_GRAPH;
    NeedRedraw();
}

void TimeGraph::RecvShowValue(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
    m_Option = SHOW_VALUE;
    NeedRedraw();
}


long TimeGraph::ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
{
    long ret = m_button->ProcessEvent(ievent, TraverseInfo, ProcessEventInfo);
    ret = m_GraphBarIcon->OnEvent(ievent, ret, ProcessEventInfo);
    ret = m_GraphIcon->OnEvent(ievent, ret, ProcessEventInfo);
    ret = m_ValueIcon->OnEvent(ievent, ret, ProcessEventInfo);
    return ret;
}

void TimeGraph::Draw(GraphicsContext& GfxContext, bool force_draw)
{
    Geometry base = GetGeometry();

    GfxContext.PushClippingRectangle(base);
    gPainter.PaintBackground(GfxContext, base);

    base.OffsetPosition(1, 1);
    base.OffsetSize(-2, -2);


    int W = m_GraphArea->GetBaseWidth()- 2;
    int H = m_GraphArea->GetBaseHeight() - 2;
    int X = m_GraphArea->GetBaseX() + 1;
    int Y = m_GraphArea->GetBaseY() + 1;

    

    m_GraphTitle->ProcessDraw(GfxContext, true);

    // Draw Graph Background
    {
        m_BackgroundLayer->SetGeometry(Geometry(X, Y, W, H));
        m_BackgroundLayer->Renderlayer(GfxContext);
    }

    {
        gPainter.PaintBackground(GfxContext, m_GraphBarIcon->GetGeometry());
        GeometryPositioning gp(eHACenter, eVACenter);
        Geometry GeoPo = ComputeGeometryPositioning(m_GraphBarIcon->GetGeometry(), gTheme.GetImageGeometry(eGraphBarIcon), gp);
        gPainter.Paint2DQuadColor(GfxContext, m_GraphBarIcon->GetGeometry(), Color(0xFF000000));
        gPainter.PaintShape(GfxContext, GeoPo, Color(0xFFFF9C00), eGraphBarIcon);
    }

    {
        gPainter.PaintBackground(GfxContext, m_GraphIcon->GetGeometry());
        GeometryPositioning gp(eHACenter, eVACenter);
        Geometry GeoPo = ComputeGeometryPositioning(m_GraphIcon->GetGeometry(), gTheme.GetImageGeometry(eGraphIcon), gp);
        gPainter.Paint2DQuadColor(GfxContext, m_GraphIcon->GetGeometry(), Color(0xFF000000));
        gPainter.PaintShape(GfxContext, GeoPo, Color(0xFFFF9C00), eGraphIcon);
    }

    {
        NString ValueString;
        ValueString = NString::Printf(TEXT("%.2f"), m_maxY);
        gPainter.PaintTextLineStatic(GfxContext, GFont, Geometry(base.x, Y, 40, 12), ValueString, Color(0xFFFFFFFF));

        ValueString = NString::Printf(TEXT("%.2f"), m_minY);
        gPainter.PaintTextLineStatic(GfxContext, GFont, Geometry(base.x, Y + H - 12, 40, 12), ValueString, Color(0xFFFFFFFF));

    }

    t_u32 numGraph = (t_u32)m_DynValueArray.size();
    t_u32 PosY = Y;
    for(t_u32 index = 0; index < numGraph; index++)
    {
        GeometryPositioning gp(eHACenter, eVACenter);
        Geometry GeoPo = ComputeGeometryPositioning(Geometry(X + W + 2, PosY, 8, 8), gTheme.GetImageGeometry(eDOT6x6), gp);
        //gPainter.Paint2DQuadColor(GfxContext, Geometry(X + W + 2, PosY, 8, 8), 0xFF000000);
        gPainter.PaintShape(GfxContext, GeoPo, m_DynValueArray[index].m_PrimaryColor, eDOT6x6);

        NString ValueString;
        if(m_DynValueArray[index].m_ValueList.size() == 0)
            ValueString = NString::Printf(TEXT("%.2f"), TEXT("0.00"));
        else
            ValueString = NString::Printf(TEXT("%.2f"), (*m_DynValueArray[index].m_ValueList.begin()));
        gPainter.PaintTextLineStatic(GfxContext, GFont, Geometry(X + W + 2 +6, PosY, 40, 8), ValueString, Color(0xFFFFFFFF), true, eAlignTextCenter);

        PosY += 12;
    }

    if(m_Option == SHOW_GRAPH)
    {
        GfxContext.PushClippingRectangle(m_GraphArea->GetGeometry());
        t_u32 numGraph = (t_u32)m_DynValueArray.size();
        for(t_u32 index = 0; index < numGraph; index++)
        {
            if(m_DynValueArray[index].m_ValueList.size() <= 1)
                continue;

            float dY = (m_maxY - m_minY) / H;
            std::list<float>::iterator it = m_DynValueArray[index].m_ValueList.begin();
            float tex_dy = (m_maxY - m_minY) / Texture->GetWidth();

            if(Texture->GetWidth() != W)
            {
                Texture = GetThreadGLDeviceFactory()->CreateTexture(W, 4, 1, BITFMT_R8G8B8A8);
            }

            it = m_DynValueArray[index].m_ValueList.begin();
            SURFACE_LOCKED_RECT lockrect;
            Texture.Handle->LockRect(0, &lockrect, 0);
            BYTE *dest = (BYTE*)lockrect.pBits;

            for(t_u32 i = 0; i < Texture->GetWidth(); i++)
            {
                float y;
                if(it == m_DynValueArray[index].m_ValueList.end())
                    y = 0.0f;
                else
                {
                    y = (*it);
                    it++;
                }
                y = (y - m_minY) / (m_maxY - m_minY);

                for(t_u32 j = 0; j < Texture->GetHeight(); j++)
                {
                    dest[j * lockrect.Pitch + 4*(W - 1 - i) + 0] = 255 * Clamp<float>(y, 0.0f, 1.0f);
                    dest[j * lockrect.Pitch + 4*(W - 1 - i) + 1] = 255 * Clamp<float>(y, 0.0f, 1.0f);
                    dest[j * lockrect.Pitch + 4*(W - 1 - i) + 2] = 255 * Clamp<float>(y, 0.0f, 1.0f);
                    dest[j * lockrect.Pitch + 4*(W - 1 - i) + 3] = 255 * Clamp<float>(y, 0.0f, 1.0f);
                }
            }
            Texture->UnlockRect(0);
            GfxContext.GetRenderStates().SetBlend(TRUE, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            m_DrawFunctionShader->SetTransformMatrix(GfxContext.GetProjectionMatrix());

            m_DrawFunctionShader->SetTextureFunction(Texture);
            m_DrawFunctionShader->SetBackgroundColor(m_DynValueArray[index].m_SecondaryColor);
            m_DrawFunctionShader->Render(X, Y, 0, W, H, GfxContext.GetWindowWidth(), GfxContext.GetWindowHeight());

            GfxContext.GetRenderStates().EnableLineSmooth(TRUE, 1, GL_FASTEST);    //You need this blending formula to get anti-aliased lines

            it = m_DynValueArray[index].m_ValueList.begin();
            float x0, y0;
            x0 = X + W;
            y0 = (*it++);
            for( ; it != m_DynValueArray[index].m_ValueList.end(); it++)
            {
                float x1, y1;
                x1 = x0 - 1;
                y1 = (*it);

                int X0, Y0, X1, Y1;
                X0 = x0; //X + W * (x0 - m_minX) / (m_maxX - m_minX);
                Y0 = Y + H * ( 1 - (y0 - m_minY) / (m_maxY - m_minY));
                X1 = x1; //X + W * (x1 - m_minX) / (m_maxX - m_minX);
                Y1 = Y + H * ( 1 - (y1 - m_minY) / (m_maxY - m_minY));
                gPainter.Draw2DLine(GfxContext, X0, Y0, X1, Y1, Color(m_DynValueArray[index].m_PrimaryColor));

                x0 = x1;
                y0 = y1;
            }
            GfxContext.GetRenderStates().EnableLineSmooth(FALSE);
            GfxContext.GetRenderStates().SetBlend(GL_FALSE);
        }
        GfxContext.PopClippingRectangle();
    }

    if(m_Option == SHOW_COLUMN)
    {
        t_u32 BarWidth = W / (t_u32)m_DynValueArray.size();
        if(BarWidth < 10)
            BarWidth = 10;
        
        GfxContext.PushClippingRectangle(m_GraphArea->GetGeometry());
        t_u32 numGraph = (t_u32)m_DynValueArray.size();
        for(t_u32 index = 0; index < numGraph; index++)
        {
            float x0, y0;
            int X0, Y0;

            //y0 = 0.0f;
            if(m_DynValueArray[index].m_ValueList.size() != 0)
            {
                y0 = (*m_DynValueArray[index].m_ValueList.begin());
            }

            x0 = X + W - (index + 1) * BarWidth;

            X0 = x0;
            Y0 = Y + H * ( 1 - (y0 - m_minY) / (m_maxY - m_minY));
            if(index == numGraph-1)
            {
                if(X0 > X)
                {
                    X0 = X;
                    BarWidth += W - numGraph*BarWidth;
                }
            }
            gPainter.Paint2DQuadColor(GfxContext, X0, Y0, BarWidth, Y + H - Y0, m_DynValueArray[index].m_PrimaryColor);
        }
        GfxContext.PopClippingRectangle();
    }
    gPainter.Paint2DQuadWireframe(GfxContext, m_GraphArea->GetGeometry(), Color(COLOR_BACKGROUND_SECONDARY));

    GfxContext.PopClippingRectangle();
//     else
//     {
//         NString ValueString;
//         if(m_DynValueArray[index].m_ValueList.size() == 0)
//             ValueString = NString::Printf(TEXT("%.2f"), TEXT("0.00"));
//         else
//             ValueString = NString::Printf(TEXT("%.2f"), (*m_DynValueArray[index].m_ValueList.begin()));
//         gPainter.PaintTextLineStatic(GfxContext, GFontBold, m_GraphArea->GetGeometry(), ValueString, 0xFFFFFFFF, eAlignTextCenter);
//     }
}

void TimeGraph::DrawContent(GraphicsContext& GfxContext, bool force_draw)
{
    Geometry base = GetGeometry();
    GfxContext.PushClippingRectangle(base);

    //m_button->ProcessDraw(GfxContext, force_draw);
    //m_GraphTitle->ProcessDraw(GfxContext, force_draw);

    GfxContext.PopClippingRectangle();
}

void TimeGraph::PostDraw(GraphicsContext& GfxContext, bool force_draw)
{

}

void TimeGraph::SetYAxisBounds(float minY, float maxY)
{
    m_minY = minY;
    m_maxY = maxY;
    NeedRedraw();
}

void TimeGraph::AddValue(float Value)
{
//     m_DynValueArray.push_front(Value);
//     m_DynValueReceived = true;
//     if(m_DynValueArray.size() >= 2048)
//     {
//         m_DynValueArray.pop_back();
//     }
}

t_u32 TimeGraph::AddGraph(Color PrimaryColor, Color SecondaryColor)
{
    Graph graph(PrimaryColor, SecondaryColor);
    // Set an initial value;
    graph.Update(0.0f);
    t_u32 index = (t_u32)m_DynValueArray.size();
    m_DynValueArray.push_back(graph);

    return index;
}

void TimeGraph::UpdateGraph(t_u32 index, float Value)
{
    nuxAssert(index < m_DynValueArray.size());
    if(index >= m_DynValueArray.size())
        return;
    m_DynValueArray[index].Update(Value);
}

void TimeGraph::ShowGraphStyle()
{
    m_Option = SHOW_GRAPH;
    NeedRedraw();
}

void TimeGraph::ShowColumnStyle()
{
    m_Option = SHOW_COLUMN;
    NeedRedraw();
}

void TimeGraph::ShowNumberStyle()
{
    m_Option = SHOW_VALUE;
    NeedRedraw();
}


NAMESPACE_END_GUI
