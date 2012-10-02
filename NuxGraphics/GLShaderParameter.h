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


#ifndef GLSHADERPARAMETER_H
#define GLSHADERPARAMETER_H

#include "GLError.h"
#include "GLResource.h"

namespace nux
{

//! Type of shader in a shader
  enum eShaderParameterType
  {
    eVERTEXUNIFORMTYPE,
    eFRAGMENTUNIFORMTYPE,
    eSAMPLERUNIFORMTYPE,
  };

//! Type of shader
  enum eShaderType
  {
    eVERTEXSHADERTYPE,
    eFRAGMENTSHADERTYPE
  };
  class GLProgramObject;
  class IOpenGLShaderProgram;

  class GLShaderParameter
  {
  public:
    int                      m_Index;                  // Register m_Index / Attribute m_Index
    eShaderParameterType       m_ShaderParameterType;
    std::string                m_Name;
    bool                       m_bIsOptional;
    bool                       m_bIsStatic;
    bool                       bStaticSet;
    GLProgramObject            *m_ShaderProgram;
    IOpenGLShaderProgram       *m_ShaderProgram2;
    GLShaderParameter          *m_NextParameter;
    UINT                        m_Size;
    UINT                        m_Type;

    GLShaderParameter(GLProgramObject *Shader,
                       const char *ParamName,
                       eShaderParameterType InType,
                       bool InbIsOptional = FALSE,
                       bool InbIsStatic = FALSE);

    inline void SetUniform1f( FLOAT FloatA )
    {
      NUX_RETURN_IF_TRUE(m_Index == -1);
      CHECKGL(glUniform1fARB( m_Index, FloatA ));
    }
    inline void SetUniform1i( INT i )
    {
      NUX_RETURN_IF_TRUE(m_Index == -1);
      CHECKGL(glUniform1iARB( m_Index, i ));
    }
    inline void SetUniform2f( FLOAT FloatA, FLOAT FloatB )
    {
      NUX_RETURN_IF_TRUE(m_Index == -1);
      CHECKGL(glUniform2fARB( m_Index, FloatA, FloatB ));
    }
    inline void SetUniform3f( FLOAT FloatA, FLOAT FloatB, FLOAT FloatC )
    {
      NUX_RETURN_IF_TRUE(m_Index == -1);
      CHECKGL(glUniform3fARB( m_Index, FloatA, FloatB, FloatC ));
    }
    inline void SetUniform4f( FLOAT FloatA, FLOAT FloatB, FLOAT FloatC, FLOAT FloatD )
    {
      NUX_RETURN_IF_TRUE(m_Index == -1);
      CHECKGL(glUniform4fARB( m_Index, FloatA, FloatB, FloatC, FloatD ));
    }

    inline void SetUniform1fv( GLsizei count, GLfloat *value )
    {
      NUX_RETURN_IF_TRUE(m_Index == -1);
      CHECKGL(glUniform1fvARB( m_Index, count, value ));
    }

    inline void SetUniform2fv( GLsizei count, GLfloat *value )
    {
      NUX_RETURN_IF_TRUE(m_Index == -1);
      CHECKGL(glUniform2fvARB( m_Index, count, value ));
    }

    inline void SetUniform3fv( GLsizei count, GLfloat *value )
    {
      NUX_RETURN_IF_TRUE(m_Index == -1);
      CHECKGL(glUniform3fvARB( m_Index, count, value ));
    }

    inline void SetUniform4fv( GLsizei count, GLfloat *value )
    {
      NUX_RETURN_IF_TRUE(m_Index == -1);
      CHECKGL(glUniform4fvARB( m_Index, count, value ));
    }

    inline void SetUniformMatrix2fv( GLsizei count, GLfloat *value, GLboolean transpose = GL_FALSE )
    {
      NUX_RETURN_IF_TRUE(m_Index == -1);
      CHECKGL(glUniformMatrix2fvARB( m_Index, count, transpose, value ));
    }

    inline void SetUniformMatrix3fv( GLsizei count, GLfloat *value, GLboolean transpose = GL_FALSE )
    {
      NUX_RETURN_IF_TRUE(m_Index == -1);
      CHECKGL(glUniformMatrix3fvARB( m_Index, count, transpose, value ));
    }

    inline void SetUniformMatrix4fv( GLsizei count, GLfloat *value, GLboolean transpose = GL_FALSE )
    {
      NUX_RETURN_IF_TRUE(m_Index == -1);
      CHECKGL(glUniformMatrix4fvARB( m_Index, count, transpose, value ));
    }


    inline void SetTexture( const GLuint /* textureId */)
    {
      //CHECKGL(glUniform1iARB( m_Index, textureId ));
      //CHECKGL(cgGLEnableTextureParameter( CgParameter ));
    }

    void MapTo( GLProgramObject *Shader );

  private:

  };
}

#endif // GLSHADERPARAMETER_H

