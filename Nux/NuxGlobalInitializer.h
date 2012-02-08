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


#ifndef NUXGLOBALINITIALIZER_H
#define NUXGLOBALINITIALIZER_H

#define NUX_NUX_GLOBAL_OBJECT_INIT_SEQUENCE()
//
//    NUX_GLOBAL_OBJECT_VARIABLE(RenderingStats);



namespace nux
{

  class NuxGlobalSingletonInitializer
  {
    NUX_DISABLE_OBJECT_COPY(NuxGlobalSingletonInitializer);
    NuxGlobalSingletonInitializer *operator & ();
    const NuxGlobalSingletonInitializer *operator & () const;

  public:
    NuxGlobalSingletonInitializer();
    ~NuxGlobalSingletonInitializer();

    static bool Ready();
  private:
    static bool m_NuxGlobalObjectsReady;

    NUX_NUX_GLOBAL_OBJECT_INIT_SEQUENCE();
  };


// Nifty Counter idiom. See http://www-d0.fnal.gov/KAI/doc/tutorials/static_initialization.html
  class NuxGlobalInitializer
  {
  public:
    NuxGlobalInitializer();
    ~NuxGlobalInitializer();
  private:
    static int m_Count;
  };

// Every compilation unit that includes this file will have its own instance of sGlobalInitializer. sGlobalInitializer is initialized
// before the main function of the program is called. The first time sGlobalInitializer is initialized, it calls SystemStart() to create
// our global object singleton. In SystemStart() we have a change to create our singletons in any order we like.
// When the program exits, every instance of sGlobalInitializer will be destroyed. The last instance destroyed will call SystemShutdown().
// In SystemShutdown() we can destroy our global objects in any order we like.

  static NuxGlobalInitializer sNuxGlobalInitializer;


}

#endif // NUXGLOBALINITIALIZER_H
