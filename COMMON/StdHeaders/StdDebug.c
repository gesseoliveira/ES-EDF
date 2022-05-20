/* *****************************************************************************
 FILE_NAME:     StdDebug.c
 DESCRIPTION:   Generic Debug Routines
 DESIGNER:      Andre F. N. Dainese
 CREATION_DATE: 09/Dec/2015
 VERSION:       2.1
********************************************************************************
Version 1.0:  21/mar/2015 - Daniel Rebeschini
              - First version with Debug Halt interface
Version 2.0:  03/apr/2017 - Andre F. N. Dainese
              - Added capability to check whether the uC's debug interface is
                  enabled and if it isn't then the breakpoint command is
                  not called.
Version 2.1:  16/jul/2018 - Andre F. N. Dainese
              - EnableDebugHalt flag is now volatile; without this property the
                  compiler could promote it to a const and, consequently, 
                  blocking from changing its value during a debug session.
***************************************************************************** */

/* *****************************************************************************
 *        INCLUDES (and DEFINES for INCLUDES)
***************************************************************************** */
#include "types.h"

/* *****************************************************************************
 *        GLOBAL VARIABLES
***************************************************************************** */
static volatile bool EnableDebugHalt = FALSE;

/* *****************************************************************************
 *        DEBUG DEFINITIONS
***************************************************************************** */
/* The definitions below sets what the code should do in order to determine   */
/*  if the debug interface is set or not. It'll change according to the core. */
#if   (__CORTEX_M == 0x04)
  #define CHECK_DEBUG                                                       TRUE

  /* Follow the definitions provided by the header files.                     */
  #define DEBUG_REG                                             CoreDebug->DHCSR
  #define DEBUG_MASK                               CoreDebug_DHCSR_C_DEBUGEN_Msk

#elif (__CORTEX_M == 0x00)
  /* Although the Cortex-M0 (ARMv6) has the debug registers, it seems that    */
  /*  they are not accessible by the core and, therefore, it is unable to know*/
  /*  whether the debug peripheral is enabled or not. So, disable the check.  */
  #define CHECK_DEBUG                                                      FALSE
#else
  /* This project uses a core that this code doesn't support. The check will  */
  /*  be disabled.                                                            */
  #define CHECK_DEBUG                                                      FALSE
  #warning "StdDebug: Warning! Core is unknown and debughalt logic is manual!"
#endif

/* *****************************************************************************
 *        ROUTINES
***************************************************************************** */
void __DEBUGHALT(void)
{
  /* This is the generic Debug Halt routine in which, basically, the software */
  /*  breakpoint routine is called. Some protections are made so that the user*/
  /*  has to explicitly enable this call, since that this routine, when used  */
  /*  outside the debugging environment, causes HardFault exceptions.         */

  if( EnableDebugHalt != FALSE )
  {
    /* Check if the debug peripheral is enabled, otherwise do not call the    */
    /*  breakpoint logic.                                                     */
  #if CHECK_DEBUG != FALSE
    if( ( DEBUG_REG & DEBUG_MASK ) != 0 )
  #endif
    {
      /* Call the software breakpoint instruction                             */
      __asm( "BKPT 255");
    }
  }

  return;
}
