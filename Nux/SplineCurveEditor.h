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


#ifndef SPLINECURVEEDITOR_H
#define SPLINECURVEEDITOR_H

#include "PaintLayer.h"

NAMESPACE_BEGIN_GUI

class CubicSpline;
class SplineKnot;
class GLSh_DrawFunction;

class SplineKnot
{
public:
    class Knot
    {
    public:
        Knot(double kX, double kY, bool selected = false, bool excluded = false)
            :   m_IsSelected(selected)
            ,   m_IsExcluded(excluded)
        {
            x = kX;
            y = kY;
        }

        bool operator == (const Knot& knot)
        {
            return ((x == knot.x) && (y == knot.y));
        }
        
        bool operator != (const Knot& knot)
        {
            return ((x != knot.x) || (y != knot.y));
        }

        bool operator > (const Knot& knot) const
        {
            return (x > knot.x);
        }

        bool operator < (const Knot& knot) const
        {
            return (x < knot.x);
        }

        double GetX() const {return x;}
        double GetY() const {return y;}
        void SetX(double d) {x = d;}
        void SetY(double d) {y = d;}
    private:
        double x, y;
        bool m_IsSelected;
        bool m_IsExcluded;

        friend class SplineKnot;
    };

public :

    SplineKnot()
    {
        // There must always be at least two points for the spline interpolation to work
//        m_knot.push_back(Knot(0.0, 0.0));
//        m_knot.push_back(Knot(1.0, 1.0));
    }

    SplineKnot(const SplineKnot& Other)
    {
        m_knot = Other.m_knot;
    }

    SplineKnot& operator = (const SplineKnot& Other)
    {
        Reset();
        m_knot = Other.m_knot;
        return *this;
    }

//    void operator = (const SplineKnot* Other)
//    {
//        if(Other == 0)
//            return
//        Reset();
//        m_knot = Other->m_knot;
//        //return *this;
//    }

    void AddKnot(double x, double y, bool selected = false)
    {
        std::vector<Knot>::iterator it;

        // Check if the exacte same knot is already in.
        it = std::find(m_knot.begin(), m_knot.end(), Knot(x, y));
        if(it != m_knot.end())
        {
            // already in
            return;
        }

        // Check if a knot is already in with the same x.
        for(it = m_knot.begin(); it != m_knot.end(); it++)
        {
            if((*it).x == x)
            {
                (*it).y = y;
                (*it).m_IsSelected = selected;
                return;
            }
        }
        m_knot.push_back(Knot(x, y, selected));
        std::sort(m_knot.begin(), m_knot.end(), std::less<SplineKnot::Knot>());
    }

    bool isKnotSelected(int i) const
    {
        if(i >= GetNumKnot() || i < 0)
        {
            return false;
        }
        return m_knot[i].m_IsSelected;
    }

    const Knot& operator [] (int i) const
    {
        return m_knot[i];
    }
    
    Knot& operator [] (int i)
    {
        return m_knot[i];
    }

    void EraseKnot(int i)
    {
        if(m_knot.size() <= 2)
        {
            // There must always be at least two points for the spline interpolation to work
            return;
        }
        if(i >= GetNumKnot() || i < 0)
        {
            return;
        }

        std::vector<Knot>::iterator it = m_knot.begin();
        it += i;
        m_knot.erase(it);
    }
    
    void EraseSelectedKnot()
    {
        if(m_knot.size() <= 2)
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
            std::vector<Knot>::iterator it;
            for(it = m_knot.begin(); it != m_knot.end(); it++)
            {
                if((*it).m_IsSelected)
                {
                    m_knot.erase(it);
                    found = TRUE;
                    break;
                }
            }
            if(!found)
                finish = TRUE;
        } while((!finish) && (m_knot.size() > 2));
    }

    void SelectKnot(int i, bool b)
    {
        m_knot[i].m_IsSelected = b;
    }

    UINT GetNumSelectedKnot()
    {
        UINT n = 0;
        std::vector<Knot>::iterator it;
        for(UINT i = 0; i < (UINT)m_knot.size(); i++)
        {
            if(m_knot[i].m_IsSelected)
                n++;
        }
        return n;
    }
    
    void SelectAllKnot()
    {
        std::vector<Knot>::iterator it;
        for(it = m_knot.begin(); it != m_knot.end(); it++)
        {
            (*it).m_IsSelected = TRUE;
        }
    }

    void UnSelectAllKnot()
    {
        std::vector<Knot>::iterator it;
        for(it = m_knot.begin(); it != m_knot.end(); it++)
        {
            (*it).m_IsSelected = false;
        }
    }

    const std::vector<double> GetXArray()
    {
        std::vector<double> array;
        std::vector<Knot>::iterator it;
        for(it = m_knot.begin(); it != m_knot.end(); it++)
        {
            if(!(*it).m_IsExcluded)
                array.push_back((*it).x);
        }
        return array;
    }


    const std::vector<double> GetYArray()
    {
        std::vector<double> array;
        std::vector<Knot>::iterator it;
        for(it = m_knot.begin(); it != m_knot.end(); it++)
        {
            if(!(*it).m_IsExcluded)
                array.push_back((*it).y);
        }
        return array;
    }

    int GetNumKnot() const {return (int) m_knot.size();}
    Knot GetKnot(int i) const {return m_knot[i];}

    /*!
        Erase all knots.
    */
    void Reset()
    {
        m_knot.clear();
        AddKnot(0,0);
        AddKnot(1,1);
    }
private:
    std::vector<Knot> m_knot;
};

typedef float (*SplineFunctionCallback)(float);  

class SplineCurveEditor : public ActiveInterfaceObject
{
public:
    SplineCurveEditor(NUX_FILE_LINE_PROTO);
    ~SplineCurveEditor();
    void SetControlPoints(const SplineKnot& splineKnot);
    const SplineKnot& GetControlPoints() const;
    virtual long ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void Draw(GraphicsContext& GfxContext, bool force_draw);
    virtual void DrawContent(GraphicsContext& GfxContext, bool force_draw);
    virtual void PostDraw(GraphicsContext& GfxContext, bool force_draw);
    
    double Eval(double t);

    void RecvMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);
    void RecvKeyEvent(
        GraphicsContext& GfxContext , /*Graphics Context for text operation*/
        unsigned long    eventType  , /*event type*/
        unsigned long    keysym     , /*event keysym*/
        unsigned long    state      , /*event state*/
        const char*      character  , /*character*/
        bool             isRepeated , /*true if the key is repeated more than once*/
        unsigned short   keyCount     /*key repeat count*/
        );

    sigc::signal< void, const smptr(SplineCurveEditor) > sigCurveChange;

    /*!
        Erase all knots.
    */
    void Reset();

    int GetNumKnot() const {return (int) m_control_knot.GetNumKnot();}
    SplineKnot::Knot GetKnot(int i) const {return m_control_knot.GetKnot(i);}
    void AddKnot(double x, double y, bool selected = false);

protected:
    void InitializeWidgets();
    void InitializeLayout();
    void DestroyLayout();

private:
    NTexture2D* m_CheckboardPattern;
    TRefGL<IOpenGLTexture2D> m_Texture;
    GLSh_DrawFunction *m_DrawFunctionShader;
    AbstractPaintLayer* m_Background;

    void SetXAxisBounds(float minX, float maxX);
    void SetYAxisBounds(float minY, float maxY);
    void SetFunctionCallback(SplineFunctionCallback f);
    float EvalFunction(float x);
    void UpdateGraph();

    SplineKnot m_control_knot;

    float m_minX, m_minY, m_maxX, m_maxY;
    SplineFunctionCallback m_FunctionCallback;

    float hit_point_dx;
    float hit_point_dy;

    CubicSpline m_CubicSpline;
};

NAMESPACE_END_GUI

#endif // SPLINECURVEEDITOR_H
