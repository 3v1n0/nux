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


#ifndef TIMEGRAPH_H
#define TIMEGRAPH_H

#include "NuxGraphics/GLDeviceFactory.h"
#include "NuxGraphics/GLDeviceObjects.h"
#include "NuxGraphics/GLSh_DrawFunction.h"
#include "TimerProc.h"

NAMESPACE_BEGIN_GUI

typedef float (*FunctionCallback)(float);  

class VLayout;
class HLayout;
class Button;
class StaticTextBox;
class BaseArea;

class TimeGraph : public ActiveInterfaceObject
{
public:
    TimeGraph(const TCHAR* Title);
    ~TimeGraph();
    virtual long ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void Draw(GraphicsContext& GfxContext, bool force_draw);
    virtual void DrawContent(GraphicsContext& GfxContext, bool force_draw);
    virtual void PostDraw(GraphicsContext& GfxContext, bool force_draw);

    void AddValue(float Value);
    void SetYAxisBounds(float minY, float maxY);
    unsigned int AddGraph(Color PrimaryColor, Color SecondaryColor);
    void UpdateGraph(unsigned int index, float Value);

    void RecvShowBarGraphics(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvShowCurveGraphics(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvShowValue(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void ShowGraphStyle();
    void ShowColumnStyle();
    void ShowNumberStyle();
protected:
    void InitializeWidgets();
    void InitializeLayout();
    void DestroyLayout();

private:
    static unsigned int sBufferSize;
    smptr(VLayout) m_vlayout;
    smptr(HLayout) m_hlayout;
    smptr(HLayout) m_hlayout2;
    smptr(Button) m_button;
    smptr(StaticTextBox) m_GraphTitle;
    smptr(CoreArea) m_GraphArea;
    smptr(CoreArea) m_GraphIcon;
    smptr(CoreArea) m_GraphBarIcon;
    smptr(CoreArea) m_ValueIcon;
    NString m_Title;

    float m_arg0;
    float m_arg1;
    float m_arg2;
    float m_arg3;

    enum 
    {
        SHOW_GRAPH,
        SHOW_COLUMN,
        SHOW_VALUE
    };

    class Graph
    {
    public:
        Graph(Color PrimaryColor, Color SecondaryColor);
        Graph(const Graph &graph);
        Graph& operator = (const Graph &graph);
        ~Graph();
        void Update(float Value);
        void Clear();
        void Reset();
        std::list<float> m_ValueList;
        // ! Set to true if the graph has been updated during the graphic refresh period.
        bool m_HasBeenUpdated;
        Color m_PrimaryColor;
        Color m_SecondaryColor;
    };

    unsigned int m_Option;

    std::vector<Graph> m_DynValueArray;
    unsigned int m_DynValueCount;
    bool m_DynValueReceived;
    
    void GraphTimerInterrupt(void* v);
    TimerFunctor* m_ScrollTimerFunctor;
    TimerHandle* m_ScrollTimerHandler;

    float m_minY, m_maxY;
    FunctionCallback m_FunctionCallback;

    TRefGL<IOpenGLTexture2D> Texture;
    TextureLayer* m_BackgroundLayer;
    GLSh_DrawFunction *m_DrawFunctionShader;
    NTexture2D *m_CheckboardPattern;
};


NAMESPACE_END_GUI

#endif // TIMEGRAPH_H
