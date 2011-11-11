#pragma once

#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include "Wincodec.h"

/******************************************************************
*                                                                 *
*  CustomTextRenderer                                             *
*                                                                 *
*  The IDWriteTextRenderer interface is an input parameter to     *
*  IDWriteTextLayout::Draw.  This interfaces defines a number of  *
*  callback functions that the client application implements for  *
*  custom text rendering.  This sample renderer implementation    *
*  renders text using text outlines and Direct2D.                 *
*  A more sophisticated client would also support bitmap          *
*  renderings.                                                    *
*                                                                 *
******************************************************************/

class CustomTextRenderer : public IDWriteTextRenderer
{
public:
    CustomTextRenderer(
        ID2D1Factory* pD2DFactory, 
        ID2D1RenderTarget* pRT, 
        ID2D1SolidColorBrush* pOutlineBrush, 
        ID2D1BitmapBrush* pFillBrush
        );

    ~CustomTextRenderer();

    IFACEMETHOD(IsPixelSnappingDisabled)(
        __maybenull void* clientDrawingContext,
        __out BOOL* isDisabled
        );

    IFACEMETHOD(GetCurrentTransform)(
        __maybenull void* clientDrawingContext,
        __out DWRITE_MATRIX* transform
        );

    IFACEMETHOD(GetPixelsPerDip)(
        __maybenull void* clientDrawingContext,
        __out FLOAT* pixelsPerDip
        );

    IFACEMETHOD(DrawGlyphRun)(
        __maybenull void* clientDrawingContext,
        FLOAT baselineOriginX,
        FLOAT baselineOriginY,
        DWRITE_MEASURING_MODE measuringMode,
        __in DWRITE_GLYPH_RUN const* glyphRun,
        __in DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
        IUnknown* clientDrawingEffect
        );

    IFACEMETHOD(DrawUnderline)(
        __maybenull void* clientDrawingContext,
        FLOAT baselineOriginX,
        FLOAT baselineOriginY,
        __in DWRITE_UNDERLINE const* underline,
        IUnknown* clientDrawingEffect
        );

    IFACEMETHOD(DrawStrikethrough)(
        __maybenull void* clientDrawingContext,
        FLOAT baselineOriginX,
        FLOAT baselineOriginY,
        __in DWRITE_STRIKETHROUGH const* strikethrough,
        IUnknown* clientDrawingEffect
        );

    IFACEMETHOD(DrawInlineObject)(
        __maybenull void* clientDrawingContext,
        FLOAT originX,
        FLOAT originY,
        IDWriteInlineObject* inlineObject,
        BOOL isSideways,
        BOOL isRightToLeft,
        IUnknown* clientDrawingEffect
        );

public:
    IFACEMETHOD_(unsigned long, AddRef) ();
    IFACEMETHOD_(unsigned long,  Release) ();
    IFACEMETHOD(QueryInterface) (
        IID const& riid,
        void** ppvObject
    );

private:
    unsigned long cRefCount_;
    ID2D1Factory* pD2DFactory_;
    ID2D1RenderTarget* pRT_;
    ID2D1SolidColorBrush* pOutlineBrush_;
    ID2D1BitmapBrush* pFillBrush_;
};

