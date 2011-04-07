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


#ifndef NYERROR_H
#define NYERROR_H

//#include <iostream>
//#include <error.h>
//#include <errno.h>
//#include <stdio.h>

//void ny_output_error(const char *error_msg);
//void ny_output_system_error(const char *error_msg);
//
//void ny_output_error(const char *error_msg)
//{
//    cout << error_msg << endl;
//}
//
//void ny_output_system_error(const char *error_msg)
//{
//    perror(error_msg);
//}



typedef enum
{
  HR_SUCCESS,
  HR_FAIL,
  HR_INVALID_ARG,
  HR_INVALID_CALL,
  HR_NOT_ENOUGH_MEMORY,

  HR_FILE_NOT_FOUND,
  HR_FILE_IO_FAIL,
  HR_UNSUPPORTED_FORMAT,

  HR_UNKNOW_ERROR
} HReport;

#endif // NYERROR_H


