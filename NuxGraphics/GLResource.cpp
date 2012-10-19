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


#include "GLResource.h"
#include "IOpenGLVertexBuffer.h"

namespace nux
{

  const char *OGLDeviceErrorMessages[] =
  {
    "OGL_OK",
    "OGL_ERROR_UNKNOWN",
    "OGL_INVALID_SURFACE_LEVEL",
    "OGL_INVALID_CALL",
    "OGL_INVALID_LOCK",
    "OGL_INVALID_UNLOCK",
    "OGL_INVALID_TEXTURE",
  };


  /*
      This table lists the most efficient combinations of internalformat(either generic or specific), format, and type:
      internalformat 	            format 	                    type
      ------------------------------------------------------------------------------------
      GL_ALPHA16                  GL_ALPHA                    GL_UNSIGNED_SHORT
      GL_ALPHA8
      or GL_ALPHA                 GL_ALPHA                    GL_UNSIGNED_BYTE
      GL_INTENSITY16 	            GL_INTENSITY                GL_UNSIGNED_SHORT
      GL_INTENSITY8 	            GL_INTENSITY                GL_UNSIGNED_BYTE
      GL_LUMINANCE16              GL_LUMINANCE                GL_UNSIGNED_SHORT
      GL_LUMINANCE8
      or GL_LUMINANCE             GL_LUMINANCE                GL_UNSIGNED_BYTE
      GL_LUMINANCE16_ALPHA16      GL_LUMINANCE_ALPHA 	        GL_UNSIGNED_INT_16_16_REV
      GL_LUMINANCE8_ALPHA8
      or GL_LUMINANCE_ALPHA       GL_LUMINANCE_ALPHA 	        GL_UNSIGNED_SHORT_8_8_REV
      GL_RGB8                     GL_RGBA                     GL_UNSIGNED_INT_8_8_8_8
      GL_RGBA8 or GL_RGBA8        GL_RGBA                     GL_UNSIGNED_INT_8_8_8_8
      GL_ARGB_SCE                 GL_BGRA                     GL_UNSIGNED_INT_8_8_8_8_REV
      GL_RGB16F                   GL_RGBA                     GL_HALF_FLOAT_ARB
      GL_RGBA16F                  GL_RGBA                     GL_HALF_FLOAT_ARB
      GL_LUMINANCE_ALPHA16F_ARB   GL_ALPHA_LUMINANCE_SCE      GL_HALF_FLOAT_ARB
      GL_LUMINANCE32F_ARB         GL_LUMINANCE                GL_FLOAT
      GL_RGB32F_ARB               GL_RGBA                     GL_FLOAT
      GL_RGBA32F_ARB              GL_RGBA                     GL_FLOAT
      GL_DEPTH_COMPONENT24
      or GL_DEPTH_COMPONENT 	    GL_DEPTH_COMPONENT 	        GL_UNSIGNED_INT_24_8_SCE
  */

//
//void AddVertexElement(std::vector<VERTEXELEMENT>& Elements,
//                         WORD Stream,
//                         WORD Offset,
//                         //WORD Stride,
//                         ATTRIB_DECL_TYPE Type,
//                         ATTRIB_USAGE_DECL Usage,
//                         BYTE UsageIndex)
//{
//    VERTEXELEMENT	Element;
//    Element.Stream		= Stream;
//    Element.Offset		= Offset;
//    // We don't want to store ATTRIB_DECL_TYPE. We unpack it here so we don't have to do it every frame.
//    // Opengl commands such as cgGLSetParameterPointer needs to know how many components there are in
//    // a vertex attribute and what is the format of each component.
//    DecomposeTypeDeclaraction(Type, &Element.NumComponent, &Element.Type);
//    //Element.Stride    = Stride;
//    Element.Usage       = Usage;
//    Element.UsageIndex  = UsageIndex;
//    Elements.AddItem(Element);
//}

  void DecomposeTypeDeclaraction(ATTRIB_DECL_TYPE Type, int &NumComponent, ATTRIB_COMPONENT_TYPE &ComponentType)
  {
    switch(Type)
    {
      case ATTRIB_DECLTYPE_FLOAT1:
      {
        NumComponent = 1;
        ComponentType = ATTRIB_CT_FLOAT;
        break;
      }
      case ATTRIB_DECLTYPE_FLOAT2:
      {
        NumComponent = 2;
        ComponentType = ATTRIB_CT_FLOAT;
        break;
      }
      case ATTRIB_DECLTYPE_FLOAT3:
      {
        NumComponent = 3;
        ComponentType = ATTRIB_CT_FLOAT;
        break;
      }
      case ATTRIB_DECLTYPE_FLOAT4:
      {
        NumComponent = 4;
        ComponentType = ATTRIB_CT_FLOAT;
        break;
      }
      case ATTRIB_DECLTYPE_COLOR:
      {
        NumComponent = 4;
        ComponentType = ATTRIB_CT_UNSIGNED_BYTE;
        break;
      }
      case ATTRIB_DECLTYPE_UBYTE4:
      {
        NumComponent = 4;
        ComponentType = ATTRIB_CT_UNSIGNED_BYTE;
        break;
      }
      case ATTRIB_DECLTYPE_SHORT2:
      {
        NumComponent = 2;
        ComponentType = ATTRIB_CT_SHORT;
        break;
      }
      case ATTRIB_DECLTYPE_SHORT4:
      {
        NumComponent = 4;
        ComponentType = ATTRIB_CT_SHORT;
        break;
      }
      case ATTRIB_DECLTYPE_UBYTE4N:
      {
        NumComponent = 4;
        ComponentType = ATTRIB_CT_UNSIGNED_BYTE;
        break;
      }
      case ATTRIB_DECLTYPE_SHORT2N:
      {
        NumComponent = 2;
        ComponentType = ATTRIB_CT_SHORT;
        break;
      }
      case ATTRIB_DECLTYPE_SHORT4N:
      {
        NumComponent = 4;
        ComponentType = ATTRIB_CT_SHORT;
        break;
      }
      case ATTRIB_DECLTYPE_USHORT2N:
      {
        NumComponent = 2;
        ComponentType = ATTRIB_CT_UNSIGNED_SHORT;
        break;
      }
      case ATTRIB_DECLTYPE_USHORT4N:
      {
        NumComponent = 4;
        ComponentType = ATTRIB_CT_UNSIGNED_SHORT;
        break;
      }

#ifndef NUX_OPENGLES_20
      case ATTRIB_DECLTYPE_FLOAT16_2:
      {
        NumComponent = 2;
        ComponentType = ATTRIB_CT_HALF_FLOAT;
        break;
      }
      case ATTRIB_DECLTYPE_FLOAT16_4:
      {
        NumComponent = 4;
        ComponentType = ATTRIB_CT_HALF_FLOAT;
        break;
      }
#endif

      case ATTRIB_DECLTYPE_UNUSED:
      default:
      {
        NumComponent = 0;
        ComponentType = ATTRIB_CT_UNKNOWN;
        nuxError("Unsupported Declaration Type. \n");
      }
    }
  }

  UINT GetVertexElementSize(VERTEXELEMENT vtxelement)
  {
    ATTRIB_COMPONENT_TYPE type = vtxelement.Type;
    UINT NumComponent = vtxelement.NumComponent;

    switch(type)
    {
      case ATTRIB_CT_BYTE:
      case ATTRIB_CT_UNSIGNED_BYTE:
        return 1 * NumComponent;

      case ATTRIB_CT_SHORT:
      case ATTRIB_CT_UNSIGNED_SHORT:
        return 2 * NumComponent;
      case ATTRIB_CT_INT:
      case ATTRIB_CT_UNSIGNED_INT:
      case ATTRIB_CT_FLOAT:
        return 4 * NumComponent;
#ifndef NUX_OPENGLES_20
      case ATTRIB_CT_HALF_FLOAT:
        return 2 * NumComponent;
      case ATTRIB_CT_DOUBLE:
        return 8 * NumComponent;
#endif
      case ATTRIB_CT_UNKNOWN:
      default:
        nuxAssert("Unknown Component Type");
        return 0;
    }

    return 0;
  }

  void AddVertexElement(std::vector<VERTEXELEMENT>& Elements,
                         WORD Stream,
                         WORD Offset,
                         //ubiU16 Stride,
                         ATTRIB_DECL_TYPE Type,
                         ATTRIB_USAGE_DECL /* Usage */,
                         BYTE /* UsageIndex */)
  {
    VERTEXELEMENT	Element;
    Element.Stream		= Stream;
    Element.Offset		= Offset;
    // We don't want to store ATTRIB_DECL_TYPE. We unpack it here so we don't have to do it every frame.
    // Opengl commands such as cgGLSetParameterPointer needs to know how many components there are in
    // a vertex attribute and what is the format of each component.
    DecomposeTypeDeclaraction(Type, Element.NumComponent, Element.Type);
    //Element.Stride    = Stride;
    //Element.Usage       = Usage;
    //Element.UsageIndex  = UsageIndex;
    Elements.push_back(Element);
  }

  unsigned int GetGLElementCount(PRIMITIVE_TYPE InPrimitiveType,
                                  unsigned int           InPrimitiveCount)
  {
    switch(InPrimitiveType)
    {
      case PRIMITIVE_TYPE_POINTLIST:
        return InPrimitiveCount;
      case PRIMITIVE_TYPE_LINELIST:
        return InPrimitiveCount * 2;
      case PRIMITIVE_TYPE_LINESTRIP:
        return InPrimitiveCount + 1;
      case PRIMITIVE_TYPE_TRIANGLELIST:
        return InPrimitiveCount * 3;
      case PRIMITIVE_TYPE_TRIANGLESTRIP:
        return InPrimitiveCount + 2;
      case PRIMITIVE_TYPE_TRIANGLEFAN:
        return InPrimitiveCount;
#ifndef NUX_OPENGLES_20
      case PRIMITIVE_TYPE_QUADLIST:
        return InPrimitiveCount * 4;
      case PRIMITIVE_TYPE_QUADSTRIP:
        return InPrimitiveCount * 2 + 2;
#endif
      default:
        return 0;
    }

    nuxAssertMsg(0, "[GetGLElementCount] Invalid PRIMITIVE_TYPE");
    return InPrimitiveCount;
  }
}
