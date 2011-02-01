/*
 * Copyright 2010 Inalogic Inc.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the  Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * version 3 along with this program.  If not, see
 * <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#include "NuxCore/NuxCore.h"
#include "NuxImage/BitmapFormats.h"
#include "NuxGraphics/GraphicsDisplay.h"
#include "NuxGraphics/GLWindowManager.h"
#include "NuxGraphics/GraphicsEngine.h"

/*
 * Tests: 
 *  - Display arb programs limits
 */
 
void ARBProgramLimits ()
{
  nux::GraphicsDisplay* graphics_display = gGLWindowManager.CreateGLWindow("Window", 570, 270, nux::WINDOWSTYLE_NORMAL, 0, false);


  int OPENGL_MAX_PROGRAM_INSTRUCTIONS_ARB;
  int OPENGL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB;
  int OPENGL_MAX_PROGRAM_TEMPORARIES_ARB;
  int OPENGL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB;
  int OPENGL_MAX_PROGRAM_PARAMETERS_ARB;
  int OPENGL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB;
  int OPENGL_MAX_PROGRAM_ATTRIBS_ARB;
  int OPENGL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB;
  int OPENGL_MAX_PROGRAM_ADDRESS_REGISTERS_ARB;
  int OPENGL_MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB;
  int OPENGL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB;
  int OPENGL_MAX_PROGRAM_ENV_PARAMETERS_ARB;
  int OPENGL_MAX_PROGRAM_ALU_INSTRUCTIONS_ARB;
  int OPENGL_MAX_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB;
  int OPENGL_MAX_PROGRAM_TEX_INSTRUCTIONS_ARB;
  int OPENGL_MAX_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB;
  int OPENGL_MAX_PROGRAM_TEX_INDIRECTIONS_ARB;
  int OPENGL_MAX_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB;

  
  CHECKGL (glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_INSTRUCTIONS_ARB,                &OPENGL_MAX_PROGRAM_INSTRUCTIONS_ARB) );
  CHECKGL (glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB,         &OPENGL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB) );
  CHECKGL (glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_TEMPORARIES_ARB,                 &OPENGL_MAX_PROGRAM_TEMPORARIES_ARB) );
  CHECKGL (glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB,          &OPENGL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB) );
  CHECKGL (glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_PARAMETERS_ARB,                  &OPENGL_MAX_PROGRAM_PARAMETERS_ARB) );
  CHECKGL (glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB,           &OPENGL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB) );
  CHECKGL (glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_ATTRIBS_ARB,                     &OPENGL_MAX_PROGRAM_ATTRIBS_ARB) );
  CHECKGL (glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB,              &OPENGL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB) );
  CHECKGL (glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_ADDRESS_REGISTERS_ARB,           &OPENGL_MAX_PROGRAM_ADDRESS_REGISTERS_ARB) );
  CHECKGL (glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB,    &OPENGL_MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB) );
  CHECKGL (glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB,            &OPENGL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB) );
  CHECKGL (glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_ENV_PARAMETERS_ARB,              &OPENGL_MAX_PROGRAM_ENV_PARAMETERS_ARB) );
  CHECKGL (glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_ALU_INSTRUCTIONS_ARB,            &OPENGL_MAX_PROGRAM_ALU_INSTRUCTIONS_ARB) );
  CHECKGL (glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB,     &OPENGL_MAX_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB) );
  CHECKGL (glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_TEX_INSTRUCTIONS_ARB,            &OPENGL_MAX_PROGRAM_TEX_INSTRUCTIONS_ARB) );
  CHECKGL (glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB,     &OPENGL_MAX_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB) );
  CHECKGL (glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_TEX_INDIRECTIONS_ARB,            &OPENGL_MAX_PROGRAM_TEX_INDIRECTIONS_ARB) );
  CHECKGL (glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB,     &OPENGL_MAX_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB) );
  
  nux::LogOutputSeverityMessage(nux::NUX_MSG_SEVERITY_NONE, "---------------------------------------------");
  nux::LogOutputSeverityMessage(nux::NUX_MSG_SEVERITY_NONE, "Vertex Program");
  nux::LogOutputSeverityMessage(nux::NUX_MSG_SEVERITY_NONE, "---------------------------------------------");
  nux::LogOutputSeverityMessage(nux::NUX_MSG_SEVERITY_INFO, "MAX_PROGRAM_INSTRUCTIONS_ARB             = %d",  OPENGL_MAX_PROGRAM_INSTRUCTIONS_ARB);
  nux::LogOutputSeverityMessage(nux::NUX_MSG_SEVERITY_INFO, "MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB      = %d",  OPENGL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB);
  nux::LogOutputSeverityMessage(nux::NUX_MSG_SEVERITY_INFO, "MAX_PROGRAM_TEMPORARIES_ARB              = %d",  OPENGL_MAX_PROGRAM_TEMPORARIES_ARB);
  nux::LogOutputSeverityMessage(nux::NUX_MSG_SEVERITY_INFO, "MAX_PROGRAM_NATIVE_TEMPORARIES_ARB       = %d",  OPENGL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB);
  nux::LogOutputSeverityMessage(nux::NUX_MSG_SEVERITY_INFO, "MAX_PROGRAM_PARAMETERS_ARB               = %d",  OPENGL_MAX_PROGRAM_PARAMETERS_ARB);
  nux::LogOutputSeverityMessage(nux::NUX_MSG_SEVERITY_INFO, "MAX_PROGRAM_NATIVE_PARAMETERS_ARB        = %d",  OPENGL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB);
  nux::LogOutputSeverityMessage(nux::NUX_MSG_SEVERITY_INFO, "MAX_PROGRAM_ATTRIBS_ARB                  = %d",  OPENGL_MAX_PROGRAM_ATTRIBS_ARB);
  nux::LogOutputSeverityMessage(nux::NUX_MSG_SEVERITY_INFO, "MAX_PROGRAM_NATIVE_ATTRIBS_ARB           = %d",  OPENGL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB);
  nux::LogOutputSeverityMessage(nux::NUX_MSG_SEVERITY_INFO, "MAX_PROGRAM_ADDRESS_REGISTERS_ARB        = %d",  OPENGL_MAX_PROGRAM_ADDRESS_REGISTERS_ARB);
  nux::LogOutputSeverityMessage(nux::NUX_MSG_SEVERITY_INFO, "MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB = %d",  OPENGL_MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB);
  nux::LogOutputSeverityMessage(nux::NUX_MSG_SEVERITY_INFO, "MAX_PROGRAM_LOCAL_PARAMETERS_ARB         = %d",  OPENGL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB);
  nux::LogOutputSeverityMessage(nux::NUX_MSG_SEVERITY_INFO, "MAX_PROGRAM_ENV_PARAMETERS_ARB           = %d",  OPENGL_MAX_PROGRAM_ENV_PARAMETERS_ARB);
  nux::LogOutputSeverityMessage(nux::NUX_MSG_SEVERITY_INFO, "MAX_PROGRAM_ALU_INSTRUCTIONS_ARB         = %d",  OPENGL_MAX_PROGRAM_ALU_INSTRUCTIONS_ARB);
  nux::LogOutputSeverityMessage(nux::NUX_MSG_SEVERITY_INFO, "MAX_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB  = %d",  OPENGL_MAX_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB);
  nux::LogOutputSeverityMessage(nux::NUX_MSG_SEVERITY_INFO, "MAX_PROGRAM_TEX_INSTRUCTIONS_ARB         = %d",  OPENGL_MAX_PROGRAM_TEX_INSTRUCTIONS_ARB);
  nux::LogOutputSeverityMessage(nux::NUX_MSG_SEVERITY_INFO, "MAX_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB  = %d",  OPENGL_MAX_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB);
  nux::LogOutputSeverityMessage(nux::NUX_MSG_SEVERITY_INFO, "MAX_PROGRAM_TEX_INDIRECTIONS_ARB         = %d",  OPENGL_MAX_PROGRAM_TEX_INDIRECTIONS_ARB);
  nux::LogOutputSeverityMessage(nux::NUX_MSG_SEVERITY_INFO, "MAX_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB  = %d",  OPENGL_MAX_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB);
  
  CHECKGL (glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_INSTRUCTIONS_ARB,                &OPENGL_MAX_PROGRAM_INSTRUCTIONS_ARB) );
  CHECKGL (glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB,         &OPENGL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB) );
  CHECKGL (glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_TEMPORARIES_ARB,                 &OPENGL_MAX_PROGRAM_TEMPORARIES_ARB) );
  CHECKGL (glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB,          &OPENGL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB) );
  CHECKGL (glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_PARAMETERS_ARB,                  &OPENGL_MAX_PROGRAM_PARAMETERS_ARB) );
  CHECKGL (glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB,           &OPENGL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB) );
  CHECKGL (glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_ATTRIBS_ARB,                     &OPENGL_MAX_PROGRAM_ATTRIBS_ARB) );
  CHECKGL (glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB,              &OPENGL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB) );
  CHECKGL (glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_ADDRESS_REGISTERS_ARB,           &OPENGL_MAX_PROGRAM_ADDRESS_REGISTERS_ARB) );
  CHECKGL (glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB,    &OPENGL_MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB) );
  CHECKGL (glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB,            &OPENGL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB) );
  CHECKGL (glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_ENV_PARAMETERS_ARB,              &OPENGL_MAX_PROGRAM_ENV_PARAMETERS_ARB) );
  CHECKGL (glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_ALU_INSTRUCTIONS_ARB,            &OPENGL_MAX_PROGRAM_ALU_INSTRUCTIONS_ARB) );
  CHECKGL (glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB,     &OPENGL_MAX_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB) );
  CHECKGL (glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_TEX_INSTRUCTIONS_ARB,            &OPENGL_MAX_PROGRAM_TEX_INSTRUCTIONS_ARB) );
  CHECKGL (glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB,     &OPENGL_MAX_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB) );
  CHECKGL (glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_TEX_INDIRECTIONS_ARB,            &OPENGL_MAX_PROGRAM_TEX_INDIRECTIONS_ARB) );
  CHECKGL (glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB,     &OPENGL_MAX_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB) );
  
  
  nux::LogOutputSeverityMessage(nux::NUX_MSG_SEVERITY_NONE, "---------------------------------------------");
  nux::LogOutputSeverityMessage(nux::NUX_MSG_SEVERITY_NONE, "Fragment Program");
  nux::LogOutputSeverityMessage(nux::NUX_MSG_SEVERITY_NONE, "---------------------------------------------");
  nux::LogOutputSeverityMessage(nux::NUX_MSG_SEVERITY_INFO, "MAX_PROGRAM_INSTRUCTIONS_ARB             = %d",  OPENGL_MAX_PROGRAM_INSTRUCTIONS_ARB);
  nux::LogOutputSeverityMessage(nux::NUX_MSG_SEVERITY_INFO, "MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB      = %d",  OPENGL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB);
  nux::LogOutputSeverityMessage(nux::NUX_MSG_SEVERITY_INFO, "MAX_PROGRAM_TEMPORARIES_ARB              = %d",  OPENGL_MAX_PROGRAM_TEMPORARIES_ARB);
  nux::LogOutputSeverityMessage(nux::NUX_MSG_SEVERITY_INFO, "MAX_PROGRAM_NATIVE_TEMPORARIES_ARB       = %d",  OPENGL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB);
  nux::LogOutputSeverityMessage(nux::NUX_MSG_SEVERITY_INFO, "MAX_PROGRAM_PARAMETERS_ARB               = %d",  OPENGL_MAX_PROGRAM_PARAMETERS_ARB);
  nux::LogOutputSeverityMessage(nux::NUX_MSG_SEVERITY_INFO, "MAX_PROGRAM_NATIVE_PARAMETERS_ARB        = %d",  OPENGL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB);
  nux::LogOutputSeverityMessage(nux::NUX_MSG_SEVERITY_INFO, "MAX_PROGRAM_ATTRIBS_ARB                  = %d",  OPENGL_MAX_PROGRAM_ATTRIBS_ARB);
  nux::LogOutputSeverityMessage(nux::NUX_MSG_SEVERITY_INFO, "MAX_PROGRAM_NATIVE_ATTRIBS_ARB           = %d",  OPENGL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB);
  nux::LogOutputSeverityMessage(nux::NUX_MSG_SEVERITY_INFO, "MAX_PROGRAM_ADDRESS_REGISTERS_ARB        = %d",  OPENGL_MAX_PROGRAM_ADDRESS_REGISTERS_ARB);
  nux::LogOutputSeverityMessage(nux::NUX_MSG_SEVERITY_INFO, "MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB = %d",  OPENGL_MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB);
  nux::LogOutputSeverityMessage(nux::NUX_MSG_SEVERITY_INFO, "MAX_PROGRAM_LOCAL_PARAMETERS_ARB         = %d",  OPENGL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB);
  nux::LogOutputSeverityMessage(nux::NUX_MSG_SEVERITY_INFO, "MAX_PROGRAM_ENV_PARAMETERS_ARB           = %d",  OPENGL_MAX_PROGRAM_ENV_PARAMETERS_ARB);
  nux::LogOutputSeverityMessage(nux::NUX_MSG_SEVERITY_INFO, "MAX_PROGRAM_ALU_INSTRUCTIONS_ARB         = %d",  OPENGL_MAX_PROGRAM_ALU_INSTRUCTIONS_ARB);
  nux::LogOutputSeverityMessage(nux::NUX_MSG_SEVERITY_INFO, "MAX_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB  = %d",  OPENGL_MAX_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB);
  nux::LogOutputSeverityMessage(nux::NUX_MSG_SEVERITY_INFO, "MAX_PROGRAM_TEX_INSTRUCTIONS_ARB         = %d",  OPENGL_MAX_PROGRAM_TEX_INSTRUCTIONS_ARB);
  nux::LogOutputSeverityMessage(nux::NUX_MSG_SEVERITY_INFO, "MAX_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB  = %d",  OPENGL_MAX_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB);
  nux::LogOutputSeverityMessage(nux::NUX_MSG_SEVERITY_INFO, "MAX_PROGRAM_TEX_INDIRECTIONS_ARB         = %d",  OPENGL_MAX_PROGRAM_TEX_INDIRECTIONS_ARB);
  nux::LogOutputSeverityMessage(nux::NUX_MSG_SEVERITY_INFO, "MAX_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB  = %d",  OPENGL_MAX_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB);

  delete graphics_display;
}

int main(int argc, char **argv)
{
  nux::NuxCoreInitialize(0);
  nux::NuxGraphicsInitialize();

  ARBProgramLimits();

  return 0;
}

