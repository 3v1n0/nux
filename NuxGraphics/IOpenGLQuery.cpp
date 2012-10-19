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
#include "GpuDevice.h"

#include "GLDeviceObjects.h"
#include "IOpenGLQuery.h"

namespace nux
{

  NUX_IMPLEMENT_OBJECT_TYPE(IOpenGLQuery);

  unsigned int IOpenGLQuery::_CurrentlyActiveQuery = 0;

  IOpenGLQuery::IOpenGLQuery(QUERY_TYPE Type)
    : IOpenGLResource(RTQUERY)
    , _Type(Type)
    , _QueryStarted(false)
  {
#ifndef NUX_OPENGLES_20
    CHECKGL(glGenQueriesARB(1, &_OpenGLID)) ;
#endif
  }

// The return type identifies the query state(see Queries).
// The method returns 1(S_OK) if the query data is available and 0(S_FALSE) if it is not.
// These are considered successful return values.
  int IOpenGLQuery::GetData(
    int *pData,
    unsigned int /* Size */,
    unsigned int /* GetDataFlags */
  )
  {
#ifndef NUX_OPENGLES_20
    unsigned int ResultReady = 0;
    glGetQueryObjectuivARB(_OpenGLID, GL_QUERY_RESULT_AVAILABLE_ARB, &ResultReady);
    CHECKGL_MSG( glGetQueryObjectuivARB );

    if (ResultReady)
    {
      glGetQueryObjectuivARB(_OpenGLID, GL_QUERY_RESULT_ARB, (GLuint *) pData);
      CHECKGL_MSG( glGetQueryObjectuivARB );
      return 1;
    }
#endif

    return 0;
  }

  unsigned int IOpenGLQuery::GetDataSize()
  {
    return 0;
  }

  void IOpenGLQuery::GetDevice(GpuDevice **ppDevice)
  {
    // Do not call this function.
    *ppDevice = NULL;
  }

  QUERY_TYPE IOpenGLQuery::GetType()
  {
    return _Type;
  }

  void IOpenGLQuery::Issue(
    unsigned int IssueFlags
  )
  {
#ifndef NUX_OPENGLES_20
    if (IssueFlags == (unsigned int) ISSUE_BEGIN)
    {
      nuxAssert(_CurrentlyActiveQuery == 0);

      if (_QueryStarted == true)
      {
        nuxError("The Query as already been activated");
      }
      else
      {
        _QueryStarted = true;
        CHECKGL(glBeginQueryARB(GL_SAMPLES_PASSED_ARB, _OpenGLID));
        _CurrentlyActiveQuery = _OpenGLID;
      }
    }
    else if (IssueFlags == (unsigned int) ISSUE_END)
    {
      nuxAssert(_CurrentlyActiveQuery == _OpenGLID);

      if (_QueryStarted == false)
      {
        nuxError("The Query as already been stoped");
      }
      else
      {
        _QueryStarted = false;
        CHECKGL(glEndQueryARB(GL_SAMPLES_PASSED_ARB));
        _CurrentlyActiveQuery = 0;
      }
    }
#endif
  }

// Return True is the result is available. That is glGetQueryObjectuivARB won't block
// if called with GL_QUERY_RESULT_ARB.
  bool IOpenGLQuery::IsResultAvailable()
  {
    unsigned int ResultReady = 0;
#ifndef NUX_OPENGLES_20
    glGetQueryObjectuivARB(_OpenGLID, GL_QUERY_RESULT_AVAILABLE_ARB, &ResultReady);
    CHECKGL_MSG( glGetQueryObjectuivARB );
#endif

    return ResultReady != 0;
  }
// Return the result of the query. Make sure IsResultAvailable returned TRUE before calling this function.
// If you fail to do that, GetResult will block before returning.
  unsigned int IOpenGLQuery::GetResult()
  {
    unsigned int SamplesPassed = 0;
#ifndef NUX_OPENGLES_20
    glGetQueryObjectuivARB(_OpenGLID, GL_QUERY_RESULT_ARB, &SamplesPassed);
    CHECKGL_MSG( glGetQueryObjectuivARB );
#endif

    return SamplesPassed;
  }

}

