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


