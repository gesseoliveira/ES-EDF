/* *****************************************************************************
 FILE_NAME:     CustoMath.c
 DESCRIPTION:   Software library with customized math functions
 DESIGNER:      Juliano Varasquim
 CREATION_DATE: 16/nov/2015
 VERSION:       3.3
***************************************************************************** */




/* *****************************************************************************
 *
 *        INCLUDES (and DEFINES for INCLUDES)
 *
***************************************************************************** */
#include "CustoMath.h"
#include <string.h>
#include <time.h>




/* *****************************************************************************
 *
 *        PRIVATE DEFINITIONS AREA
 *
***************************************************************************** */
typedef struct
{
  uint16 Dthres;
  float  P1;
  float  P2;
  float  P3;
  float  P4;
} CNP6_70_t;

/* Use the macro below to properly check if a float value is negative.        */
#define IS_FLOAT_NEGATIVE( FLT )                        ( __signbitf(FLT) != 0 )



/* *****************************************************************************
 *
 *        COMMOM TABLES AREA
 *
***************************************************************************** */

/* The table below is calculated using the spreadsheet "Oil Compensation      */
/* Pars" that is located in the Volumetric library's repository folder. The P */
/* values are dependant on A1, A2, B1, B2 values. For more information, refer */
/* to the document "06-Itamar de Freitas Maciel.pdf", available in the PD     */
/* server.                                                                    */

/* Note: P1 and P3 values have their value multiplied by 1000, because they   */
/* should be multiplied by this constant value in the FCV calculation. More   */
/* details can be found in the equation coding area.                          */
static const CNP6_70_t TableCNP6_70_Pars[] =
{
  /* Dmax[kg/m�]             P1             P2             P3             P4  */
  {          498, -4.5946490000,  0.0061232432, -0.0317075000,  0.0000548397  },
  {          518, -4.4279279000,  0.0057882992, -0.0263545000,  0.0000441695  },
  {          539, -4.2635157000,  0.0054649855, -0.0263294000,  0.0000438862  },
  {          559, -3.9313336000,  0.0048491425, -0.0171988000,  0.0000271198  },
  {          579, -3.5459928000,  0.0041555627, -0.0174082000,  0.0000272053  },
  {          600, -4.4795786000,  0.0057678079, -0.0384017000,  0.0000636946  },
  {          615, -2.4361019000,  0.0023329280, -0.0015651000,  0.0000019239  },
  {          635, -2.2189302000,  0.0019797819, -0.0015670000,  0.0000019270  },
  {          655, -1.9375650000,  0.0015367709, -0.0015694000,  0.0000019308  },
  {          675, -1.8211309000,  0.0013590734, -0.0015704000,  0.0000019323  },
  {          695, -1.7610562000,  0.0012701186, -0.0015709000,  0.0000019331  },
  {          746, -1.8105498000,  0.0013412881, -0.0015705000,  0.0000019325  },
  {          766, -2.2215907000,  0.0018913203, -0.0015669000,  0.0000019277  },
  {          786, -1.9500670000,  0.0015367709, -0.0015693000,  0.0000019308  },
  {          806, -1.7395987000,  0.0012701186, -0.0015711000,  0.0000019331  },
  {          826, -1.5241519000,  0.0010028290, -0.0015730000,  0.0000019354  },
  {          846, -1.3028125000,  0.0007349001, -0.0015749000,  0.0000019377  },
  {          871, -1.1210535000,  0.0005200950, -0.0015765000,  0.0000019396  },
  {          896, -0.9335585000,  0.0003048780, -0.0015781000,  0.0000019414  },
  {          996, -0.7238306000,  0.0000712602, -0.0015799000,  0.0000019435  },
  {         9999, -0.9082062000,  0.0002563515,  0.0074474000, -0.0000071189  },
};
#define TABLE_CNP6_70_COUNT    ( sizeof(TableCNP6_70_Pars) / sizeof(CNP6_70_t) )
#define TABLE_CNP6_70_LASTITEM ( TABLE_CNP6_70_COUNT - 1 )




/* *****************************************************************************
 *
 *        FUNCTIONS AREA
 *
***************************************************************************** */




/* -----------------------------------------------------------------------------
GetfIntDec()
        Gets the float's integer and decimal parts and returns them on
        separate variables
--------------------------------------------------------------------------------
Input:  FloatVar
          Float variable that will be worked on.
Output: IntPart
          Pointer to the variable that will receive the integer part from the
            float variable
        DecPart
          Pointer to the variable that will receive the decimal part from the
            float variable. It'll hold the input's float signal.
Return: ERR_PARAM_RANGE
          The operation could not be performed. Is the float variable a NaN?
        ANSWERED_REQUEST
          The operation has been successfully performed
WARNING: Notice that the IntPart output is unsigned! If the input float is
          negative, the DecPart will hold the signal.
         For example, if -9.1 is given, the result will be:
          IntPart = 9
          DecPart - -0.1
----------------------------------------------------------------------------- */
ReturnCode_t GetfIntDec(float FloatVar, uint32 * IntPart, float * DecPart)
{
  ReturnCode_t  ReturnValue;
  float         FloatWorkVar;

  /* First, check if given float is a NaN                                     */
  if( isnan(FloatVar) == FALSE )
  {
    /* Use the math.h modf routine to perform this operation. When getting the*/
    /*  integer part, though, care must be taken to remove the signal.        */
    *DecPart = modff( FloatVar, &FloatWorkVar );
    if( FloatWorkVar < 0.0 ) { *IntPart = (uint32)(FloatWorkVar * -1.0); }
    else                     { *IntPart = (uint32)(FloatWorkVar);        }

    ReturnValue = ANSWERED_REQUEST;
  }
  else
  { /* NaN! */
    ReturnValue = ERR_PARAM_RANGE;
  }

  /* Job done. */
  return ReturnValue;
}

/* -----------------------------------------------------------------------------
GetdIntDec()
        Gets the double's integer and decimal parts and returns them on
        separate variables
--------------------------------------------------------------------------------
Input:  DoubleVar
          Double variable that will be worked on.
Output: IntPart
          Pointer to the variable that will receive the integer part from the
            float variable
        DecPart
          Pointer to the variable that will receive the decimal part from the
            float variable. It'll hold the input's float signal.
Return: ERR_PARAM_RANGE
          The operation could not be performed. Is the float variable a NaN?
        ANSWERED_REQUEST
          The operation has been successfully performed
WARNING: Notice that the IntPart output is unsigned! If the input float is
          negative, the DecPart will hold the signal.
         For example, if -9.1 is given, the result will be:
          IntPart = 9
          DecPart - -0.1
----------------------------------------------------------------------------- */
ReturnCode_t GetdIntDec(double DoubleVar, uint64 * IntPart, double * DecPart)
{
  ReturnCode_t  ReturnValue;
  double        DoubleWorkVar;

  /* First, check if given double is a NaN                                     */
  if( isnan(DoubleVar) == FALSE )
  {
    /* Use the math.h modf routine to perform this operation. When getting the*/
    /*  integer part, though, care must be taken to remove the signal.        */
    *DecPart = modf( DoubleVar, &DoubleWorkVar );
    if( DoubleWorkVar < 0.0 ) { *IntPart = (uint64)(DoubleWorkVar * -1.0); }
    else                      { *IntPart = (uint64)(DoubleWorkVar);        }

    ReturnValue = ANSWERED_REQUEST;
  }
  else
  { /* NaN! */
    ReturnValue = ERR_PARAM_RANGE;
  }

  /* Job done. */
  return ReturnValue;
}



/* -----------------------------------------------------------------------------
TruncfTo()
        Truncates a float value with a defined number of decimal places.
--------------------------------------------------------------------------------
Input:  DecPlac
          Number of decimal places the float variable will have after the
          truncation process.
Input/
Output: Var
          Pointer to the variables which will be used to the truncation
          processing, as well as, pointer to the memory region that will get
          back the float value truncated.
Return: ERR_PARAM_RANGE
          The quantity of decimal places sent as parameter is not supported to
          the function. So the value of the variables is kept unchanged.
        ANSWERED_REQUEST
          The variable was truncated as expected.
--------------------------------------------------------------------------------
Notes:  Be careful, due the precision limitation of a float variable, the
        truncation process no necessarily will converge to the expected value,
        the only guarantee is the value nearest of the expected one will
        be gotten.
----------------------------------------------------------------------------- */
ReturnCode_t TruncfTo(float *Var, uint16 DecPlac)
  {
  ReturnCode_t Ret;
  int32 VarAsInt;
  float VarAsFloat;
  static const float Factor[] =
      {
      1.0F,
      10.0F,
      100.0F,
      1000.0F,
      10000.0F,
      100000.0F,
      1000000.0F,
      };

  if(DecPlac > (sizeof(Factor)/4))
    {
    /* If the amount of digits received as parameter has no a factor          */
    Ret = ERR_PARAM_RANGE;
    }
  else
    {
    /* If the factor exists into the table                                    */
    VarAsFloat = (*Var);

    VarAsFloat *= Factor[DecPlac];

    VarAsInt = (int32)VarAsFloat;
    VarAsFloat = (float)VarAsInt;

    VarAsFloat /= Factor[DecPlac];

    (*Var) = VarAsFloat;

    Ret = ANSWERED_REQUEST;
    }

  return Ret;
  }




/* -----------------------------------------------------------------------------
Damping()
        Performs the average calculation based on the parameters below.
--------------------------------------------------------------------------------
Input:  NewSample
          Float value which has the value of the newest sample of data.
        BufUsedLen
          Integer value that says the quantity of samples used to performs the
          average calculation.
Input/
Output: Param
          Pointer to the structure of variables used internally during the
          average calculation. The user of this function does not need
          understand deeply the composition of this structure, the only
          requirement is the initialization of the structure during it
          declaration using the macro called "LINKED_TO", available at
          "customath.h".
Return: The average of the previous "BufUsedLen" samples. Is important to say
        that if the quantity of samples still is lower than the value of
        "BufUsedLen", the average is calculated using the current quantity
        of valid samples, so, for example, if the amount of samples sent as
        parameter is 10, but only 3 samples were gotten so far, the average
        calculation will be performed using 3 samples instead of 10.
--------------------------------------------------------------------------------
Notes:  In order to make easier the debug process, the buffer of variables was
        intentionally left to be declared out of the average structure, however
        if a buffer is not linked to the structure as showed in the example
        below, the function WILL NOT WORKS properly.

          static float Buf[20];
          static DampStr_t AvgStr = LINKED_TO(Buf);

        The name and the length of the array are only examples, the only
        requirement is to pass the name of the correct array as argument to the
        macro "LINKED_TO".
---------------------------------------------------------------------------- */
float Damping(float NewSample, uint16 BufUsedLen, DampStr_t *Param)
  {
  float Avg;
  float Sum;
  uint16 i;

  /* First of all some sanity checks...                                       */
  if(BufUsedLen > Param->MaxBufLength)
    {
    /* If the asked length sent as parameter to the function is higher than   */
    /* buffer is able to support...                                           */
    Param->Return = ERR_PARAM_SIZE;
    Avg = NAN;
    }
  else if(isfinite(NewSample) == FALSE)
    {
    /* If the new sample value is Not a Number, or if it is not finite        */
    Param->Return = ERR_PARAM_VALUE;
    Avg = NAN;
    }
  else
    {
    if(BufUsedLen != Param->PrevBufUsedLength)
      {
      /* If the used buffer length is valid, but it was changed since the     */
      /* previous check restarts the suitable variables                       */
      Param->PrevBufUsedLength = BufUsedLen;
      Param->BufIndex = 0;
      Param->ValidSampInTheBuf = 0;
      (void)memset(Param->Buf, 0, (4*Param->MaxBufLength) );
      Param->Return = VARIABLE_UPDATED;
      }
    else
      {
      /* If nothing changed since the previous check                          */
      Param->Return = ANSWERED_REQUEST;
      }

    /* Updates the oldest value and manage the buffer index variable          */
    Param->Buf[ Param->BufIndex++ ] = NewSample;
    if(Param->BufIndex >= BufUsedLen)
      {
      Param->BufIndex = 0;
      }

    /* Updates the counter of valid samples into the buffer                   */
    if(Param->ValidSampInTheBuf < BufUsedLen)
      {
      Param->ValidSampInTheBuf++;
      }

    /* Calculates the average values only if the buffer has valid samples     */
    if(Param->ValidSampInTheBuf == 0)
      {
      Avg = 0.0F;
      }
    else
      {
      Sum = 0;
      for(i=0; i<Param->ValidSampInTheBuf; i++)
        {
        Sum += Param->Buf[i];
        }
      Avg = (Sum / Param->ValidSampInTheBuf);
      }

    }
  /*  */
  return Avg;
  }




/* -----------------------------------------------------------------------------
MovingDamping()
        Performs the moving average calculation based on the parameters below.
        A moving average follows the equation:
            NewAvg = (OldAvg * (Weight - 1)/Weight) + (NewValue / Weight)
--------------------------------------------------------------------------------
Input:  NewSample
          Float value which has the value of the newest sample of data.
Input/
Output: Param
          Pointer to the structure of variables used internally during the
          average calculation. The user of this function does not need
          understand deeply the composition of this structure, the only
          requirement is the initialization of the structure during it
          declaration using the macro called "MOVING_DMP_CFG", available at
          "customath.h".
Return: The current moving average for the set.
--------------------------------------------------------------------------------
Notes:  The user must declare the configuration strucure that the moving damping
        routine will use.
        It is recommended to follow the example below:

          static MovDampStr_t DampingStr = MOVING_AVG_CFG( 30 ) // Weight = 30
----------------------------------------------------------------------------- */
float MovingDamping(float NewSample, MovDampStr_t *Param)
{
  /* The moving average equation is:                                          */
  /*  NewAvg = (OldAvg * (Weight - 1)/Weight) + (NewValue / Weight)           */
  /* The Weight will increase as new samples are added, until reaching the    */
  /*  limit value stated at CntLimit. The component (Weight - 1)/Weight) will */
  /*  be kept calculated in the DmpWeight variable, so that after the count   */
  /*  reaches the limit it no longer has to be calculated.                    */
  float NewAvg;

  /* First, check if the new value is valid                                   */
  if(isfinite(NewSample) == FALSE)
  {
    Param->Return = ERR_PARAM_VALUE;
    NewAvg = NAN;
  }
  else
  {
    /* If ResetDamping is True, clear the CurrAvg and CurrCnt variables.      */
    if( Param->ResetDamping != FALSE )
    {
      Param->CurrAvg = 0.0;
      Param->CurrCnt = 0;
      Param->ResetDamping = FALSE;
      Param->Return = VARIABLE_UPDATED;
    }
    else
    { /* It'll be a regular operation. Return regular value                   */
      Param->Return = ANSWERED_REQUEST;
    }

    /* Check if the current count is not yet in the limit. If it isn't, then  */
    /*  it, along with the memory weight variable, will have to be updated.   */
    if( Param->CurrCnt != Param->CntLimit )
    { /* If parameter is lower than limit, increment it. If it is higher, then*/
      /*  saturate it to the limit                                            */
      if( Param->CurrCnt < Param->CntLimit )  { Param->CurrCnt++;                 }
      else /* Higher */                       { Param->CurrCnt = Param->CntLimit; }

      /* Update the weight variable                                           */
      Param->DmpWeight = (float)( Param->CurrCnt - 1 ) / Param->CurrCnt;
    }

    /* Perform the damping calculation                                        */
    NewAvg = (Param->CurrAvg * Param->DmpWeight) + (NewSample / Param->CurrCnt);
    /* Remember the value for the next process */
    Param->CurrAvg = NewAvg;
  }

  /* Job done */
  return NewAvg;
}




/* -----------------------------------------------------------------------------
UnitConvert()
        Performs the unit conversion of a physical quantity at unit dictated by
        the international system to another one indexed by the received
        parameters.
--------------------------------------------------------------------------------
Input:  OrigValue
          Float value which has the value of the variable represented using the
          international system.
        toUnit
          Index to the unit used to the conversion.
        AngTab
          Pointer to the first element of the angular coefficients table.
        LinTab
          Pointer to the first element of the linear coefficients table.

Return: Value of the original variable converted to the desired unit.
----------------------------------------------------------------------------- */
float UnitConversion(float OrigValue, uint8 toUnit, const float *AngTab, const float *LinTab)
{
  float ConvValue;

  /* Since some of the tables may not be given, it may not be necessary to    */
  /*  perform both linear and angular operations.                             */
  /* The operation that will be performed in this routine will follow the     */
  /*  equation: ConvValue = OrigValue * Ang + Lin                             */

  if(AngTab == NULL)
  {
    /* If the angular coefficient is not used, consider it to be of 1.0.      */
    /* So, the angular block will simply consist of the Original Value, as    */
    /*  it'll be multiplied by one.                                           */
    ConvValue = OrigValue;
  }
  else
  {
    /* If a valid pointer was sent as parameter, the the angular block will   */
    /*  be the product of the original value times the table value.           */
    ConvValue = OrigValue * fabsf( AngTab[toUnit] );
  }

  if(LinTab != NULL)
  { /* If a linear coefficient was given, sum it to the angular block result. */
    ConvValue += LinTab[toUnit];
  }

  if(AngTab[toUnit] < 0.0F)
  { /* If the angular coefficient is negative, a special step is performed so */
    /* the final value is inverted before the returning                       */
    ConvValue = 1.0F / ConvValue;
  }

  /* Job done. ConvValue will hold the unit conversion result.                */
  return ConvValue;
}




/* -----------------------------------------------------------------------------
UnitChange()
        Performs the unit conversion of a physical quantity from the initial
        unit to the final indexed by the received parameters.
        For to do this, the function operates in two steps, in one of the
        received variable is converted from the received unit to the standard
        unit using the equation:
                 ValueAtStdUnit = ( ValueAtIniUnit - Lin ) / Ang
        So, with the value at standard unit, a new calculation is performed, at
        this case to convert the variable to standard unit to the desired unit
        using the follow equation:
                 ValueAtDesiredUnit = ValueAtStdUnit * Ang + Lin
--------------------------------------------------------------------------------
Input:  OrigValue
          Float value which has the value of the variable represented using the
          international system.
        fromUnit
          Index to the unit used as initial unit to the conversion.
        toUnit
          Index to the unit used as final unit to the conversion.
        AngTab
          Pointer to the first element of the angular coefficients table.
        LinTab
          Pointer to the first element of the linear coefficients table.

Return: Value of the original variable converted to the desired unit.
----------------------------------------------------------------------------- */
float UnitChange(float ValueAtIniUnit, uint8 fromUnit, uint8 toUnit, const float *AngTab, const float *LinTab)
  {
  float ValueAtStdUnit;
  float ValueAtDesiredUnit;

  float LinCoefFromUnit     = 0.0F;
  float LinCoefToUnit       = 0.0F;

  float AngCoefFromUnit     = 1.0F;
  float AngCoefToUnit       = 1.0F;

  bool AngCoefFromUnitIsNeg = FALSE;
  bool AngCoefToUnitIsNeg   = FALSE;

  /* The operation is not necessary if both units' indexes are the same.      */
  if( fromUnit == toUnit )
    {
    ValueAtDesiredUnit = ValueAtIniUnit;
    }
  else
    {
    /* Get the linear coefficients used to the conversion                     */
    if(LinTab != NULL)
      {
      LinCoefFromUnit = LinTab[fromUnit];
      LinCoefToUnit = LinTab[toUnit];
      }

    /* Get the angular coefficients used to the conversion                    */
    if(AngTab != NULL)
      {
      AngCoefFromUnit = fabsf( AngTab[fromUnit] );
      if(AngTab[fromUnit] < 0.0F) { AngCoefFromUnitIsNeg = TRUE; }

      AngCoefToUnit = fabsf( AngTab[toUnit] );
      if(AngTab[toUnit] < 0.0F) { AngCoefToUnitIsNeg = TRUE; }
      }

    /* Conversion from initial unit to standard unit                          */
    ValueAtStdUnit = (ValueAtIniUnit - LinCoefFromUnit) / AngCoefFromUnit;
    if(AngCoefFromUnitIsNeg) { ValueAtStdUnit = 1 / ValueAtStdUnit; }

    /* Conversion from standard unit to desired unit                          */
    ValueAtDesiredUnit = ValueAtStdUnit * AngCoefToUnit + LinCoefToUnit;
    if(AngCoefToUnitIsNeg) { ValueAtDesiredUnit = 1 / ValueAtDesiredUnit; }
    }
  /* Prepare the function returns                                             */
  return ValueAtDesiredUnit;
  }




/* -----------------------------------------------------------------------------
floatCheckLimits()
        Test and classifies "PROCESS_VAL" and "PROCESS_MATRIX" in accordance
          with its limits.
--------------------------------------------------------------------------------
Input:  pVar
          Pointer to the "PROCESS_VAL" or "PROCESS_MATRIX" variable
        Index
          Index from the position that should be classified. For "PROCESS_VAL"
            types, set this to one.
        ArrLength
          Number of elements of the Matrix variable. For "PROCESS_VAL" types,
            set this to one.
Return:                       LimLL     LimL      LimH      LimHH
                                ^         ^         ^         ^
        TooHighValue  +         +         +         +         +#########+
        HighValue     +         +         +         +##########         +
        RegularValue  +         +         ###########         +         +
        LowValue      +         ##########+         +         +         +
        TooLowValue   +#########+         +         +         +         +

WARNING: Make sure that the arguments are correct and that the target's type
          is of FLOAT. If not properly used, this routine may behave like a
          wild pointer and cause side-effects.
----------------------------------------------------------------------------- */
VarStatus_t floatCheckLimits(float *pVar, uint16 Index, uint16 ArrLength)
{
  struct ProcVal_s
  {
    float Value;
    float *LimLL;
    float *LimL;
    float *LimH;
    float *LimHH;
  } Data;
  VarStatus_t Ret;

  /* First, copy the data value, according to the given index.                */
  memcpy(&Data.Value, (pVar + Index), sizeof(float));

  /* First, do some sanity checks. ArrLength must not be zero and Index must  */
  /*  be lower than ArrLength. Notice that only the latter needs to be        */
  /*  checked, as the first condition will also cause the second to be true.  */
  /* Also, check if the Value is a valid number.                              */
  if( ( Index >= ArrLength ) || ( isfinite( Data.Value ) == FALSE ) )
  {
    Ret = InvalidValue;
  }
  else
  {
    /* Now, copy the arguments that will be used for the comparison.          */
    memcpy(&Data.LimLL, (pVar + ArrLength), (4 * sizeof(float *)));

    if( Data.Value > (*Data.LimH) )
    {
      Ret = (Data.Value > (*Data.LimHH) ) ? TooHighValue : HighValue;
    }
    else if( Data.Value < (*Data.LimL) )
    {
      Ret = (Data.Value < (*Data.LimLL) ) ? TooLowValue : LowValue;
    }
    else
    {
      Ret = RegularValue;
    }
  }
  return Ret;
}




/* -----------------------------------------------------------------------------
LinearAdjust()
        Performs a linear adjust at the sent value based on the angular and
        linear coefficients
--------------------------------------------------------------------------------
Input:  IptValue
          Float variable which stores the value that will the linearly adjusted.
        AngCoeff
          Integer value that says the quantity of samples used to performs the
          average calculation.
        LinCoeff
Output: NA
Return: The linear adjust of "Iptvalue" by the angular and linear coefficients
----------------------------------------------------------------------------- */
float LinearAdjust(float IptValue, float AngCoeff, float LinCoeff)
  {
  float ValueCal;

  ValueCal = ( (IptValue * AngCoeff) + LinCoeff );

  /* If the calculation result is not finite number returns 0                 */
  if(isfinite(ValueCal) == FALSE)
    {
    ValueCal = 0;
    }

  return ValueCal;
  }




/* -----------------------------------------------------------------------------
fIsEqual() - Float equality comparison
        Evaluates the two provided float number to check if they are equal.
        Actually, they'll be checked to see if their difference is lower than a
        tolerance value
--------------------------------------------------------------------------------
Input:  N1
          First float number
        N2
          Second float number

Return: Boolean - TRUE if they are considered equal, FALSE if note

Remark #1: If any of the numbers are not normal, the comparison returns FALSE
Remark #2: For custom tolerance values, use the 'fIsEqualArb' routine, which
            receives the desired tolerance as argument.
----------------------------------------------------------------------------- */
#define IS_EQUAL_TOLERANCE                                               0.0001F
bool fIsEqual(float N1, float N2)
{
  bool Result;
  float Difference;

  /* First, check if both numbers are normal                                  */
  if(( isfinite(N1) == FALSE ) || ( isfinite(N2) == FALSE ))
  {
    Result = FALSE;
  }
  else
  {
    /* Get the difference                                                     */
    if( isgreaterequal(N1, N2) != FALSE ) { Difference = N1 - N2; }
    else                                  { Difference = N2 - N1; }
    /* Perform the comparison with the tolerance                              */
    if( isgreater( Difference, IS_EQUAL_TOLERANCE ) ) { Result = FALSE; }
    else                                              { Result = TRUE;  }
  }

  return Result;
}




/* -----------------------------------------------------------------------------
fIsEqualArb() - Float equality comparison, arbitrary tolerance
        Evaluates the two provided float number to check if they are equal.
        Similar to the fIsEqual routine, but the tolerance is variable.
--------------------------------------------------------------------------------
Input:  N1
          First float number
        N2
          Second float number
        Tol
          Tolerance value to be checked
Return: Boolean - TRUE if they are considered equal, FALSE if note

Remark #1: If any of the numbers are not normal, the comparison returns FALSE
Remark #2: For default tolerance values, use the 'fIsEqual' routine.
----------------------------------------------------------------------------- */
bool fIsEqualArb(float N1, float N2, float Tol)
{
  bool Result;
  float Difference;

  /* First, check if all numbers are normal                                   */
  if(( isfinite(N1) == FALSE ) || ( isfinite(N2) == FALSE ) || ( isfinite(Tol) == FALSE ))
  {
    Result = FALSE;
  }
  else
  {
    /* Get the difference                                                     */
    if( isgreaterequal(N1, N2) != FALSE ) { Difference = N1 - N2; }
    else                                  { Difference = N2 - N1; }
    /* Perform the comparison with the tolerance                              */
    if( isgreater( Difference, Tol ) )    { Result = FALSE; }
    else                                  { Result = TRUE;  }
  }

  return Result;
}

/* -----------------------------------------------------------------------------
IntegerPow() - Integer Power
        This routine calculates the integer power for the given integer number.

--------------------------------------------------------------------------------
Input:  base
          Number to have its power calculated
        exponent
          Exponent to use in the power operation
Return: uint32 - calculation result. Warning : No checks are performed to check
            if value has overflowed or not.
----------------------------------------------------------------------------- */
uint32 IntegerPow( uint32 base, uint32 exponent )
{
  uint32 Result = 1;

  while( exponent != 0 )
  {
    exponent--;
    Result *= base;
  }

  return Result;
}

/* -----------------------------------------------------------------------------
IntegerSqrt() - Integer Square Root
        This routine calculates the integer square root for the given 32bit
        number. It is based on Microchip's App Note TB040 - Fast Integer Square
        Root.
--------------------------------------------------------------------------------
Input:  number
          Number to have its square root calculated
Return: uint16 - calculation result
----------------------------------------------------------------------------- */
uint16 IntegerSqrt(uint32 number)
{
  uint16 root = 0;
  uint16 bit;
  uint16 trial;
  uint32 trial_pow;

  for(bit = 0x8000; bit > 0; bit >>=1)
  {
    trial = root + bit;
    trial_pow = (uint32)(trial * trial);
    if( trial_pow < number )  { root += bit;  }
  }
  return root;
}

/* -----------------------------------------------------------------------------
FloatToFixedPoint16() - Float to fixed point representation, base 10, Int16
        This routine converts a float value into its fixed point representation
          on a signed integer. For example, 3.14 can be represented as 314 if
          converted with two decimal places.
--------------------------------------------------------------------------------
Input:  Number
          Number to have its value converted
        DecDigits
          How many digits will represent the float's decimal places
Output: Result
          If convertion is successful, it'll hold the operation's result.
Return: Convertion status
          ANSWERED_REQUEST : Convertion is successful, data is available.
          Else : Some error happened, data may not be available or is saturated.
--------------------------------------------------------------------------------
Note: signed 16 bit ranges from -32768 to +32767
----------------------------------------------------------------------------- */
ReturnCode_t FloatToFixedPoint16( float Number, uint8 DecDigits, int16 * Result )
{
  ReturnCode_t ReturnValue;

  if( Result == NULL )                  { ReturnValue = ERR_PARAM_RANGE; }
  else if( DecDigits >= 6 )             { ReturnValue = ERR_PARAM_RANGE; }
  else if( isfinite(Number) == FALSE )  { ReturnValue = ERR_PARAM_RANGE; }
  else
  {
    const uint32 Multiplier = IntegerPow( 10, DecDigits );
          int32  IntNumber;

    /* Shift the input number to the right according to the required          */
    /*  decimal places.                                                       */
    Number *= Multiplier;

    /* Cast it to integer.                                                    */
    IntNumber  = (int32) roundf( Number );

    /* Evaluate if number is inside limits and saturate if needed.            */
    ReturnValue = ERR_MATH;

    if(      IntNumber < SHRT_MIN ) { IntNumber   = SHRT_MIN; }
    else if( IntNumber > SHRT_MAX ) { IntNumber   = SHRT_MAX; }
    else                            { ReturnValue = ANSWERED_REQUEST; }

    *Result = (int16) IntNumber;
  }

  return ReturnValue;
}

/* -----------------------------------------------------------------------------
FloatToFixedPointU16() - Float to fixed point representation, base 10, Uint16
        This routine converts a float value into its fixed point representation
          on an unsigned integer. For example, 3.14 can be represented as 314 if
          converted with two decimal places.
--------------------------------------------------------------------------------
Input:  Number
          Number to have its value converted
        DecDigits
          How many digits will represent the float's decimal places
Output: Result
          If convertion is successful, it'll hold the operation's result.
Return: Convertion status
          ANSWERED_REQUEST : Convertion is successful, data is available.
          Else : Some error happened, data may not be available or is saturated.
--------------------------------------------------------------------------------
Note: unsigned 16 bit ranges from 0 to 65535
----------------------------------------------------------------------------- */
ReturnCode_t FloatToFixedPointU16( float Number, uint8 DecDigits, uint16 * Result )
{
  ReturnCode_t ReturnValue;

  if( Result == NULL )                  { ReturnValue = ERR_PARAM_RANGE; }
  else if( DecDigits >= 6 )             { ReturnValue = ERR_PARAM_RANGE; }
  else if( isfinite(Number) == FALSE )  { ReturnValue = ERR_PARAM_RANGE; }
  else if( IS_FLOAT_NEGATIVE(Number) )  { ReturnValue = ERR_MATH;        }
  else
  {
    const uint32 Multiplier = IntegerPow( 10, DecDigits );
          uint32 IntNumber;

    /* Shift the input number to the right according to the required          */
    /*  decimal places.                                                       */
    Number *= Multiplier;

    /* Cast it to integer.                                                    */
    IntNumber   = (uint32) roundf( Number );

    /* Evaluate if number is inside limits and saturate if needed.            */
    ReturnValue = ERR_MATH;

    if( IntNumber > USHRT_MAX ) { IntNumber   = USHRT_MAX;        }
    else                        { ReturnValue = ANSWERED_REQUEST; }

    *Result = (uint16) IntNumber;
  }

  return ReturnValue;
}


/* -----------------------------------------------------------------------------
Coord_GetDistance() - Distance Calculation of two points on earth.
        This routine will calculate the distance between two Lat/Long points
          and will return its value through an argument.
        It uses the ‘haversine’ formula to calculate the great-circle distance
          between two points – that is, the shortest distance over the earth’s
          surface – giving an ‘as-the-crow-flies’ distance between the points.
        a = sin²(Δφ/2) + cos φ1 * cos φ2 * sin²(Δλ/2)
        c = 2 * atan2( √a, √(1−a) )
        d = R * c
        φ is latitude, λ is longitude, R is earth’s radius
        Reference:
        http://www.movable-type.co.uk/scripts/latlong.html
--------------------------------------------------------------------------------
Input:  P1_Lat, P1_Long: Coordinates for the 1st point, decimal degrees.
Input:  P2_Lat, P2_Long: Coordinates for the 2nd point, decimal degrees.

Output: DistKm: Estimate distance, in km, between the points.

Return: Result - ANSWERED_REQUEST : Operation Successful
                 Else             : Error
----------------------------------------------------------------------------- */
ReturnCode_t Coord_GetDistance( float P1_Lat, float P1_Long, float P2_Lat, float P2_Long, float * DistKm )
{
  const float   DegToRad    = fl_pi / 180.0;
  const float   EarthRadius = 6371.0; /* km */
        float   sinLat, sinLong;
        float   a, c, Result;
        ReturnCode_t  ReturnValue;

  /* First, convert all the coordinates to radians.                           */
  P1_Lat  *= DegToRad;
  P1_Long *= DegToRad;
  P2_Lat  *= DegToRad;
  P2_Long *= DegToRad;

  /* Calculate the sin-squared values. They are half the lat and long difs.   */
  sinLat  = sinf( (P2_Lat  - P1_Lat)  / 2 );
  sinLat  *= sinLat;
  sinLong = sinf( (P2_Long - P1_Long) / 2 );
  sinLong *= sinLong;

  /* Perform the haversine calculation and multiply by the earth's radius.    */
  a = sinLat + cosf(P1_Lat) * cosf(P2_Lat) * sinLong;
  c = 2 * atan2f( sqrt(a), sqrt(1 - a) );
  Result = EarthRadius * c;

  /* If all went well, answer success and copy the result. Otherwise, flag err*/
  if( isfinite(Result) )
  {
    *DistKm     = Result;
    ReturnValue = ANSWERED_REQUEST;
  }
  else
  {
    ReturnValue = ERR_MATH;
  }

  return ReturnValue;
}




/* -----------------------------------------------------------------------------
Time_TimeToBCD() - Time Calculation, Absolute time to BCD time
        This routine calculates the BCD value from a given time structure.
--------------------------------------------------------------------------------
Input:  Time structure containing the time to be converted.

Output: BCD time structure that will receive the result.

Return: Result - ANSWERED_REQUEST : Operation Successful
                 Else             : Error
----------------------------------------------------------------------------- */
ReturnCode_t Time_TimeToBCD( Time_t * Input, TimeBDC_t * Output )
{
  ReturnCode_t  ReturnValue;
  TimeBDC_t     WorkOutput;
  uint8         ErrCnt = 0;

  /* Perform the convertion of each field and increment the error counter if  */
  /*  it fails.                                                               */
  if( ConvertByteToBCD( Input->Secs,  &WorkOutput.Secs  ) != ANSWERED_REQUEST ) { ErrCnt++; }
  if( ConvertByteToBCD( Input->Mins,  &WorkOutput.Mins  ) != ANSWERED_REQUEST ) { ErrCnt++; }
  if( ConvertByteToBCD( Input->Hours, &WorkOutput.Hours ) != ANSWERED_REQUEST ) { ErrCnt++; }
  /* Guarantee a cleared padding.                                             */
  WorkOutput.Padding = 0x00;
  /* Check result.                                                            */
  if( ErrCnt == 0 )
  { /* All ok!                                                                */
    memcpy( Output, &WorkOutput, sizeof(TimeBDC_t) );
    ReturnValue = ANSWERED_REQUEST;
  }
  else
  { /* Some problem happened.                                                 */
    ReturnValue = ERR_MATH;
  }

  return ReturnValue;
}




/* -----------------------------------------------------------------------------
Time_TimeFromBCD() - Time Calculation, BCD time to Absolute time
        This routine calculates the Time value from a given BCD structure.
--------------------------------------------------------------------------------
Input:  BCD Time structure containing the time to be converted.

Output: Time structure that will receive the result.

Return: Result - ANSWERED_REQUEST : Operation Successful
                 Else             : Error
----------------------------------------------------------------------------- */
ReturnCode_t Time_TimeFromBCD( TimeBDC_t * Input, Time_t * Output )
{
  ReturnCode_t  ReturnValue;
  Time_t        WorkOutput;
  uint8         ErrCnt = 0;

  /* Perform the convertion of each field and increment the error counter if  */
  /*  it fails.                                                               */
  if( ConvertBCDToByte( Input->Secs,  &WorkOutput.Secs  ) != ANSWERED_REQUEST ) { ErrCnt++; }
  if( ConvertBCDToByte( Input->Mins,  &WorkOutput.Mins  ) != ANSWERED_REQUEST ) { ErrCnt++; }
  if( ConvertBCDToByte( Input->Hours, &WorkOutput.Hours ) != ANSWERED_REQUEST ) { ErrCnt++; }

  /* Check result.                                                            */
  if( ErrCnt == 0 )
  { /* All ok!                                                                */
    memcpy( Output, &WorkOutput, sizeof(Time_t) );
    ReturnValue = ANSWERED_REQUEST;
  }
  else
  { /* Some problem happened.                                                 */
    ReturnValue = ERR_MATH;
  }

  return ReturnValue;
}




/* -----------------------------------------------------------------------------
Time_DateToBCD() - Date Calculation, Absolute Date to BCD Date
        This routine calculates the BCD value from a given Date structure.
--------------------------------------------------------------------------------
Input:  Date structure containing the Date to be converted.

Output: BCD Date structure that will receive the result.

Return: Result - ANSWERED_REQUEST : Operation Successful
                 Else             : Error
----------------------------------------------------------------------------- */
ReturnCode_t Time_DateToBCD( Date_t * Input, DateBDC_t * Output )
{
  ReturnCode_t  ReturnValue;
  DateBDC_t     WorkOutput;
  uint8         ErrCnt = 0;

  /* Perform the convertion of each field and increment the error counter if  */
  /*  it fails.                                                               */
  if( ConvertByteToBCD( Input->Days,   &WorkOutput.Days   ) != ANSWERED_REQUEST ) { ErrCnt++; }
  if( ConvertByteToBCD( Input->Months, &WorkOutput.Months ) != ANSWERED_REQUEST ) { ErrCnt++; }
  if( ConvertWordToBCD( Input->Years,  &WorkOutput.Years  ) != ANSWERED_REQUEST ) { ErrCnt++; }
  /* Check result.                                                            */
  if( ErrCnt == 0 )
  { /* All ok!                                                                */
    memcpy( Output, &WorkOutput, sizeof(DateBDC_t) );
    ReturnValue = ANSWERED_REQUEST;
  }
  else
  { /* Some problem happened.                                                 */
    ReturnValue = ERR_MATH;
  }

  return ReturnValue;
}




/* -----------------------------------------------------------------------------
Time_DateFromBCD() - Date Calculation, BCD Date to Absolute Date
        This routine calculates the Date value from a given BCD structure.
--------------------------------------------------------------------------------
Input:  BCD Date structure containing the Date to be converted.

Output: Date structure that will receive the result.

Return: Result - ANSWERED_REQUEST : Operation Successful
                 Else             : Error
----------------------------------------------------------------------------- */
ReturnCode_t Time_DateFromBCD( DateBDC_t * Input, Date_t * Output )
{
  ReturnCode_t  ReturnValue;
  Date_t        WorkOutput;
  uint8         ErrCnt = 0;

  /* Perform the convertion of each field and increment the error counter if  */
  /*  it fails.                                                               */
  if( ConvertBCDToByte( Input->Days,   &WorkOutput.Days   ) != ANSWERED_REQUEST ) { ErrCnt++; }
  if( ConvertBCDToByte( Input->Months, &WorkOutput.Months ) != ANSWERED_REQUEST ) { ErrCnt++; }
  if( ConvertBCDToWord( Input->Years,  &WorkOutput.Years  ) != ANSWERED_REQUEST ) { ErrCnt++; }
  /* Check result.                                                            */
  if( ErrCnt == 0 )
  { /* All ok!                                                                */
    memcpy( Output, &WorkOutput, sizeof(Date_t) );
    ReturnValue = ANSWERED_REQUEST;
  }
  else
  { /* Some problem happened.                                                 */
    ReturnValue = ERR_MATH;
  }

  return ReturnValue;
}




/* -----------------------------------------------------------------------------
Time_ToTimeStamp() - Timestamp Calculation, DateTime to Timestamp
        This routine calculates Timestamp value of a DateTime structure
--------------------------------------------------------------------------------
Input:  DateTime structure containing the Date to be converted.

Output: Timestamp that will receive the result.

Return: Result - ANSWERED_REQUEST : Operation Successful
                 Else             : Error
----------------------------------------------------------------------------- */
ReturnCode_t Time_ToTimeStamp( DateTime_t * Input, TimeStamp_t * Output )
{
  /* Use the time.h mktime function to generate the timestamp.                */
  struct tm    tminfo;
  time_t       Result;
  ReturnCode_t ReturnValue;

  /* Assemble the tminfo with the DateTime contents                           */
  tminfo.tm_sec   = Input->TimeVal.Secs;
  tminfo.tm_min   = Input->TimeVal.Mins;
  tminfo.tm_hour  = Input->TimeVal.Hours;

  tminfo.tm_mday  = Input->DateVal.Days;
  /* tm's month representation goes from 0 to 11. So, subtract one from it.   */
  tminfo.tm_mon   = Input->DateVal.Months - 1;
  tminfo.tm_year  = Input->DateVal.Years - 1900;
  /* tm's year representation starts on 1900 and the localtime starts the     */
  /*  time representation from 1970. Since we starts from 2000, we need       */
  /*  to subtract 1900 + 30 = 1930 years                                      */
  /* Note: There is the possibility that the given year register is not       */
  /*  starting from 2000 but from 00. If that is the case, sum 2000 to it.    */
//  if( Input->DateVal.Years < 2000 ) { tminfo.tm_year  = Input->DateVal.Years + 70;   }
//  else                              { tminfo.tm_year  = Input->DateVal.Years - 1930; }

  Result = mktime( &tminfo );

  if( Result != -1 )
  { /* Convertion was ok.                                                     */
    *Output = (TimeStamp_t)Result;
    ReturnValue = ANSWERED_REQUEST;
  }
  else
  { /* Some problem happened with the mktime.                                 */
    ReturnValue = ERR_MATH;
  }

  return ReturnValue;
}




/* -----------------------------------------------------------------------------
Time_FromTimeStamp() - Timestamp Calculation, Timestamp to DateTime
        This routine calculates the DateTime structure value from a
          Timestamp value.
--------------------------------------------------------------------------------
Input:  Timestamp that will be converted.

Output: DateTime structure that will receive the results..

Return: Result - ANSWERED_REQUEST : Operation Successful
                 Else             : Error
----------------------------------------------------------------------------- */
ReturnCode_t Time_FromTimeStamp( TimeStamp_t * Input, DateTime_t * Output )
{
  /* Use the time.h localtime function to generate the timestamp.             */
  struct tm *tminfo;

  tminfo = localtime( (const time_t *) Input );

  /* Put the calculated values in our output structure.                       */
  Output->TimeVal.Secs   = (uint8)  tminfo->tm_sec;
  Output->TimeVal.Mins   = (uint8)  tminfo->tm_min;
  Output->TimeVal.Hours  = (uint8)  tminfo->tm_hour;

  Output->DateVal.Days   = (uint8)  tminfo->tm_mday;
  /* tm's month representation goes from 0 to 11. So, add one to it.          */
  Output->DateVal.Months = (uint8)  tminfo->tm_mon + 1;
  /* tm's year representation starts on 1900 and the localtime starts the     */
  /*  time representation from 1970. Since we want to start from 2000, we need*/
  /*  to add 1900 + 30 = 1930 years                                           */
  Output->DateVal.Years  = (uint16) tminfo->tm_year + 1930;

  /* Job done.                                                                */
  return ANSWERED_REQUEST;
}




/* -----------------------------------------------------------------------------
ConvertByteToBCD() - BCD Calculation, Byte to BCD
        This routine calculates the BCD value from a given byte.
        The BCD is of two digits only (0x00 - 0x99)
--------------------------------------------------------------------------------
Input:  Byte to be converted

Output: Convertion result

Return: Result - ANSWERED_REQUEST : Operation Successful
                 Else             : Error
----------------------------------------------------------------------------- */
ReturnCode_t ConvertByteToBCD( uint8 ByteValue, uint8 * BCD )
{
  ReturnCode_t ReturnValue;
  /* Check if given number is inside the acceptable range.                    */
  if( ByteValue <= 99 )
  { /* Perform the convertion and leave.                                      */
    *BCD = ((ByteValue / 10) * 16 + (ByteValue % 10));
    ReturnValue = ANSWERED_REQUEST;
  }
  else
  { /* Number is outside range and doesn't fit in the BCD.                    */
    ReturnValue = ERR_PARAM_RANGE;
  }
  return ReturnValue;
}




/* -----------------------------------------------------------------------------
ConvertWordToBCD() - BCD Calculation, Word to BCD
        This routine calculates the BCD value from a given Word.
        The BCD is of four digits only (0x0000 - 0x9999)
--------------------------------------------------------------------------------
Input:  Word to be converted

Output: Convertion result

Return: Result - ANSWERED_REQUEST : Operation Successful
                 Else             : Error
----------------------------------------------------------------------------- */
ReturnCode_t ConvertWordToBCD( uint16 WordValue, uint16 * BCD )
{
  ReturnCode_t ReturnValue;
  uint8        InValue_High;
  uint8        InValue_Low;
  UINT16xUINT8 WorkBCD;

  /* Check if given number is inside the acceptable range.                    */
  if( WordValue <= 9999 )
  { /* Separate the High and Low values by dividing the number by 100.        */
    InValue_High = (uint8)(WordValue / 100);
    InValue_Low  = (uint8)(WordValue % 100);

    if( ( ConvertByteToBCD( InValue_High, &WorkBCD.asUint8.Hi ) == ANSWERED_REQUEST ) &&
        ( ConvertByteToBCD( InValue_Low , &WorkBCD.asUint8.Lo ) == ANSWERED_REQUEST ) )
    { /* All ok. Copy the operation result.                                   */
      *BCD = WorkBCD.asUint16;
      ReturnValue = ANSWERED_REQUEST;
    }
    else
    { /* Some problem happened. Abort.                                        */
      ReturnValue = ERR_PARAM_RANGE;
    }
  }
  else
  { /* Number is outside range and doesn't fit in the BCD.                    */
    ReturnValue = ERR_PARAM_RANGE;
  }
  return ReturnValue;
}




/* -----------------------------------------------------------------------------
ConvertByteToBCD() - BCD Calculation, BCD to Byte
        This routine calculates the byte value from a given BCD.
        The BCD is of two digits only (0x00 - 0x99)
--------------------------------------------------------------------------------
Input:  Byte to be converted

Output: Convertion result

Return: Result - ANSWERED_REQUEST : Operation Successful
                 Else             : Error
----------------------------------------------------------------------------- */
ReturnCode_t ConvertBCDToByte( uint8 BCD, uint8 * ByteValue )
{
  ReturnCode_t ReturnValue;
  uint8        Result;

  /* Perform the convertion and check if result is valid.                     */
  Result = ((((BCD & 0xF0) >> 4) * 10) + (BCD & 0x0F));

  if( Result <= 99 )
  { /* All ok!                                                                */
    *ByteValue = Result;
    ReturnValue = ANSWERED_REQUEST;
  }
  else
  { /* Input value was not a BCD!                                             */
    ReturnValue = ERR_PARAM_RANGE;
  }
  return ReturnValue;
}




/* -----------------------------------------------------------------------------
ConvertBCDToWord() - BCD Calculation, BCD to Word
        This routine calculates the Word value from a given BCD.
        The BCD is of four digits only (0x0000 - 0x9999)
--------------------------------------------------------------------------------
Input:  Word to be converted

Output: Convertion result

Return: Result - ANSWERED_REQUEST : Operation Successful
                 Else             : Error
----------------------------------------------------------------------------- */
ReturnCode_t ConvertBCDToWord( uint16 BCD, uint16 * WordValue )
{
  ReturnCode_t ReturnValue;
  UINT16xUINT8 WorkBCD;

  /* Store the input value in the workBCD variable, but each byte already     */
  /*  converted from BCD.                                                     */
  if( ( ConvertBCDToByte( (uint8)( ( BCD & 0xFF00 ) >> 8 ), &WorkBCD.asUint8.Hi ) == ANSWERED_REQUEST ) &&
      ( ConvertBCDToByte( (uint8)( ( BCD & 0x00FF )      ), &WorkBCD.asUint8.Lo ) == ANSWERED_REQUEST ) )
  { /* All ok. Finish operation by multiplying the high byte by 100.          */
    *WordValue = (uint16)(WorkBCD.asUint8.Hi * 100) + WorkBCD.asUint8.Lo;
    ReturnValue = ANSWERED_REQUEST;
  }
  else
  { /* Some problem happened during the convertion.                           */
    ReturnValue = ERR_PARAM_RANGE;
  }
  return ReturnValue;
}




/* -----------------------------------------------------------------------------
BigFloat_Add() - Big Float calculation, increment
        This routine will sum a given Big Float number by a given float
          value. It'll adjust the Lower and Upper values as needed.
        Operation is: BigFloat = BigFloat + Increment
--------------------------------------------------------------------------------
Input / Output:  BigFloat
                  BigFloat that will receive the increment
Input : Value
                  Value which the BigFloat will be updated by. Can be either a
                    positive or a negative value.
Input : Threshold
                  Value that delimitates the related BigFloats' lower values.
Return: Result - ANSWERED_REQUEST : Operation Successful
                 Else             : Error
----------------------------------------------------------------------------- */
ReturnCode_t BigFloat_Add( BigFloat_t * BigFloat, float Value, float Threshold )
{
  ReturnCode_t ReturnValue;

  /* First, check if the floats are regular numbers. Otherwise, operation     */
  /*  cannot proceed.                                                         */
  if( ( isfinite(Value) != FALSE ) && ( isfinite(Threshold) != FALSE ) )
  {
    bool  isBigFloatNegative;

    float  WorkBase    = BigFloat->Lower;
    uint32 WorkMillion = BigFloat->Upper;
    float  AbsWorkBase;

    /* Operation will depend on if the BigFloat is a positive or negative     */
    /*  number. So, check it now before continuing.                           */
    if( IS_FLOAT_NEGATIVE(WorkBase) ) { isBigFloatNegative = TRUE;  }
    else                              { isBigFloatNegative = FALSE; }

    /* Now add the Value variable to the Lower component and get the resulting*/
    /*  abs value.                                                            */
    WorkBase += Value;

    /* If the Lower switched signal, then try to adjust the Upper component   */
    if( isBigFloatNegative == FALSE )
    { /* Number was positive...                                               */
      if( IS_FLOAT_NEGATIVE(WorkBase) )
      { /* ... and now it is negative                                         */
        /* Keep decrementing the Upper component 'till it depletes or signal*/
        /*  goes back to the correct value                                    */
        while( ( WorkMillion != 0 ) && ( WorkBase < 0.0 ) )
        {
          WorkBase += Threshold;
          --WorkMillion;
        }
      }
    }
    else
    { /* Number was negative...                                               */
      if( IS_FLOAT_NEGATIVE(WorkBase) == FALSE )
      { /* ... and now it is positive                                         */
        /* Keep decrementing the Upper component 'till it depletes or signal*/
        /*  goes back to the correct value                                    */
        while( ( WorkMillion != 0 ) && ( WorkBase >= 0.0 ) )
        {
          WorkBase -= Threshold;
          --WorkMillion;
        }
      }
    }

    /* If the operation above resulted on an invalid float, flag error.       */
    if( isfinite( WorkBase ) == FALSE ) { ReturnValue = ERR_MATH; }
    else
    {
      /* Get the resulting absolute value                                     */
      AbsWorkBase = fabsf(WorkBase);

      /* If the Lower component surpasses the limit, then the Upper component */
      /*  has to be adjusted.                                                 */
      if( AbsWorkBase >= Threshold )
      {
        uint32 AddToUpper = (uint32)( AbsWorkBase / Threshold );

        WorkMillion += AddToUpper;
        AbsWorkBase -= (float)( AddToUpper * Threshold );
      }

      /* The absolute Lower value may have been updated. Put it on the        */
      /*  original variable, taking care of the signal.                       */
      if( WorkBase >= 0.0 ) { WorkBase = AbsWorkBase;       }
      else                  { WorkBase = AbsWorkBase * -1;  }

      /* Update the original values.                                          */
      BigFloat->Lower = WorkBase;
      BigFloat->Upper = WorkMillion;

      /* Flag success                                                         */
      ReturnValue = ANSWERED_REQUEST;
    }
  }
  else
  { /* Invalid value                                                          */
    ReturnValue = ERR_VALUE;
  }

  return ReturnValue;
}




/* -----------------------------------------------------------------------------
BigFloat_Sum() - Big Float calculation, addition
        This routine will sum two BigFloats, with the result being stored in a
          third BigFloat.
        Operation is: BF_Res = BF_N1 + BF_N2
--------------------------------------------------------------------------------
Input : BF_N1, BF_N2
                  BigFloats that will be summed. They won't be modified.
Input : Threshold
                  Value that delimitates the related BigFloats' lower values.
Output: BF_Res
                  BigFloat that will hold the result. It may be modified.
Return: Result - ANSWERED_REQUEST : Operation Successful
                 Else             : Error
----------------------------------------------------------------------------- */
ReturnCode_t BigFloat_Sum( BigFloat_t * BF_N1, BigFloat_t * BF_N2, BigFloat_t * BF_Res, float Threshold )
{
  bool isN1Neg, isN2Neg;
  float Value;

  /* Everything will be prepared to use the BigFloat_Add operation to execute */
  /*  most of the sum process.                                                */
  /* Operation will depend of if both BigFloats have the same signal.         */
  if( IS_FLOAT_NEGATIVE(BF_N1->Lower) )   { isN1Neg = TRUE;   }
  else                                    { isN1Neg = FALSE;  }

  if( IS_FLOAT_NEGATIVE(BF_N2->Lower) )   { isN2Neg = TRUE;   }
  else                                    { isN2Neg = FALSE;  }

  if( isN1Neg == isN2Neg )
  { /* If both have the same signals, simply sum both Upper units and run     */
    /*  the _Add operation for their Lower units.                             */
    BF_Res->Upper = BF_N1->Upper + BF_N2->Upper;
    BF_Res->Lower = BF_N1->Lower;
    Value = BF_N2->Lower;
  }
  else
  { /* Their signals are inverted. The Upper component will have to be        */
    /*  subtracted instead of summing.                                        */
    if( BF_N2->Upper > BF_N1->Upper )
    {
      BF_Res->Upper = BF_N2->Upper - BF_N1->Upper;
      BF_Res->Lower = BF_N2->Lower;
      Value = BF_N1->Lower;
    }
    else
    { /* N1 is greater than or equal to N2                                    */
      BF_Res->Upper = BF_N1->Upper - BF_N2->Upper;
      BF_Res->Lower = BF_N1->Lower;
      Value = BF_N2->Lower;
    }
  }

  return BigFloat_Add(BF_Res, Value, Threshold);
}




/* -----------------------------------------------------------------------------
BigFloat_Sub() - Big Float calculation, subtraction
        This routine will subtract two BigFloats, with the result being stored
          in a third BigFloat.
        Operation is: BF_Res = BF_N1 - BF_N2
--------------------------------------------------------------------------------
Input : BF_N1, BF_N2
                  BigFloats that will be subtracted. They won't be modified.
Input : Threshold
                  Value that delimitates the related BigFloats' lower values.
Output: BF_Res
                  BigFloat that will hold the result. It may be modified.
Return: Result - ANSWERED_REQUEST : Operation Successful
                 Else             : Error
----------------------------------------------------------------------------- */
ReturnCode_t BigFloat_Sub( BigFloat_t * BF_N1, BigFloat_t * BF_N2, BigFloat_t * BF_Res, float Threshold )
{
  /* The subtraction operation can be interpreted as a sum operation with the */
  /*  second BigFloat having its signal inverted. So, simply invert its       */
  /*  signal and call the BigFloat Sum routine.                               */
  BigFloat_t BF_InvN2;

  BF_InvN2.Upper = BF_N2->Upper;
  BF_InvN2.Lower = BF_N2->Lower * -1.0;

  return BigFloat_Sum( BF_N1, &BF_InvN2, BF_Res, Threshold );
}




/* -----------------------------------------------------------------------------
BigFloat_Add() - Big Float calculation, linear adjust
        This routine will perform a linear adjustment on a BigFloat, with the
          result being stored in the same variable
        Operation is: BigFloat = (BigFloat * AngCoeff) + LinCoeff
--------------------------------------------------------------------------------
Input / Output:  BigFloat
                  BigFloat that will receive the adjust
Input : Threshold
                  Value that delimitates the related BigFloats' lower values.
Input : AngCoef
                  Angular coefficient. If it is null, it is ignored.
        LinCoef
                  Linear coefficient. If it is null, it is ignored.
Return: Result - ANSWERED_REQUEST : Operation Successful
                 Else             : Error
----------------------------------------------------------------------------- */
ReturnCode_t BigFloat_LinearAdjust( BigFloat_t * BigFloat, float AngCoef, float LinCoef, float Threshold )
{
  ReturnCode_t ReturnValue;
  /* The routine will perform all required rough operations and will call the */
  /*  _Add routine to run the required adjustments.                           */
  /* First, check if values are valid                                         */
  if( ( isfinite(AngCoef) != FALSE ) && ( isfinite(LinCoef) != FALSE ) )
  {
    /* The angular operation is executed first, followed by the linear one.   */
    /* In order to guarantee numeric precision, the following actions will be */
    /*  taken:                                                                */
    /* - The angular coefficient will be separated into their decimal and     */
    /*  integer parts. These will multiply the Upper component.               */
    /* - The integer multiplication will result in a 64bit number. If the     */
    /*  result gets bigger than 32bit, it'll be capped in 32 and an error will*/
    /*  be answered.                                                          */
    /* - The decimal multiplication will result in a double number. The result*/
    /*  will be splitted, the integer part will be added to the Upper         */
    /*  component and the decimal part will be added to the Lower.            */
    /* - The Lower will be directly multiplied by the coefficient. The op will*/
    /*  be splitted into four lower ones so that more precision is attainable.*/
    uint64 MillionResult_Int;
    double MillionResult_Dec;
    uint64 Float_IntPart = 0;
    double Float_DecPart = 0.0;
    uint32 Coef_IntPart = 0;
    float  Coef_DecPart = 0.0;
    uint32 Lowr_IntPart = 0;
    float  Lowr_DecPart = 0.0;
    bool   MultResultIsNeg;

    /* First, determine if the multiplication result will be negative or      */
    /*  positive. Then treat both numbers as positives.                       */
    if( IS_FLOAT_NEGATIVE(BigFloat->Lower) )
    { /* Bigfloat is negative. So, the result might be negative.              */
      BigFloat->Lower *= -1;
      MultResultIsNeg = TRUE;
    }
    else
    { /* Bigfloat is positive.                                                */
      MultResultIsNeg = FALSE;
    }

    if( IS_FLOAT_NEGATIVE(AngCoef) )
    { /* The angular coefficient is negative. So, toggle the previous result. */
      AngCoef *= -1;
      if( MultResultIsNeg != FALSE ) { MultResultIsNeg = FALSE; }
      else                           { MultResultIsNeg = TRUE;  }
    }
    else
    { /* The coefficient is positive, so keep the previous result.            */
    }

    /* Split the angular coefficient                                          */
    GetfIntDec( AngCoef, &Coef_IntPart, &Coef_DecPart );

    /* Perform the integer and float multiplication                           */
    MillionResult_Int = BigFloat->Upper * Coef_IntPart;
    MillionResult_Dec = BigFloat->Upper * (double)Coef_DecPart;

    /* Split the decimal result from the float multiplication                 */
    GetdIntDec( MillionResult_Dec, &Float_IntPart, &Float_DecPart );

    /* Add the integer part to the integer multiplication result              */
    MillionResult_Int += Float_IntPart;

    /* If the integer block resulted in a value higher than the uint32 limit, */
    /*  stop right here.                                                      */
    if( MillionResult_Int > 0xFFFFFFFF )
    { /* Flag error and cap result to the max                                 */
      ReturnValue = ERR_RANGE;
      BigFloat->Upper = 0xFFFFFFFF;
      BigFloat->Lower = 999999.99;
      /* Put the sign according to what was decided before.                   */
      if( MultResultIsNeg != FALSE ) { BigFloat->Lower *= -1; }
    }
    else
    { /* Proceed with regular operation                                       */
      ReturnValue = ANSWERED_REQUEST;               /* Operation will go well */
      /* Save the Upper component                                             */
      BigFloat->Upper = (uint32) MillionResult_Int;
      /* Start operating in the lower coefficient. The operation is a basic   */
      /*  multiplication but in order to improve the precision the operands   */
      /*  will be treated having their integer and decimal parts separated.   */
      /* Split the lower value                                                */
      GetfIntDec( BigFloat->Lower, &Lowr_IntPart, &Lowr_DecPart );
      /* There will be four multiplications. Between them the threshold will  */
      /*  be considered and the upper value updated.                          */
      /* First process both integer parts.                                    */
      BigFloat->Lower = (float)(Lowr_IntPart * Coef_IntPart);
      BigFloat->Upper += (uint32)(BigFloat->Lower / Threshold);
      BigFloat->Lower = fmodf( BigFloat->Lower, Threshold );
      /* Now process the lower decimal and the coef integer                   */
      BigFloat->Lower += (float)(Lowr_DecPart * Coef_IntPart);
      BigFloat->Upper += (uint32)(BigFloat->Lower / Threshold);
      BigFloat->Lower = fmodf( BigFloat->Lower, Threshold );
      /* Now process the lower integer and the coef decimal                   */
      BigFloat->Lower += (float)(Lowr_IntPart * Coef_DecPart);
      BigFloat->Upper += (uint32)(BigFloat->Lower / Threshold);
      BigFloat->Lower = fmodf( BigFloat->Lower, Threshold );
      /* Finally, process the lower decimal and the coef decimal              */
      BigFloat->Lower += (float)(Lowr_DecPart * Coef_DecPart);
      BigFloat->Upper += (uint32)(BigFloat->Lower / Threshold);
      BigFloat->Lower = fmodf( BigFloat->Lower, Threshold );
      /* Add the Upper decimal result to the Lower                            */
      BigFloat->Lower += (Float_DecPart * Threshold);
      /* Put the sign according to what was decided before.                   */
      if( MultResultIsNeg != FALSE ) { BigFloat->Lower *= -1; }
      /* To finish, call the Add operation, providing the linear coefficient  */
      BigFloat_Add(BigFloat, LinCoef, Threshold);
    }
  }
  else
  { /* Invalid values                                                         */
    ReturnValue = ERR_VALUE;
  }

  return ReturnValue;
}




/* -----------------------------------------------------------------------------
BigFloat_Scale()
        This routine will perform the multiplication of a BigFloat value, by a
        float value returning the result at a third variable.
        Operation is: ResultBF = InputBF * Factor
--------------------------------------------------------------------------------
Input:
        InputBF
          BigFloat value that will be multiplied
        Factor
          Float value that will multiply the BigFloat value
        Threshold
          Float value that delimitates the related BigFloats' lower values.

Output: ResultBF

Return:
        ReturnValueResult
          ANSWERED_REQUEST - Operation Successful
          ERR_VALUE -
----------------------------------------------------------------------------- */
ReturnCode_t BigFloat_Scale( BigFloat_t * ResultBF, BigFloat_t * InputBF, float Factor, float Threshold )
{
  ReturnCode_t ReturnValue;

  *ResultBF = *InputBF;

  ReturnValue = BigFloat_LinearAdjust(ResultBF, Factor, 0.0F, Threshold );

  return ReturnValue;
}




/* -----------------------------------------------------------------------------
BigFloat_AreEqual() - Big Float calculation, comparison
        This routine will compare the given BigFloats for equality.
        The operation representation is: BF_N1 == BF_N2 ? TRUE : FALSE
--------------------------------------------------------------------------------
Input:
        BF_N1, BF_N2: Pointers to both BigFloats
Output:
        AreEqual: Comparison Result: TRUE = Are equal. FALSE = Are different.
Return: Result - ANSWERED_REQUEST : Operation Successful
                 Else             : Error
----------------------------------------------------------------------------- */
ReturnCode_t BigFloat_AreEqual( BigFloat_t * BF_N1, BigFloat_t * BF_N2, bool * AreEqual )
{
  *AreEqual = ( ( BF_N1->Upper == BF_N2->Upper           ) &&
                ( fIsEqual( BF_N1->Lower, BF_N2->Lower ) ) );
  return ANSWERED_REQUEST;
}




/* -----------------------------------------------------------------------------
BigFloat_IsPositive()
        The function checks if the value is positive, in other words if it is
        BIGGER than zero, so the value zero is NOT considered positive.
--------------------------------------------------------------------------------
Input:
        BF
          Pointer to both BigFloats variable that will be evaluated
Output:
        IsPositive
          TRUE  - Is positive
          FALSE - Is NOT positive, i.e. a negative value or zero
Return: Result - ANSWERED_REQUEST : Operation Successful
                 Else             : Error
----------------------------------------------------------------------------- */
ReturnCode_t BigFloat_IsPositive( BigFloat_t * BF, bool * IsPositive )
  {
  if( IS_FLOAT_NEGATIVE(BF->Lower) )
    {
    /* If the float part has a negative signal, so the only valid possibility */
    /* is that the value is not positive                                      */
    *IsPositive = FALSE;
    }
  else if(BF->Lower > 0.0F)
    {
    /* If the float part has positive signal and is bigger than zero, so the  */
    /* only valid possibility is that the value is positive                   */
    *IsPositive = TRUE;
    }
  else if(BF->Upper > 0x00000000)
    {
    /* If the float part is zero and the integer part is bigger than zero, so */
    /* the only valid possibility is that the value is positive               */
    *IsPositive = TRUE;
    }
  else
    {
    /* If both parts are zero, so the value is zero hence not positive        */
    *IsPositive = FALSE;
    }
  return ANSWERED_REQUEST;
  }




/* -----------------------------------------------------------------------------
BigFloat_Clear() - Big Float calculation, clear BigFloat
        This routine will clear the given BigFloat in the argument.
--------------------------------------------------------------------------------
Input / Output:  BigFloat
                  BigFloat that will be cleared
Return: Result - ANSWERED_REQUEST : Operation Successful
                 Else             : Error
----------------------------------------------------------------------------- */
ReturnCode_t BigFloat_Clear( BigFloat_t * BigFloat )
{
  BigFloat->Lower = 0.0;
  BigFloat->Upper = 0;
  return ANSWERED_REQUEST;
}




/* -----------------------------------------------------------------------------
BigFloat_ToFloat()
          Converts a BigFloat value to a single float. Warning this operation
          can impacts the precision of the number, so be sure of what you are
          doing!
--------------------------------------------------------------------------------
Input:  BigFloat
          Pointer to a BigFloat variable that will be used to generate a float
          value
        Threshold
          Value used as limit to the decimal part of the BigFloat value

Output: FloatVal
          Pointer to a float value which will receive the converted value

Return:
        ANSWERED_REQUEST : Operation Successful

----------------------------------------------------------------------------- */
ReturnCode_t BigFloat_ToFloat( BigFloat_t * BigFloat, float Threshold, float * FloatVal )
{
  float DecAbs;
  float IntAbs;
  float SignedVal;

  IntAbs = (float)(BigFloat->Upper);
  DecAbs = fabsf(BigFloat->Lower);
  SignedVal = ( IS_FLOAT_NEGATIVE(BigFloat->Lower) ) ? -1.0F : 1.0F;

  *FloatVal = (IntAbs * Threshold + DecAbs) * SignedVal;
  return ANSWERED_REQUEST;
}




/* -----------------------------------------------------------------------------
BigFloat_TwoTotComput()
          Performs the calculation of a resultant totalization based on two
          totalizers, where each one of them have its totalization increased by
          fluid flow in one direction
--------------------------------------------------------------------------------
Input:  TotAB
          Totalization of the flow which occurred in one direction, which for
          identification purpose, it was called as ''A to B''
        TotBA
          Totalization of the flow which occurred in one direction, which for
          identification purpose, it was called as ''B to A''
        TotMode
          Manner as the final totalization will be computed, can be set as:
          . TotAB
          . TotBA
          . TotAB - TotBA
          . TotBA - TotAB
        Threshold
          Module of the value used to the BigFloat variables

Output: TotResult
          Pointer to a float value which will receive the computed value

Return: ANSWERED_REQUEST
          Operation was performed successfully
        ERR_PARAM_MODE
          The selected mode is invalid
        ERR_PARAM_ADDRESS
          The address of variable which will get the totalization result is
          invalid
----------------------------------------------------------------------------- */
ReturnCode_t BigFloat_TwoTotComput( BigFloat_t TotAB, BigFloat_t TotBA, MetersTotMode_t TotMode, BigFloat_t * TotResult, float Threshold )
  {
  ReturnCode_t FuncReturn = ANSWERED_REQUEST;

  if(TotResult == NULL)
    {
    /* The pointer to the variable which will receive the totalization result */
    /* was not initialized                                                    */
    FuncReturn = ERR_PARAM_ADDRESS;
    }
  else
    {
    switch(TotMode)
      {
      case ModeTotOnlyAB:
        {
        /* The totalization value is obtained considering only the            */
        /* totalization from AB direction                                     */
        (*TotResult) = TotAB;
        }
        break;

      case ModeTotOnlyBA:
        {
        /* The totalization value is obtained considering only the            */
        /* totalization from BA direction                                     */
        (*TotResult) = TotBA;
        }
        break;

      case ModeTotABminusBA:
        {
        /* The totalization value is obtained considering the totalization    */
        /* from AB direction minus the totalization from BA direction         */
        (void)BigFloat_Sub( &TotAB,
                            &TotBA,
                            TotResult,
                            Threshold );
        }
        break;

      case ModeTotBAminusAB:
        {
        /* The totalization value is obtained considering the totalization    */
        /* from BA direction minus the totalization from AB direction         */
        (void)BigFloat_Sub( &TotBA,
                            &TotAB,
                            TotResult,
                            Threshold );
        }
        break;

      default:
        {
        /* The set mode is not valid                                              */
        FuncReturn = ERR_PARAM_MODE;
        }
        break;
      }
    }

  return FuncReturn;
  }




/* -----------------------------------------------------------------------------
BigFloat_AvgFlowComput()
          Performs the calculation of a flow value according to the difference
            between two totalizations and a time period that has passed.
          The result is the average flow that should be needed so that the
            totalization at T1 becomes the value T2 after the time period.
--------------------------------------------------------------------------------
Input:  TotT1
          Initial totalization value. Should have the same unit as TotT2.
        TotT2
          Final totalization value. Should have the same unit as TotT1.
        Threshold
          Module of the value used to the BigFloat variables
        TimeT1T2
          Time that passed between the Tot T1 and Tot T2.

Output: Result
          Pointer to a float value which will receive the computed value.
          Have in mind that the resulting unit will depend of the input
            variable's units. For example, it the totalizations are in [L] and
            TimeT1T2 is in minutes, then the flow result will be in [L/min].

Return: ANSWERED_REQUEST
          Operation was performed successfully and result is available.
        Else
          Some problem happened and result may not be available.
----------------------------------------------------------------------------- */
ReturnCode_t BigFloat_AvgFlowComput( BigFloat_t * TotT1, BigFloat_t * TotT2, float Threshold, uint32 TimeT1T2, float * Result )
{
  ReturnCode_t ReturnValue;

  if(      TotT1  == NULL )               { ReturnValue = ERR_PARAM_RANGE; }
  else if( TotT2  == NULL )               { ReturnValue = ERR_PARAM_RANGE; }
  else if( Result == NULL )               { ReturnValue = ERR_PARAM_RANGE; }
  else if( isfinite(Threshold) == FALSE ) { ReturnValue = ERR_PARAM_RANGE; }
  else if( Threshold == 0 )               { ReturnValue = ERR_PARAM_RANGE; }
  else
  {
    BigFloat_t DeltaAsBF;

    /* Get the totalization difference.                                       */
    ReturnValue = BigFloat_Sub( TotT2, TotT1, &DeltaAsBF, Threshold );

    if( ReturnValue == ANSWERED_REQUEST )
    {
      float DeltaAsFloat;

      /* Convert the delta to float value.                                    */
      ReturnValue = BigFloat_ToFloat( &DeltaAsBF, Threshold, &DeltaAsFloat );

      if( ReturnValue == ANSWERED_REQUEST )
      {
        if( TimeT1T2 != 0 )
        {
          /* Finish operation by dividing the difference by the time period.  */
          *Result = DeltaAsFloat / TimeT1T2;
        }
        else
        {
          if( DeltaAsFloat == 0 ) { *Result = 0; }
          else
          {
            ReturnValue = ERR_MATH;

            if( IS_FLOAT_NEGATIVE( DeltaAsFloat ) ) { *Result = -HUGE_VAL; }
            else                                    { *Result = +HUGE_VAL; }
          }
        }
      }
    }
  }

  return ReturnValue;
}





/* -----------------------------------------------------------------------------
FCV_CNP6_70() - Volume Convertion Factor calculation, 1970's CNP n6 table.(Oil)
--------------------------------------------------------------------------------
Input / Output:  Pars - Parameters and internal variables used in calculation
        Output:  FCV  - Calculation result for the given pars.
Return: Result - ANSWERED_REQUEST : Operation Successful, FCV is available.
                 OPERATION_IDLE   : FCV is available, but it is the same as
                                      from the previous call.
                 Else             : Error
----------------------------------------------------------------------------- */
ReturnCode_t FCV_CNP6_70( float * FCV, FCV_CNP6_70_t * Pars )
{
  ReturnCode_t ReturnValue;

  bool         DensityHasChanged;
  bool         TemperatureHasChanged;

  /* Start by checking if the pointers were initialized.                      */
  if( ( Pars->pTempTOp == NULL ) || ( Pars->pDensTRef == NULL ) )
  {
    DO_ONLY_ONCE( __DEBUGHALT(); );
    ReturnValue = ERR_PARAM_RANGE;
  }
  else
  {
    /* First, check if temperature or density has changed. If it isn't, then  */
    /*  there is nothing to do.                                               */
    if( fIsEqual(Pars->LastDensTRef, *Pars->pDensTRef ) == FALSE ) { DensityHasChanged = TRUE;  }
    else                                                           { DensityHasChanged = FALSE; }

    if( fIsEqual(Pars->LastTempTOp, *Pars->pTempTOp )  == FALSE )  { TemperatureHasChanged = TRUE;  }
    else                                                           { TemperatureHasChanged = FALSE; }

    if( ( DensityHasChanged == FALSE ) && ( TemperatureHasChanged == FALSE ) )
    {
      /* There is nothing to do. Just keep the FCV with the previous value.   */
      *FCV = Pars->LastFCV;
      ReturnValue = OPERATION_IDLE;
    }
    else
    {
      float WorkDensity, WorkTemperature;

      /* First, work with density change                                      */
      if( DensityHasChanged == FALSE )
      {
        /* Simply load the density value from the memory                      */
        WorkDensity = Pars->LastDensTRef;
      }
      else
      {
        /* Perform sanity check on the parameters and update the memory one.  */
        uint16 IntDensity;

        /* First, NaN check                                                   */
        if( isfinite(*Pars->pDensTRef) != FALSE ) { WorkDensity = *Pars->pDensTRef;    }
        else                                      { WorkDensity =  Pars->LastDensTRef; }
        /* Now, check if value is inside the limits from the pars table       */
        /* If it isn't, then saturate it.                                     */
        if( isless( WorkDensity, 0.0) != FALSE )                             { WorkDensity = 0.0; }
        if( WorkDensity > TableCNP6_70_Pars[TABLE_CNP6_70_LASTITEM].Dthres ) { WorkDensity = TableCNP6_70_Pars[TABLE_CNP6_70_LASTITEM].Dthres; }
        Pars->LastDensTRef = WorkDensity;

        /* Consider the density value as an integer, to speed up every        */
        /*  comparison process.                                               */
        IntDensity = (uint16) WorkDensity;

        /* Check if the P Index has to be updated. It will have to if the     */
        /*  current value is outside the limits from the index in the memory  */
        /* The upper limit is always checked. Concerning the lower limit, it  */
        /*  is checked (by looking at the previous item's upper limit) if the */
        /*  current item is not the first item, as the first item doesn't     */
        /*  have previous item.                                               */
        if( ( IntDensity > TableCNP6_70_Pars[Pars->LastCNP_Index].Dthres ) ||
            ( ( Pars->LastCNP_Index > 0 ) && ( IntDensity <= TableCNP6_70_Pars[Pars->LastCNP_Index - 1].Dthres ) ) )
        {
          /* Run through every item in the Pars table 'till finding the range */
          uint8 CurrItem;

          for( CurrItem = 0; CurrItem < TABLE_CNP6_70_COUNT; ++CurrItem )
          {
            if( IntDensity <= TableCNP6_70_Pars[CurrItem].Dthres )
            {
              /* Found item.                                                  */
              Pars->LastCNP_Index = CurrItem;
              break;
            }
          }
        }
      }

      /* Now, work with temperature change                                    */
      if( TemperatureHasChanged == FALSE )
      {
        /* Simply load the temperature value from the memory                  */
        WorkTemperature = Pars->LastTempTOp;
      }
      else
      {
        /* Perform sanity check on the parameters and update the memory one.  */
        if( isfinite(*Pars->pTempTOp) != FALSE ) { WorkTemperature = *Pars->pTempTOp;   }
        else                                     { WorkTemperature = Pars->LastTempTOp; }
        Pars->LastTempTOp = WorkTemperature;
      }

      /* Finally, perform the FCV calculation                                 */
      {
        float  FcvResult, TemperaturePow2;
        uint32 Index = Pars->LastCNP_Index;

        /* FCV =  (1 + P2*(Temp - 20) + P4*(Temp - 20)^2 +                    */
        /*                 ( (P1*(Temp - 20) + P3*(Temp - 20)^2 ) / (dens) ) )*/
        /* Subtract 20 degrees from the temperature value and multiply to     */
        /*  calculate its power.                                              */
        WorkTemperature -= 20.0;
        TemperaturePow2 = WorkTemperature * WorkTemperature;

        /* Note concerning the Density:                                       */
        /* The density variable in the equation is divided by the water       */
        /*  density @ 4ºC, which has a value of 1000.0 kg/m3                  */
        /* In order to optimize the process, the P1 and P3 values (which are  */
        /*  divided by this ref density) are multiplied by 1000.0.            */
        FcvResult = ( ( ( WorkTemperature * TableCNP6_70_Pars[Index].P1 ) + ( TemperaturePow2 * TableCNP6_70_Pars[Index].P3 ) ) / WorkDensity );
        FcvResult += ( 1.0 + ( WorkTemperature * TableCNP6_70_Pars[Index].P2 ) + ( TemperaturePow2 * TableCNP6_70_Pars[Index].P4 ) );

        /* Output the result calculation and update the memory variable.      */
        Pars->LastFCV = FcvResult;
        *FCV = FcvResult;
      }

      ReturnValue = ANSWERED_REQUEST;
    }
  }

  return ReturnValue;
}


/* -----------------------------------------------------------------------------
FCV_NBR5992() - Volume Convertion Factor calculation, NBR5992 table. (Alcohol)
--------------------------------------------------------------------------------
Input / Output:  Pars - Parameters and internal variables used in calculation
        Output:  FCV  - Calculation result for the given pars.
Return: Result - ANSWERED_REQUEST : Operation Successful, FCV is available.
                 OPERATION_IDLE   : FCV is available, but it is the same as
                                      from the previous call.
                 Else             : Error
----------------------------------------------------------------------------- */
ReturnCode_t FCV_NBR5992( float * FCV, FCV_NBR5992_t * Pars )
{
  ReturnCode_t ReturnValue;

  /* Start by checking if the pointers were initialized.                      */
  if( ( Pars->pTempTOp == NULL ) || ( Pars->pDensTOp == NULL ) || ( Pars->pDensTRef == NULL ) )
  {
    DO_ONLY_ONCE( __DEBUGHALT(); );
    ReturnValue = ERR_PARAM_RANGE;
  }
  else
  {
    /* Check if the FCV has to be recalculated. It'll have to be if any       */
    /*  parameter have changed.                                               */
    if( ( Pars->LastTempTOp  == *Pars->pTempTOp  ) &&
        ( Pars->LastDensTOp  == *Pars->pDensTOp  ) &&
        ( Pars->LastDensTRef == *Pars->pDensTRef ) )
    {
      /* There is nothing to do, the FCV has the previous value.              */
      *FCV = Pars->LastFCV;
      ReturnValue = OPERATION_IDLE;
    }
    else
    {
      /* Recalculate FCV                                                      */
      float NewFCV;
      float WorkTemperature;
      float WorkDensityOp;
      float WorkDensityRef;

      /* Perform sanity check on the parameters and update the memory ones.   */
      if( isfinite(*Pars->pTempTOp) != FALSE )  { WorkTemperature = *Pars->pTempTOp;    }
      else                                      { WorkTemperature =  Pars->LastTempTOp; }
      Pars->LastTempTOp = WorkTemperature;

      if( isfinite(*Pars->pDensTOp) != FALSE )  { WorkDensityOp = *Pars->pDensTOp;      }
      else                                      { WorkDensityOp =  Pars->LastDensTOp;   }
      Pars->LastDensTOp = WorkDensityOp;

      if( isfinite(*Pars->pDensTRef) != FALSE ) { WorkDensityRef = *Pars->pDensTRef;    }
      else                                      { WorkDensityRef =  Pars->LastDensTRef; }
      Pars->LastDensTRef = WorkDensityRef;

      /* Proceed with the calculation. Main formula:                          */
      /*  Div = ( DensTOp / DensTRef );                                       */
      /*  Fcv = Div*( 1 + 0.000036 *( TempTOp - 20 ));                        */

      /* If the density variables are equal or lower than zero, stop          */
      /*  calculation and set FCV as zero.                                    */
      if( ( WorkDensityOp <= 0.0 ) || ( WorkDensityRef <= 0.0 ) )
      {
        NewFCV = 0.0;
      }
      else
      {
        NewFCV = ( 1 + ( 0.000036 * ( WorkTemperature - 20.0 ) ) );
        NewFCV *= WorkDensityOp / WorkDensityRef;
      }
      /* Calculation is complete. Update output variable and the memory var.  */
      Pars->LastFCV = NewFCV;
      *FCV = NewFCV;

      /* FCV updated                                                          */
      ReturnValue = ANSWERED_REQUEST;
    }
  }

  return ReturnValue;
}




/* -----------------------------------------------------------------------------
HydrocarbDensAt20()
        Performs the Density calculation to its correlated value at 20�C to
        hydrocarbon
--------------------------------------------------------------------------------
Input
        Param.pInpTempTOp
          Pointer to the variable which has the current temperature. Warning the
          temperature MUST be in [�C]
        Param.pInpDensTOp
          Pointer to the variable which has the current density. Warning the
          density MUST be in [kg/m�]
Return:
        ANSWERED_REQUEST
          Operation was performed successfully.
        OPERATION_IDLE
          A new calculation was not required since the input variables were kept
          since the previous execution
        ERR_PARAM_ADDRESS
          Some pointer of the structure was not initialized
        ERR_PARAM_VALUE
          Some of the input pointed variables was not finite
----------------------------------------------------------------------------- */
ReturnCode_t HydrocarbDensAt20( HydrocarbConv_t * Param )
  {
  ReturnCode_t ReturnValue;
  uint32 i;
  uint16 IntDens;
  float Dens;
  float TDev;
  float TDev2;
  float p1,p2,p3,p4;

  /* Start by checking if the pointers were initialized.                      */
  if( ( Param->pInpDensTOp  == NULL )
    ||( Param->pInpTempTOp  == NULL )
    ||( Param->pOutDensT20  == NULL ) )
    {
    DO_ONLY_ONCE( __DEBUGHALT(); );
    ReturnValue = ERR_PARAM_ADDRESS;
    }
  else if(( isfinite(*Param->pInpDensTOp) == FALSE )
    ||    ( isfinite(*Param->pInpTempTOp) == FALSE ))
    {
    /* Any of the input values is not finite, so that is no how to perform a  */
    /* new calculation                                                        */
    ReturnValue = ERR_PARAM_VALUE;
    }
  else if(( Param->PrevDensTOp  == *Param->pInpDensTOp )
    &&    ( Param->PrevTempTOp  == *Param->pInpTempTOp ))
    {
    /* There is nothing to do, the input variables were not changed           */
    *Param->pOutDensT20 = Param->PrevDensT20;
    ReturnValue = OPERATION_IDLE;
    }
  else
    {
    /* Prepare the values required to the processing for convenience          */
    Dens = (*Param->pInpDensTOp);
    IntDens = (uint16)Dens;
    TDev = (*Param->pInpTempTOp) - 20.0F;
    TDev2 = TDev * TDev;

    /* Look for the density value at reference table                          */
    for(i=0; i<=TABLE_CNP6_70_LASTITEM; i++)
      {
      if( IntDens <= TableCNP6_70_Pars[i].Dthres ) break;
      }

    /* Loads the coefficients for conversion                                  */
    p1 = TableCNP6_70_Pars[i].P1;
    p2 = TableCNP6_70_Pars[i].P2;
    p3 = TableCNP6_70_Pars[i].P3;
    p4 = TableCNP6_70_Pars[i].P4;

    /* Calculates the density at 20�C                                         */
    (*Param->pOutDensT20) = (Dens - p1*TDev - p3*TDev2)
                          / (1    + p2*TDev + p4*TDev2);

    /* Saves the previous parameters                                          */
    Param->PrevDensTOp = (*Param->pInpDensTOp);
    Param->PrevTempTOp = (*Param->pInpTempTOp);
    Param->PrevDensT20 = (*Param->pOutDensT20);

    /* All done                                                               */
    ReturnValue = ANSWERED_REQUEST;
    }


  return ReturnValue;
  }

