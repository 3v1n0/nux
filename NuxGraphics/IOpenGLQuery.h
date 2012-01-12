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


#ifndef IOPENGLQUERY_H
#define IOPENGLQUERY_H

namespace nux
{

  class IOpenGLResource;
  class IOpenGLQuery: public IOpenGLResource
  {
    NUX_DECLARE_OBJECT_TYPE(IOpenGLQuery, IOpenGLResource);

  public:
    int GetData(
      int *pData,
      unsigned int Size,
      unsigned int GetDataFlags
    );
    unsigned int GetDataSize();

    void GetDevice(
      GpuDevice **ppDevice
    );

    QUERY_TYPE GetType();

    void Issue(
      unsigned int IssueFlags
    );

    // Convenient functions

    // Return True is the result is available. That is glGetQueryObjectuivARB won't block
    // if called with GL_QUERY_RESULT_ARB.
    bool IsResultAvailable();
    // Return the result of the query. Make sure IsResultAvailable returned TRUE before calling this function.
    // If you fail to do that, GetResult will block before returning.
    unsigned int GetResult();

    IOpenGLQuery(QUERY_TYPE Type);
    virtual ~IOpenGLQuery() {};


  private:
    QUERY_TYPE _Type;
    bool _QueryStarted;
    friend class GpuDevice;
    static unsigned int _CurrentlyActiveQuery;
  };

}

#endif // IOPENGLQUERY_H
