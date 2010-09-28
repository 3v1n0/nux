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


#ifndef GLSHADERPARAMETER_H
#define GLSHADERPARAMETER_H

#include "GLError.h"
#include "GLResource.h"

namespace nux { //NUX_NAMESPACE_BEGIN

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
    GLuint                      m_Index;                  // Register m_Index / Attribute m_Index
    eShaderParameterType        m_ShaderParameterType;
    NString                     m_Name;
    bool                       m_bIsOptional;
    bool                       m_bIsStatic;
    bool                       bStaticSet;
    GLProgramObject*            m_ShaderProgram;
    IOpenGLShaderProgram*       m_ShaderProgram2;
    GLShaderParameter*          m_NextParameter;
    UINT                        m_Size;
    UINT                        m_Type;

    GLShaderParameter(GLProgramObject* Shader,
        const TCHAR* ParamName,
        eShaderParameterType InType,
        bool InbIsOptional=FALSE,
        bool InbIsStatic=FALSE);

    inline void SetUniform1f( FLOAT FloatA )
    {
        CHECKGL( glUniform1fARB( m_Index, FloatA ) );
    }
    inline void SetUniform1i( INT i )
    {
        CHECKGL( glUniform1iARB( m_Index, i ) );
    }
    inline void SetUniform2f( FLOAT FloatA, FLOAT FloatB )
    {
        CHECKGL( glUniform2fARB( m_Index, FloatA, FloatB ) );
    }
    inline void SetUniform3f( FLOAT FloatA, FLOAT FloatB, FLOAT FloatC )
    {
        CHECKGL( glUniform3fARB( m_Index, FloatA, FloatB, FloatC ) );
    }
    inline void SetUniform4f( FLOAT FloatA, FLOAT FloatB, FLOAT FloatC, FLOAT FloatD )
    {
        CHECKGL( glUniform4fARB( m_Index, FloatA, FloatB, FloatC, FloatD ) );
    }

    inline void SetUniform1fv( GLsizei count, GLfloat *value )
    {
        CHECKGL( glUniform1fvARB( m_Index, count, value ) );
    }

    inline void SetUniform2fv( GLsizei count, GLfloat *value )
    {
        CHECKGL( glUniform2fvARB( m_Index, count, value ) );
    }

    inline void SetUniform3fv( GLsizei count, GLfloat *value )
    {
        CHECKGL( glUniform3fvARB( m_Index, count, value ) );
    }

    inline void SetUniform4fv( GLsizei count, GLfloat *value )
    {
        CHECKGL( glUniform4fvARB( m_Index, count, value ) );
    }

    inline void SetUniformMatrix2fv( GLsizei count, GLfloat *value, GLboolean transpose = GL_FALSE )
    {
        CHECKGL( glUniformMatrix2fvARB( m_Index, count, transpose, value ) );
    }

    inline void SetUniformMatrix3fv( GLsizei count, GLfloat *value, GLboolean transpose = GL_FALSE )
    {
        CHECKGL( glUniformMatrix3fvARB( m_Index, count, transpose, value ) );
    }

    inline void SetUniformMatrix4fv( GLsizei count, GLfloat *value, GLboolean transpose = GL_FALSE )
    {
        CHECKGL( glUniformMatrix4fvARB( m_Index, count, transpose, value ) );
    }


	inline void SetTexture( const GLuint textureId )	
	{
		//CHECKGL( glUniform1iARB( m_Index, textureId ) );
		//CHECKGL( cgGLEnableTextureParameter( CgParameter ) );
	}

	void MapTo( GLProgramObject* Shader );

private:

};
} //NUX_NAMESPACE_END

#endif // GLSHADERPARAMETER_H

