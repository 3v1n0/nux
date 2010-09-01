#include "NKernel.h"
#include "NSystemTypes.h"
#include "ColorFunctions.h"


NAMESPACE_BEGIN
// Converts from Gamma2.2 space to Gamma1.0 space
t_float Undo_Gamma_Correction(t_float c)
{
    if(c <= 0.03928f)     
    {                      
      return (c / 12.92f);     
    }                      
    else                   
    {                      
      return (float)pow((c + 0.055f) / 1.055f, 2.4f);
    }                                            
}
  
t_float Do_Gamma_Correction(t_float c)
{
    if(c <= 0.00304f)     
    {                      
      return (c * 12.92f);     
    }                      
    else                   
    {                      
      return (float)pow(c, 1.0f/2.4f) * 1.055f - 0.055f;
    }                                            
}

NAMESPACE_END