#ifndef COLORFUNCTIONS_H
#define COLORFUNCTIONS_H

#include "NSystemTypes.h"
///////////////////////////////////////////////////////////////////////////
//
// Gamma correction
//
///////////////////////////////////////////////////////////////////////////
NAMESPACE_BEGIN

t_float Undo_Gamma_Correction(t_float c);
t_float Do_Gamma_Correction(t_float c);

NAMESPACE_END
#endif // COLORFUNCTIONS_H
