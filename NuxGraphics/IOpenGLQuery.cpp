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


#include "GLResource.h"
#include "GLDeviceFactory.h"

#include "GLDeviceObjects.h"
#include "IOpenGLQuery.h"

namespace nux { //NUX_NAMESPACE_BEGIN

NUX_IMPLEMENT_OBJECT_TYPE(IOpenGLQuery);

t_u32 IOpenGLQuery::_CurrentlyActiveQuery = 0;

IOpenGLQuery::IOpenGLQuery(QUERY_TYPE Type)
: IOpenGLResource(RTQUERY)
, _Type(Type)
, _QueryStarted(false)
{
    CHECKGL( glGenQueriesARB(1, &_OpenGLID)) ;
}

// The return type identifies the query state (see Queries).
// The method returns 1 (S_OK) if the query data is available and 0 (S_FALSE) if it is not.
// These are considered successful return values. 
int IOpenGLQuery::GetData(
                          int* pData,
                          t_u32 Size,
                          t_u32 GetDataFlags
                          )
{
    unsigned int ResultReady = 0;
    glGetQueryObjectuivARB(_OpenGLID, GL_QUERY_RESULT_AVAILABLE_ARB, &ResultReady);
    CHECKGL_MSG( glGetQueryObjectuivARB );

    if(ResultReady)
    {
        glGetQueryObjectuivARB(_OpenGLID, GL_QUERY_RESULT_ARB, (GLuint*)pData);
        CHECKGL_MSG( glGetQueryObjectuivARB );
        return 1;
    }
    else
    {
        return 0;
    }
}

t_u32 IOpenGLQuery::GetDataSize()
{
    return 0;
}

void IOpenGLQuery::GetDevice(GLDeviceFactory **ppDevice)
{
    // Do not call this function.
    *ppDevice = NULL;
}

QUERY_TYPE IOpenGLQuery::GetType()
{
    return _Type;
}

void IOpenGLQuery::Issue(
                         t_u32 IssueFlags
                         )
{
    if(IssueFlags == (t_u32)ISSUE_BEGIN)
    {
        nuxAssert(_CurrentlyActiveQuery == 0);
        if(_QueryStarted == true)
        {
            nuxError(TEXT("The Query as already been activated"));
        }
        else
        {
            _QueryStarted = true;
            CHECKGL( glBeginQueryARB(GL_SAMPLES_PASSED_ARB, _OpenGLID) );
            _CurrentlyActiveQuery = _OpenGLID;
        }
    }
    else if(IssueFlags == (t_u32)ISSUE_END)
    {
        nuxAssert(_CurrentlyActiveQuery == _OpenGLID);
        if(_QueryStarted == false)
        {
            nuxError(TEXT("The Query as already been stoped"));
        }
        else
        {
            _QueryStarted = false;
            CHECKGL( glEndQueryARB(GL_SAMPLES_PASSED_ARB) );
            _CurrentlyActiveQuery = 0;
        }
    }
}

// Return True is the result is available. That is glGetQueryObjectuivARB won't block
// if called with GL_QUERY_RESULT_ARB.
bool IOpenGLQuery::IsResultAvailable()
{
    unsigned int ResultReady = 0;
    glGetQueryObjectuivARB(_OpenGLID, GL_QUERY_RESULT_AVAILABLE_ARB, &ResultReady);
    CHECKGL_MSG( glGetQueryObjectuivARB );

    return ResultReady != 0;
}
// Return the result of the query. Make sure IsResultAvailable returned TRUE before calling this function.
// If you fail to do that, GetResult will block before returning.
unsigned int IOpenGLQuery::GetResult()
{
    unsigned int SamplesPassed = 0;
    glGetQueryObjectuivARB(_OpenGLID, GL_QUERY_RESULT_ARB, &SamplesPassed);
    CHECKGL_MSG( glGetQueryObjectuivARB );

    return SamplesPassed;
}

} //NUX_NAMESPACE_END
