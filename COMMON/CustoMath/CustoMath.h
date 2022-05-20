/* *****************************************************************************
 FILE_NAME:     CustoMath.h
 DESCRIPTION:   Software library with customized math functions
 DESIGNER:      Juliano Varasquim
 CREATION_DATE: 16/nov/2015
 VERSION:       3.3
***************************************************************************** */
#ifndef CUSTOMATH_H_INCLUDED
#define CUSTOMATH_H_INCLUDED




/* *****************************************************************************
 *
 *        INCLUDES (and DEFINES for INCLUDES)
 *
***************************************************************************** */
#include "types.h"
#include "returncode.h"
#include "macros.h"
#include <math.h>



/* *****************************************************************************
 *
 *        FIRMWARE VERSION
 *
***************************************************************************** */
#define COMMON_CUSTOMATH_VER_MAJOR  3
#define COMMON_CUSTOMATH_VER_MINOR  3
#define COMMON_BRANCH_MASTER




/* *****************************************************************************
 *
 *        DEFINES, ENUMS, STRUCTURES
 *
 **************************************************************************** */


/* Configuration structure to be used with the Damping algorithm              */
#define LINKED_TO(BN)                   { 0, 0, (sizeof(BN)/4), 0, ANSWERED_REQUEST, (float *)(&BN)}
typedef struct DampStr_s
  {
  uint16 BufIndex;
  uint16 PrevBufUsedLength;
  const uint16 MaxBufLength;
  uint16 ValidSampInTheBuf;
  ReturnCode_t Return;
  float *Buf;
  } DampStr_t;


/* Configuration structure to be used with the Moving Damping algorithm       */
#define MOVING_DMP_CFG(WEIGHT)          { 0.0, 0.0, 0, WEIGHT, ANSWERED_REQUEST, FALSE }
typedef struct MovDampStr_s
  {
  /* Internal items */
  float        CurrAvg;      /* Will hold the current moving average value                                      */
  float        DmpWeight;    /* Memory used to speed up calculation when counter is full                        */
  uint16       CurrCnt;      /* Will hold how many items have been averaged so far. Goes up until the CntLimit. */
  /* External items */
  uint16       CntLimit;     /* Maximum items to consider for the average.                                      */
  ReturnCode_t Return;       /* Informs Damping condition                                                       */
  bool         ResetDamping; /* If True, damping routine restarts the averaging process                         */
  } MovDampStr_t;


/* Configuration structure to be used with the BigFloat implementation, along */
/*  with its initialization macro.                                            */
/* Representation: 'BigFloat' = Threshold * 'Upper' + 'Lower'                 */
/*  The Base component indicates if the number is positive or negative.       */
/*  The Threshold value is given as an argument by the caller.                */
typedef struct BigFloat_s
{
  float  Lower;
  uint32 Upper;
} BigFloat_t;
#define BIGFLOAT_INIT                                                 { 0.0, 0 }

/* Configuration structure to be used with the FCV CNP6_70 routine.           */
typedef struct
{
  /* Pointer to the input variables                                           */
  float * pTempTOp;
  float * pDensTRef;
  /* Internal variables - do not touch them.                                  */
  float   LastTempTOp;
  float   LastDensTRef;
  uint32  LastCNP_Index;
  float   LastFCV;
} FCV_CNP6_70_t;
#define FCV_CNP6_70_INIT                        { NULL, NULL, NAN, NAN, 0, NAN }

/* Configuration structure to be used with the FCV NBR5992 routine.           */
typedef struct
{
  /* Pointer to the input variables - set them before using the convertion.   */
  float * pTempTOp;
  float * pDensTOp;
  float * pDensTRef;
  /* Internal variables - do not touch them.                                  */
  float   LastTempTOp;
  float   LastDensTOp;
  float   LastDensTRef;
  float   LastFCV;
} FCV_NBR5992_t;
#define FCV_NBR5992_INIT                { NULL, NULL, NULL, NAN, NAN, NAN, NAN }

/* Configuration structure to compensate the density at 20�C.                 */
typedef struct HydrocarbConv_s
{
  /* Pointer to the input variables. They must be set before the using.       */
  float * pInpTempTOp;
  float * pInpDensTOp;
  /* Pointer to the output variables. They must be set before the using.      */
  float * pOutDensT20;
  /* Backup of the previous variable value. It is strongly advised do not     */
  /* touch them.                                                              */
  float PrevTempTOp;
  float PrevDensTOp;
  float PrevDensT20;
} HydrocarbConv_t;
/* It is strongly advised to use the macro below to initialize the structure  */
/* above                                                                      */
#define HYDROCARB_CONV_INIT(TempOp,DensOp,Dens20) {&TempOp,&DensOp,&Dens20,NAN,NAN,NAN}

/* Constants with generic use in mathematics as for example pi, Euler's       */
/* constant, typical root square, logarithms, etc. represented as double.     */
#define db_e            2.7182818284590452354       /* e     (neperian digit) */
#define db_log2e        1.4426950408889634074       /* log2(e)                */
#define db_log10e       0.43429448190325182765      /* log10(e)               */
#define db_ln2          0.693147180559945309417     /* ln(2)                  */
#define db_ln10         2.30258509299404568402      /* ln(10)                 */
#define db_pi           3.14159265358979323846      /* pi constant            */
#define db_sqrpi        1.77245385090551602792981   /* ��/�(pi)               */
#define db_sqr2         1.41421356237309504880      /* ��/�(2)                */
#define db_sqr3         1.73205080756887719000      /* ��/�(3)                */


/* Constants with generic use in mathematics as for example pi, Euler's       */
/* constant, typical root square, logarithms, etc. represented as float.      */
#define fl_e            ((float)db_e)               /* e     (neperian digit) */
#define fl_log2e        ((float)db_log2e)           /* log2(e)                */
#define fl_log10e       ((float)db_log10e)          /* log10(e)               */
#define fl_ln2          ((float)db_ln2)             /* ln(2)                  */
#define fl_ln10         ((float)db_ln10)            /* ln(10)                 */
#define fl_pi           ((float)db_pi)              /* pi constant            */
#define fl_sqrpi        ((float)db_sqrpi)           /* ��/�(pi)               */
#define fl_sqr2         ((float)db_sqr2)            /* ��/�(2)                */
#define fl_sqr3         ((float)db_sqr3)            /* ��/�(3)                */


/* Volume flow conversion table. Beware, those coefficients only make sense   */
/* if the converted data is received in L/s                                   */
#define VOLUME_FLOW_CONV_TABLE      { 1.0F,               /* L_s      */      \
                                      60.0F,              /* L_min    */      \
                                      3600.0F,            /* L_h      */      \
                                      0.001F,             /* m3_s     */      \
                                      0.06F,              /* m3_min   */      \
                                      3.6F,               /* m3_h     */      \
                                      1000.0F,            /* cm3_s    */      \
                                      60000.0F,           /* cm3_min  */      \
                                      3600000.0F,         /* cm3_h    */      \
                                      1000.0F,            /* mL_s     */      \
                                      60000.0F,           /* mL_min   */      \
                                      3600000.0F,         /* mL_h     */      \
                                      0.0353146625F,      /* ft3_s    */      \
                                      2.1188801972F,      /* ft3_min  */      \
                                      127.132779509F,     /* ft3_h    */      \
                                      61.0237589903F,     /* in3_s    */      \
                                      3661.4250925517F,   /* in3_min  */      \
                                      219685.537727586F,  /* in3_h    */      \
                                      0.2641720373F,      /* gal_s    */      \
                                      15.8503222371F,     /* gal_min  */      \
                                      951.0196357024F,    /* gal_h    */      \
                                      22824.4634425134F,  /* gal_d    */      \
                                      0.0013079505F,      /* yd3_s    */      \
                                      0.0784770431F,      /* yd3_min  */      \
                                      4.7086211086F,      /* yd3_h    */      \
                                      0.006289810411905F, /* bbl_s    */      \
                                      0.377388624692857F, /* bbl_m    */      \
                                      22.643324659581F,   /* bbl_h    */      \
                                      543.439605774128F,  /* bbl_d    */      }

typedef enum VolumeFlowUnit_e
  {
  /* 00 */  L_s,
  /* 01 */  L_min,
  /* 02 */  L_h,
  /* 03 */  m3_s,
  /* 04 */  m3_min,
  /* 05 */  m3_h,
  /* 06 */  cm3_s,
  /* 07 */  cm3_min,
  /* 08 */  cm3_h,
  /* 09 */  mL_s,
  /* 10 */  mL_min,
  /* 11 */  mL_h,
  /* 12 */  ft3_s,
  /* 13 */  ft3_min,
  /* 14 */  ft3_h,
  /* 15 */  in3_s,
  /* 16 */  in3_min,
  /* 17 */  in3_h,
  /* 18 */  gal_s,
  /* 19 */  gal_min,
  /* 20 */  gal_h,
  /* 21 */  gal_d,
  /* 22 */  yd3_s,
  /* 23 */  yd3_min,
  /* 24 */  yd3_h,
  /* 25 */  bbl_s,
  /* 26 */  bbl_min,
  /* 27 */  bbl_h,
  /* 28 */  bbl_d,
            volumeflow_len, // Do not move this item of that position
            volumeflow_size = 0xFFFF,
  } VolumeFlowUnit_t;
#define VOL_FLOW_UNIT_MIN                                (VolumeFlowUnit_t)(L_s)
#define VOL_FLOW_UNIT_MAX                   (VolumeFlowUnit_t)(volumeflow_len-1)


/* Volume totalization conversion table. Beware, those coefficients only make */
/* sense if the converted data is received in L                               */
#define VOLUME_CONV_TABLE       { 1.0F,               /* L              */    \
                                  100.0F,             /* cL             */    \
                                  1000.0F,            /* mL             */    \
                                  0.001F,             /* m3             */    \
                                  1.0F,               /* dm3            */    \
                                  1000.0F,            /* cm3            */    \
                                  1000000.0F,         /* mm3            */    \
                                  0.0353147F,         /* ft3            */    \
                                  61.023759F,         /* in3            */    \
                                  0.001308F,          /* yd3            */    \
                                  0.264172F,          /* gal            */    \
                                  0.2270207F,         /* gal_dry        */    \
                                  0.2199692F,         /* gal_imp        */    \
                                  2.1133763F,         /* pint           */    \
                                  1.8161659F,         /* pint_dry       */    \
                                  1.759754F,          /* pint_imp       */    \
                                  1.0566883F,         /* quart          */    \
                                  0.9080829F,         /* quart_dry      */    \
                                  0.879877F,          /* quart_imp      */    \
                                  0.0062898F,         /* bbl            */    \
                                  0.0283776F,         /* bushel         */    \
                                  33.8140222F,        /* fl_oz          */    \
                                  35.1950828F,        /* fl_oz_imp      */    \
                                  0.1135104F,         /* peck           */    \
                                  4.2267535F,         /* cup_US         */    \
                                  3.519508F,          /* cup_imp        */    }
typedef enum VolumeUnit_e
    {
    /* 00 */  L,
    /* 01 */  cL,
    /* 02 */  mL,
    /* 03 */  m3,
    /* 04 */  dm3,
    /* 05 */  cm3,
    /* 06 */  mm3,
    /* 07 */  ft3,
    /* 08 */  in3,
    /* 09 */  yd3,
    /* 10 */  gal,
    /* 11 */  gal_dry,
    /* 12 */  gal_imp,
    /* 13 */  pint,
    /* 14 */  pint_dry,
    /* 15 */  pint_imp,
    /* 16 */  quart,
    /* 17 */  quart_dry,
    /* 18 */  quart_imp,
    /* 19 */  bbl,
    /* 20 */  bushel,
    /* 21 */  fl_oz,
    /* 22 */  fl_oz_imp,
    /* 23 */  peck,
    /* 24 */  cup_US,
    /* 25 */  cup_imp,
              volume_size = 0xFFFF,
    } VolumeUnit_t;

/* Density conversion table. Beware, those coefficients only make sense       */
/* if the converted data is received in kg/m³                                 */
#define DENSITY_CONV_TABLE            { 1.0F,               /* kg_m3          */      \
                                        0.001F,             /* g_cm3          */      \
                                        1000.0F,            /* g_m3           */      \
                                        1000000.0F,         /* mg_m3          */      \
                                        0.001F,             /* kg_L           */      \
                                        1.0F,               /* g_L            */      \
                                        1000.0F,            /* mg_L           */      \
                                        0.00834541F,        /* lb_gal_US      */      \
                                        0.01002241F,        /* lb_gal_UK      */      \
                                        0.06242797F,        /* lb_ft3         */      \
                                        0.00003613F,        /* lb_in3         */      \
                                        1.685555F,          /* lb_yd3         */      \
                                        0.07768885F,        /* lb_bushel      */      \
                                        0.13352647F,        /* oz_gal_US      */      \
                                        0.16035861F,        /* oz_gal_UK      */      \
                                        0.00057804F,        /* oz_in3         */      \
                                        58.41783473F,       /* gr_gal_US      */      \
                                        0.00194032F,        /* slug_ft3       */      \
                                        0.00084278F,        /* ton_yd3_S      */      }
typedef enum DensityUnit_e
  {
  /* 00 */  kg_m3,
  /* 01 */  g_cm3,
  /* 02 */  g_m3,
  /* 03 */  mg_m3,
  /* 04 */  kg_L,
  /* 05 */  g_L,
  /* 06 */  mg_L,
  /* 07 */  lb_gal_US,
  /* 08 */  lb_gal_UK,
  /* 09 */  lb_ft3,
  /* 10 */  lb_in3,
  /* 11 */  lb_yd3,
  /* 12 */  lb_bushel,
  /* 13 */  oz_gal_US,
  /* 14 */  oz_gal_UK,
  /* 15 */  oz_in3,
  /* 16 */  gr_gal_US,
  /* 17 */  slug_ft3,
  /* 18 */  ton_yd3_S,
            density_size = 0xFFFF,
  } DensityUnit_t;

/* Concentration unii table. Beware, those coefficients only make sense       */
/* if the converted data is received in kg/m³                                */
/* Warning: Currently this conversion table is not entirely built.            */
#define CONCENT_CONV_TABLE             { 1.0F,              /* deg_BRIX       */      \
                                         1.0F,              /* dummy          */      \
                                         1.0F,              /* dummy          */      \
                                         1.0F,              /* dummy          */      \
                                         1.0F,              /* deg_INPM       */      }

typedef enum ConcentUnit_e
  {
  /* 00 */  deg_BRIX       = 0,
  /* 03 */  deg_GV         = 3,
  /* 04 */  deg_INPM       = 4,
            specmass_size = 0xFFFF,
  } ConcentUnit_t;
  
/* Mass Flow conversion table. Beware, those coefficients only make sense     */
/* if the converted data is received in kg/s                                  */
#define MASSFLOW_CONV_TABLE           { 1000.0F,            /* g_s            */      \
                                        60000.0F,           /* g_min          */      \
                                        3600000.0F,         /* g_hr           */      \
                                        86400000.0F,        /* g_d            */      \
                                        1000000.0F,         /* mg_s           */      \
                                        60000000.0F,        /* mg_min         */      \
                                        3600000000.0F,      /* mg_hr          */      \
                                        86400000000.0F,     /* mg_d           */      \
                                        1.0F,               /* kg_s           */      \
                                        60.0F,              /* kg_min         */      \
                                        3600.0F,            /* kg_hr          */      \
                                        86400.0F,           /* kg_d           */      \
                                        2.204622476038F,    /* lb_s           */      \
                                        132.277348562278F,  /* lb_min         */      \
                                        7936.640913736644F, /* lb_hr          */      \
                                        190479.38192968094F,/* lb_d           */      \
                                        0.001F,             /* ton_s          */      \
                                        0.06F,              /* ton_min        */      \
                                        3.6,                /* ton_hr         */      \
                                        86.4,               /* ton_d          */      }
typedef enum MassFlowUnit_e
  {
  /* 00 */  g_s,
  /* 01 */  g_min,
  /* 02 */  g_hr,
  /* 03 */  g_d,
  /* 04 */  mg_s,
  /* 05 */  mg_min,
  /* 06 */  mg_hr,
  /* 07 */  mg_d,
  /* 08 */  kg_s,
  /* 09 */  kg_min,
  /* 10 */  kg_hr,
  /* 11 */  kg_d,
  /* 12 */  lb_s,
  /* 13 */  lb_min,
  /* 14 */  lb_hr,
  /* 15 */  lb_d,
  /* 16 */  ton_s,
  /* 17 */  ton_min,
  /* 18 */  ton_hr,
  /* 19 */  ton_d,
            massflow_size = 0xFFFF,
  } MassFlowUnit_t;
  
/* Mass Totalization conversion table. Beware, those coefficients only make   */
/* sense if the converted data is received in kg                              */
#define MASSTOT_CONV_TABLE            { 1000.0F,            /* g              */      \
                                        1000000.0F,         /* mg             */      \
                                        1.0F,               /* kg             */      \
                                        2.2046225F,         /* lb             */      \
                                        0.001F,             /* ton            */      }
typedef enum MassTotUnit_e
  {
  /* 00 */  g,
  /* 01 */  mg,
  /* 02 */  kg,
  /* 03 */  lb,
  /* 04 */  ton,
            masstot_size = 0xFFFF,
  } MassTotUnit_t;
  
/* Frequency conversion table. Beware, those coefficients only make           */
/* sense if the converted data is received in Hz                              */
#define FREQUENCY_CONV_TABLE          {    1.0F,            /* Hz             */      \
                                          60.0F,            /* RPM            */      }
typedef enum FrequencyUnit_e
  {
  /* 00 */  Hz,
  /* 01 */  RPM,
            frequency_size = 0xFFFF,
  } FrequencyUnit_t;

/* Volumetric consumption conversion table. Beware, those coefficients only   */
/* make sense if the converted data is received in km/L                       */
#define VOLTOTCONS_CONV_TABLE   { 1.0F,           /* km_L                     */      \
                                  3.78541178F,    /* km_gal_us                */      \
                                  4.54609188F,    /* km_gal_uk                */      \
                                  2.352145833F,   /* mpg_us                   */      \
                                  2.824809363F,   /* mpg_uk                   */      \
                                  0.5396118F,     /* nmi_L                    */      \
                                  2.042652885F,   /* nmi_gal_us               */      \
                                  2.453123789F,   /* nmi_gal_uk               */      \
                                  -1.0F,          /* L_km         (see NOTE)  */      \
                                  -3.78541178F,   /* gal_us_km    (see NOTE)  */      \
                                  -4.54609188F,   /* gal_uk_km    (see NOTE)  */      \
                                  -0.5396118F,    /* L_nmi        (see NOTE)  */      \
                                  -2.042652885F,  /* gal_us_nmi   (see NOTE)  */      \
                                  -2.453123789F,  /* gal_uk_nmi   (see NOTE)  */      \
                                  -100.0F,        /* L_100km      (see NOTE)  */      \
                                }

/* NOTE:  If a negative value is used as conversion factor, the functions     */
/*        responsible to the unit changing use the absolute value of the      */
/*        factor to multiply the original value, after that, the function     */
/*        calculate the inverse of the multiplication result and return this  */
/*        value to the user.                                                  */
typedef enum VolTotConsUnit_e
  {
  /* 00 */  km_L,
  /* 01 */  km_gal_us,
  /* 02 */  km_gal_uk,
  /* 03 */  mpg_us,
  /* 04 */  mpg_uk,
  /* 05 */  nmi_L,
  /* 06 */  nmi_gal_us,
  /* 07 */  nmi_gal_uk,
  /* 08 */  L_km,
  /* 09 */  gal_us_km,
  /* 10 */  gal_uk_km,
  /* 11 */  L_nmi,
  /* 12 */  gal_us_nmi,
  /* 13 */  gal_uk_nmi,
  /* 14 */  L_100km,

            VolTotConsUnit_t_Items,   /* Warning! New units should be added   */
                                      /* before this item of the enumeration  */
            voltotcons_size = 0xFFFF,
  } VolTotConsUnit_t;
#define FUEL_USAGE_MIN_UNIT                                               (km_L)
#define FUEL_USAGE_MAX_UNIT                           (VolTotConsUnit_t_Items-1)




/* Temperature conversion linear table.                                       */
/* Temp. if the converted data is received in Celsius                         */
#define TEMP_CONV_LINEAR_TABLE        {    0.0F,          /* Celsus           */      \
                                          32.0F,          /* Fahrenheit       */      \
                                        491.67F,          /* Rankine          */      \
                                        273.15F,          /* Kelvin           */      }

/* Temperature conversion angle table.                                        */
/* Temp. if the converted data is received in Celsius                         */
#define TEMP_CONV_ANGLE_TABLE        {     1.0F,          /* Celsus           */      \
                                           1.8F,          /* Fahrenheit       */      \
                                           1.8F,          /* Rankine          */      \
                                           1.0F,          /* Kelvin           */      }
typedef enum TempUnit_e
  {
  /* 00 */  CELSIUS = 0,
  /* 01 */  FAHRENHEIT,
  /* 02 */  RANKINE,
  /* 03 */  KELVIN,
            temp_size = 0xFFFF,
  } TempUnit_t;

#define CELSUS_TO_FAHRENHEIT(C)    ((C*1.8)+32)
#define CELSUS_TO_RANKINE(C)       ((C*1.8)+491.67)
#define CELSUS_TO_KELVIN(C)        (C+273.15)

#define FAHRENHEIT_TO_CELSUS(F)     ((F-32)/1.8)
#define FAHRENHEIT_TO_RANKINE(F)    (F+459.67)
#define FAHRENHEIT_TO_KELVIN(F)     ((F+459.67)/1.8)

#define RANKINE_TO_CELSUS(R)        ((R-491.67)/1.8)
#define RANKINE_TO_FAHRENHEIT(R)    (R-459.67)
#define RANKINE_TO_KELVIN(R)        (R/1.8)

#define KELVIN_TO_CELSUS(K)         (K+273.15)
#define KELVIN_TO_FAHRENHEIT(K)     ((K*1.8)-459.889)
#define KELVIN_TO_RANKINE(K)        (K*1.8)

/* *****************************************************************************
 *
 *        PUBLIC PROTOTYPES
 *
***************************************************************************** */
ReturnCode_t GetfIntDec(float FloatVar, uint32 * IntPart, float * DecPart);
ReturnCode_t GetdIntDec(double DoubleVar, uint64 * IntPart, double * DecPart);
ReturnCode_t TruncfTo(float *Var, uint16 DecPlac);
float        Damping(float NewSample, uint16 BufUsedLen, DampStr_t *Param);
float        MovingDamping(float NewSample, MovDampStr_t *Param);
float        UnitConversion(float OrigValue, uint8 toUnit, const float *AngTab, const float *LinTab);
float        UnitChange(float ValueAtIniUnit, uint8 fromUnit, uint8 toUnit, const float *AngTab, const float *LinTab);
VarStatus_t  floatCheckLimits(float *pVar, uint16 Index, uint16 ArrLength);
float        LinearAdjust(float IptValue, float AngCoeff, float LinCoeff);
bool         fIsEqual(float N1, float N2);
bool         fIsEqualArb(float N1, float N2, float Tol);
uint32       IntegerPow( uint32 base, uint32 exponent );
uint16       IntegerSqrt(uint32 number);

ReturnCode_t FloatToFixedPoint16(  float Number, uint8 DecDigits, int16 *  Result );
ReturnCode_t FloatToFixedPointU16( float Number, uint8 DecDigits, uint16 * Result );

ReturnCode_t Coord_GetDistance( float P1_Lat, float P1_Long, float P2_Lat, float P2_Long, float * DistKm );

ReturnCode_t Time_TimeToBCD( Time_t * Input, TimeBDC_t * Output );
ReturnCode_t Time_TimeFromBCD( TimeBDC_t * Input, Time_t * Output );
ReturnCode_t Time_DateToBCD( Date_t * Input, DateBDC_t * Output );
ReturnCode_t Time_DateFromBCD( DateBDC_t * Input, Date_t * Output );
ReturnCode_t Time_ToTimeStamp( DateTime_t * Input, TimeStamp_t * Output );
ReturnCode_t Time_FromTimeStamp( TimeStamp_t * Input, DateTime_t * Output );

ReturnCode_t ConvertByteToBCD( uint8  ByteValue, uint8  * BCD );
ReturnCode_t ConvertWordToBCD( uint16 WordValue, uint16 * BCD );
ReturnCode_t ConvertBCDToByte( uint8  BCD, uint8  * ByteValue );
ReturnCode_t ConvertBCDToWord( uint16 BCD, uint16 * WordValue );

ReturnCode_t BigFloat_Add( BigFloat_t * BigFloat, float Increment, float Threshold );
ReturnCode_t BigFloat_Sum( BigFloat_t * BF_N1, BigFloat_t * BF_N2, BigFloat_t * BF_Res, float Threshold );
ReturnCode_t BigFloat_Sub( BigFloat_t * BF_N1, BigFloat_t * BF_N2, BigFloat_t * BF_Res, float Threshold );
ReturnCode_t BigFloat_LinearAdjust( BigFloat_t * BigFloat, float AngCoef, float LinCoef, float Threshold );
ReturnCode_t BigFloat_Scale( BigFloat_t * ResultBF, BigFloat_t * InputBF, float Factor, float Threshold );
ReturnCode_t BigFloat_AreEqual( BigFloat_t * BF_N1, BigFloat_t * BF_N2, bool * AreEqual );
ReturnCode_t BigFloat_IsPositive( BigFloat_t * BF, bool * IsPositive );
ReturnCode_t BigFloat_Clear( BigFloat_t * BigFloat );
ReturnCode_t BigFloat_ToFloat( BigFloat_t * BigFloat, float Threshold, float * FloatVal );
ReturnCode_t BigFloat_TwoTotComput( BigFloat_t TotAB, BigFloat_t TotBA, MetersTotMode_t TotMode, BigFloat_t * TotResult, float Threshold );
ReturnCode_t BigFloat_AvgFlowComput( BigFloat_t * TotT1, BigFloat_t * TotT2, float Threshold, uint32 TimeT1T2, float * Result );

ReturnCode_t FCV_CNP6_70( float * FCV, FCV_CNP6_70_t * Pars );
ReturnCode_t FCV_NBR5992( float * FCV, FCV_NBR5992_t * Pars );
ReturnCode_t HydrocarbDensAt20( HydrocarbConv_t * Param );

#endif /* CUSTOMATH_H_INCLUDED */
