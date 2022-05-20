/* *****************************************************************************
 FILE_NAME:     Macros.h
 DESCRIPTION:   This header file contains macros, designed to meet the needs of
                all projects.
 DESIGNER:      Andre F. N Dainese
                Daniel C. Rebeschini
                Juliano G. P. Varasquim
 CREATION_DATE: 04/08/2015
 *******************************************************************************
 VERSION 0.2:   25/08/2015
                - Introduction of new macros to create variable accessible to
                  the user of the system
 VERSION 0.3:   01/09/2015
                - Introduction of new macros to create variable in array format
                  accessible to the user of the system
 VERSION 0.4:   01/09/2015 - Andre F. N. Dainese
                - Added CONC2BYTE macro
                - Removed all the tab characters.
 VERSION 0.5:   01/09/2015 - Andre F. N. Dainese
                - Bugfixes in the saturate macros.
 VERSION 0.6:   18/04/2017 - Andre F. N. Dainese
                - ADD_MATRIX_VAR and ADD_MATRIX_BIGF now accepts matrices [][]
***************************************************************************** */
#ifndef MACROS_H
#define MACROS_H

#include <string.h>


/* *****************************************************************************
 *        FIRMWARE VERSION
***************************************************************************** */
#define COMMON_MACROS_VER_MAJOR   0
#define COMMON_MACROS_VER_MINOR   5
#define COMMON_BRANCH_MASTER

/* *****************************************************************************
 DESCRIPTION: Macros to create variables accessible to the system user
 MACROS:      CREATE_SINGLE_CONFIG_DATA
              EXTERN_SINGLE_CONFIG_DATA
              CREATE_SINGLE_PROCESS_VAL
              EXTERN_SINGLE_PROCESS_VAL
              CREATE_SINGLE_DIAGNOS_VAL
              EXTERN_SINGLE_DIAGNOS_VAL

 DESIGNER:    Andre F. N Dainese
              Daniel C. Rebeschini
 EXAMPLE:
              CREATE_SINGLE_CONFIG_DATA(UnitMass_t, MassUnit, _gr, _kg, _ton);
              CREATE_SINGLE_CONFIG_DATA(float, MassLimFL, 0.0, 1.0, 10.0);
              CREATE_SINGLE_CONFIG_DATA(float, MassLimAL, 0.0, 2.0, 10.0);
              CREATE_SINGLE_CONFIG_DATA(float, MassLimAH, 0.0, 8.0, 10.0);
              CREATE_SINGLE_CONFIG_DATA(float, MassLimFH, 0.0, 9.0, 10.0);
              CREATE_SINGLE_PROCESS_VAL(float, Mass, 0.0, MassLimFL, MassLimAL, MassLimAH, MassLimFH);

***************************************************************************** */

/* Definition of the structure used to create "Data Configuration" variables  */
#define CREATE_SINGLE_CONFIG_DATA(Type,Name,Mi,C,Ma) Name##_Type Name =       \
  {                                                                           \
    C,                                                                        \
    Mi,                                                                       \
    Ma,                                                                       \
    C,                                                                        \
  }
#define EXTERN_SINGLE_CONFIG_DATA(Type,Name) typedef struct Name##_s          \
  {                                                                           \
    Type Current;                                                             \
    Type Min;                                                                 \
    Type Max;                                                                 \
    Type Default;                                                             \
  } Name##_Type;                                                              \
  extern Name##_Type Name


/* Definition of the structure used to create variables of "Process Values"   */
#define CREATE_SINGLE_PROCESS_VAL(Type,Name,LL,L,C,H,HH) Name##_Type Name =   \
  {                                                                           \
    C,                                                                        \
    &LL.Current,                                                              \
    &L.Current,                                                               \
    &H.Current,                                                               \
    &HH.Current,                                                              \
  }
#define EXTERN_SINGLE_PROCESS_VAL(Type,Name) typedef struct Name##_s          \
  {                                                                           \
    Type Value;                                                               \
    Type *LowLow;                                                             \
    Type *Low;                                                                \
    Type *High;                                                               \
    Type *HighHigh;                                                           \
  } Name##_Type;                                                              \
  extern Name##_Type Name


/* Definition of the structure used to create "Diagnostics Values" variables  */
/* - Initialized declaration                                                  */
#define CREATE_SINGLE_DIAGNOS_VAL(Type,Name,C) Name##_Type Name =             \
  {                                                                           \
    C,                                                                        \
  }
/* - Unitialized declaration                                                  */
#define UNINIT_SINGLE_DIAGNOS_VAL(Type,Name)   Name##_Type Name
/* - External declaration                                                     */
#define EXTERN_SINGLE_DIAGNOS_VAL(Type,Name) typedef struct Name##_s          \
  {                                                                           \
    Type Value;                                                               \
  } Name##_Type;                                                              \
  extern Name##_Type Name


/* -- OBSOLETE MACROS. FOR COMPATIBILITY ONLY, SHOULD NOT BE USED ANYMORE! -- */
#define CREATE_CONFIG_DATA_STRUCT                      CREATE_SINGLE_CONFIG_DATA
#define EXTERN_CONFIG_DATA_STRUCT                      EXTERN_SINGLE_CONFIG_DATA
#define CREATE_PROCESS_VAL_STRUCT                      CREATE_SINGLE_PROCESS_VAL
#define EXTERN_PROCESS_VAL_STRUCT                      EXTERN_SINGLE_PROCESS_VAL
#define CREATE_DIAGNOS_VAL_STRUCT                      CREATE_SINGLE_DIAGNOS_VAL
#define EXTERN_DIAGNOS_VAL_STRUCT                      EXTERN_SINGLE_DIAGNOS_VAL
/* -------------------------------------------------------------------------- */

/* *****************************************************************************
 DESCRIPTION:   Macros to create variables arrays AND MATRICES accessible to
                  the system user
 MACROS:        CREATE_MATRIX_CONFIG_DATA
                EXTERN_MATRIX_CONFIG_DATA
                CREATE_MATRIX_PROCESS_VAL
                EXTERN_MATRIX_PROCESS_VAL
                CREATE_MATRIX_DIAGNOS_VAL
                UNINIT_MATRIX_DIAGNOS_VAL
                EXTERN_MATRIX_DIAGNOS_VAL

 DESIGNER:      Juliano Varasquim / Andre F. N. Dainese
 EXAMPLE:
                #define MDBS_DEVICE_AMOUNT  2
                #define MDBS_MIN_ADDR_LIST  1,    1
                #define MDBS_INIT_ADDR_LIST 1,    1
                #define MDBS_MAX_ADDR_LIST  247,  247
                ...
                CREATE_MATRIX_CONFIG_DATA(byte, MDBS_DeviceAddress, [MDBS_DEVICE_AMOUNT], MDBS_MIN_ADDR_LIST  , MDBS_INIT_ADDR_LIST , MDBS_MAX_ADDR_LIST);


                CREATE_SINGLE_CONFIG_DATA(  float                 , TEMP_ThermoLowLow   , TEMP_MIN_LL_THERMO  , TEMP_INI_LL_THERMO  , TEMP_MAX_LL_THERMO   );
                CREATE_SINGLE_CONFIG_DATA(  float                 , TEMP_ThermoLow      , TEMP_MIN_L_THERMO   , TEMP_INI_L_THERMO   , TEMP_MAX_L_THERMO    );
                CREATE_SINGLE_CONFIG_DATA(  float                 , TEMP_ThermoHigh     , TEMP_MIN_H_THERMO   , TEMP_INI_H_THERMO   , TEMP_MAX_H_THERMO    );
                CREATE_SINGLE_CONFIG_DATA(  float                 , TEMP_ThermoHighHigh , TEMP_MIN_HH_THERMO  , TEMP_INI_HH_THERMO  , TEMP_MAX_HH_THERMO   );
                ...
                CREATE_MATRIX_PROCESS_VAL(  float, TEMP_ThermoTemp, [TEMP_THERMO_AMOUNT], TEMP_ThermoLowLow   , TEMP_ThermoLow      , TEMP_INIT_LT_THERMO, TEMP_ThermoHigh,TEMP_ThermoHighHigh);


                typedef enum
                  {
                  STATE1,
                  STATE2,
                  STATE3
                  }MachineState;
                #define STATE_AMOUNT  2
                #define INIT_STATE_LIST STATE1, STATE1
                ...
                CREATE_MATRIX_DIAGNOS_VAL(MachineState, State,[STATE_AMOUNT],INIT_STATE_LIST);
************************************************************************************************************ */

/* Definition of the structure used to create matrix of "Data Configuration" */
#define CREATE_MATRIX_CONFIG_DATA(Type,Name,I,Mi,C,Ma) Name##_Type Name =     \
  {                                                                           \
    {C},                                                                      \
    {Mi},                                                                     \
    {Ma},                                                                     \
    {C},                                                                      \
  }
#define EXTERN_MATRIX_CONFIG_DATA(Type,Name,I) typedef struct Name##_s        \
  {                                                                           \
    Type Current I ;                                                          \
    Type Min I ;                                                              \
    Type Max I ;                                                              \
    Type Default I ;                                                          \
  } Name##_Type;                                                              \
  extern Name##_Type Name


/* Definition of the structure used to create matrix of "Process Values" */
#define CREATE_MATRIX_PROCESS_VAL(Type,Name,I,LL,L,C,H,HH) Name##_Type Name = \
  {                                                                           \
    {C},                                                                      \
    &LL.Current,                                                              \
    &L.Current,                                                               \
    &H.Current,                                                               \
    &HH.Current,                                                              \
  }
#define EXTERN_MATRIX_PROCESS_VAL(Type,Name,I) typedef struct Name##_s        \
  {                                                                           \
    Type Value I ;                                                            \
    Type *LowLow;                                                             \
    Type *Low;                                                                \
    Type *High;                                                               \
    Type *HighHigh;                                                           \
  } Name##_Type;                                                              \
  extern Name##_Type Name


/* Definition of the structure used to create matrix of "Diagnostics Values"  */
/* - Initialized declaration                                                  */
#define CREATE_MATRIX_DIAGNOS_VAL(Type,Name,I,C) Name##_Type Name =           \
  {                                                                           \
    {C},                                                                      \
  }
/* - Unitialized declaration                                                  */
#define UNINIT_MATRIX_DIAGNOS_VAL(Type,Name,I) Name##_Type Name
/* - External declaration                                                     */
#define EXTERN_MATRIX_DIAGNOS_VAL(Type,Name,I) typedef struct Name##_s        \
  {                                                                           \
    Type Value I ;                                                            \
  } Name##_Type;                                                              \
  extern Name##_Type Name



/* -- OBSOLETE MACROS. FOR COMPATIBILITY ONLY, SHOULD NOT BE USED ANYMORE! -- */
#define CREATE_CONFIG_DATA_MATRIX                      CREATE_MATRIX_CONFIG_DATA
#define EXTERN_CONFIG_DATA_MATRIX(T,N,I)               EXTERN_MATRIX_CONFIG_DATA(T,N,[I])
#define CREATE_PROCESS_VAL_MATRIX                      CREATE_MATRIX_PROCESS_VAL
#define EXTERN_PROCESS_VAL_MATRIX(T,N,I)               EXTERN_MATRIX_PROCESS_VAL(T,N,[I])
#define CREATE_DIAGNOS_VAL_MATRIX                      CREATE_MATRIX_DIAGNOS_VAL
#define EXTERN_DIAGNOS_VAL_MATRIX(T,N,I)               EXTERN_MATRIX_DIAGNOS_VAL(T,N,[I])
/* -------------------------------------------------------------------------- */

/* *****************************************************************************
 DESCRIPTION:   Macros to create pseudo variables arrays and matrices
                  accessible to the system user. These variables have their
                  limits and default values set to zero, as it is expected that
                  these fields will be handled by a library (such as the modbus
                  master when controlling an external variable).
                If the macros are fed with two arguments, then a 'single'
                  variable is expanded. If they are fed with three or four,
                  though, then a 'matrix' is expanded. So, the 'length'
                  arguments are optional.
                NOTE: These macros doesn't work with arrays with three
                  dimensions or more!

 MACROS:        CREATE_HOLLOW_CONFIG_DATA( Type, Name, [Length1], [Length2] )
                EXTERN_HOLLOW_CONFIG_DATA( Type, Name, [Length1], [Length2] )
                CREATE_HOLLOW_DIAGNOS_VAL( Type, Name, [Length1], [Length2] )
                EXTERN_HOLLOW_DIAGNOS_VAL( Type, Name, [Length1], [Length2] )

 DESIGNER:      Andre F. N. Dainese
 EXAMPLE:
                CREATE_HOLLOW_CONFIG_DATA(  float                 , TEMP_ThermoLowLow             );
                CREATE_HOLLOW_CONFIG_DATA(  float                 , TEMP_ThermoUnit     ,  2      );
                CREATE_HOLLOW_CONFIG_DATA(  float                 , TEMP_ThermoMatrix   ,  2 , 10 );

                CREATE_HOLLOW_DIAGNOS_VAL(  uint32                , DummyCount                    );
                CREATE_HOLLOW_DIAGNOS_VAL(  float                 , FlowValue           ,  3      );
************************************************************************************************************ */
/* Internal macros for intermediary build                                     */
#define GET_HOLLOW_MACRO(_1,_2,_3,_4,NAME,...) NAME
#define HOLLOW_ERROR  "Invalid Hollow Variable argument count"

#define CREATE_HOLLOW_CFG_SINGLE_0( TYPE, NAME       ) CREATE_SINGLE_CONFIG_DATA( TYPE, NAME,          0,                                                       0,                                                       0 )
#define CREATE_HOLLOW_CFG_MATRIX_1( TYPE, NAME, I    ) CREATE_MATRIX_CONFIG_DATA( TYPE, NAME, [I]    , REPEATS_N_TIMES_VALUE( I, 0 ),                           REPEATS_N_TIMES_VALUE( I, 0 ),                           REPEATS_N_TIMES_VALUE( I, 0 ) )
#define CREATE_HOLLOW_CFG_MATRIX_2( TYPE, NAME, I, J ) CREATE_MATRIX_CONFIG_DATA( TYPE, NAME, [I][J] , REPEATS_N_TIMES_VALUE( I, INITIALIZE_VARIABLE( J, 0 ) ), REPEATS_N_TIMES_VALUE( I, INITIALIZE_VARIABLE( J, 0 ) ), REPEATS_N_TIMES_VALUE( I, INITIALIZE_VARIABLE( J, 0 ) ) )
#define EXTERN_HOLLOW_CFG_SINGLE_0( TYPE, NAME       ) EXTERN_SINGLE_CONFIG_DATA( TYPE, NAME         )
#define EXTERN_HOLLOW_CFG_MATRIX_1( TYPE, NAME, I    ) EXTERN_MATRIX_CONFIG_DATA( TYPE, NAME, [I]    )
#define EXTERN_HOLLOW_CFG_MATRIX_2( TYPE, NAME, I, J ) EXTERN_MATRIX_CONFIG_DATA( TYPE, NAME, [I][J] )
#define CREATE_HOLLOW_VAL_SINGLE_0( TYPE, NAME       ) CREATE_SINGLE_DIAGNOS_VAL( TYPE, NAME,          0                                                       )
#define CREATE_HOLLOW_VAL_MATRIX_1( TYPE, NAME, I    ) CREATE_MATRIX_DIAGNOS_VAL( TYPE, NAME, [I]    , REPEATS_N_TIMES_VALUE( I, 0 )                           )
#define CREATE_HOLLOW_VAL_MATRIX_2( TYPE, NAME, I, J ) CREATE_MATRIX_DIAGNOS_VAL( TYPE, NAME, [I][J] , REPEATS_N_TIMES_VALUE( I, INITIALIZE_VARIABLE( J, 0 ) ) )
#define EXTERN_HOLLOW_VAL_SINGLE_0( TYPE, NAME       ) EXTERN_SINGLE_DIAGNOS_VAL( TYPE, NAME         )
#define EXTERN_HOLLOW_VAL_MATRIX_1( TYPE, NAME, I    ) EXTERN_MATRIX_DIAGNOS_VAL( TYPE, NAME, [I]    )
#define EXTERN_HOLLOW_VAL_MATRIX_2( TYPE, NAME, I, J ) EXTERN_MATRIX_DIAGNOS_VAL( TYPE, NAME, [I][J] )

/* Final macro declarations                                                   */
#define CREATE_HOLLOW_CONFIG_DATA(...) GET_HOLLOW_MACRO( __VA_ARGS__, CREATE_HOLLOW_CFG_MATRIX_2, CREATE_HOLLOW_CFG_MATRIX_1, CREATE_HOLLOW_CFG_SINGLE_0, HOLLOW_ERROR )(__VA_ARGS__)
#define EXTERN_HOLLOW_CONFIG_DATA(...) GET_HOLLOW_MACRO( __VA_ARGS__, EXTERN_HOLLOW_CFG_MATRIX_2, EXTERN_HOLLOW_CFG_MATRIX_1, EXTERN_HOLLOW_CFG_SINGLE_0, HOLLOW_ERROR )(__VA_ARGS__)

#define CREATE_HOLLOW_DIAGNOS_VAL(...) GET_HOLLOW_MACRO( __VA_ARGS__, CREATE_HOLLOW_VAL_MATRIX_2, CREATE_HOLLOW_VAL_MATRIX_1, CREATE_HOLLOW_VAL_SINGLE_0, HOLLOW_ERROR )(__VA_ARGS__)
#define EXTERN_HOLLOW_DIAGNOS_VAL(...) GET_HOLLOW_MACRO( __VA_ARGS__, EXTERN_HOLLOW_VAL_MATRIX_2, EXTERN_HOLLOW_VAL_MATRIX_1, EXTERN_HOLLOW_VAL_SINGLE_0, HOLLOW_ERROR )(__VA_ARGS__)

/* *****************************************************************************
 DESCRIPTION: Macros to check limits of the single variables. It can be applied
              both in single configuration data and in single process values.
 MACROS:
              IS_SINGLE_VAR
 DESIGNER:
              Juliano Varasquim
 CONFIG DATA
 OPTIONS:     HIGHER_THAN_MAX
              LOWER_THAN_MIN
              OUT_OF_RANGE
              INTO_OF_RANGE
 PROCESS VAL
 OPTIONS:     NORMAL
              ABNORMAL
              TOO_ABNORMAL
              TOO_HIGH
              HIGH
              LOW
              TOO_LOW
 EXAMPLE:
              if IS_SINGLE_VAR(Counter, HIGHER_THAN_MAX)  {...}
              if IS_SINGLE_VAR(Baudrate, OUT_OF_RANGE)    {...}
              if IS_SINGLE_VAR(Flow, ABNORMAL)            {...}
              if IS_SINGLE_VAR(Temperature, TOO_HIGH)     {...}
***************************************************************************** */

/* Only to config data single variables */
#define IS_SINGLE_VAR__HIGHER_THAN_MAX(CDS)                                   \
  ( ( CDS.Current > CDS.Max ) ? TRUE : FALSE )
#define IS_SINGLE_VAR__LOWER_THAN_MIN(CDS)                                    \
  ( ( CDS.Current < CDS.Min ) ? TRUE : FALSE )
#define IS_SINGLE_VAR__OUT_OF_RANGE(CDS)                                      \
  ( ( ( CDS.Current > CDS.Max ) || ( CDS.Current < CDS.Min ) ) ? TRUE : FALSE )
#define IS_SINGLE_VAR__INTO_OF_RANGE(CDS)                                     \
  ( ( ( CDS.Current > CDS.Max ) || ( CDS.Current < CDS.Min ) ) ? FALSE : TRUE )

/* Only to process value single variables */
#define IS_SINGLE_VAR__TOO_LOW(PVS)                                           \
  ( ( PVS.Value < *(PVS.LowLow) ) ? TRUE : FALSE )
#define IS_SINGLE_VAR__LOW(PVS)                                               \
  ( ( PVS.Value < *(PVS.Low) ) ? TRUE : FALSE )
#define IS_SINGLE_VAR__HIGH(PVS)                                              \
  ( ( PVS.Value > *(PVS.High) ) ? TRUE : FALSE )
#define IS_SINGLE_VAR__TOO_HIGH(PVS)                                          \
  ( (PVS.Value > *(PVS.HighHigh) ) ? TRUE : FALSE )
#define IS_SINGLE_VAR__TOO_ABNORMAL(PVS)                                      \
  ( ( ( PVS.Value > *(PVS.HighHigh) ) || ( PVS.Value < *(PVS.LowLow) ) ) ? TRUE : FALSE )
#define IS_SINGLE_VAR__ABNORMAL(PVS)                                          \
  ( ( ( PVS.Value > *(PVS.High) ) || ( PVS.Value < *(PVS.Low) ) ) ? TRUE : FALSE )
#define IS_SINGLE_VAR__NORMAL(PVS)                                            \
  ( ( ( PVS.Value <= *(PVS.High) ) && ( PVS.Value >= *(PVS.Low) ) ) ? TRUE : FALSE )

/* Master macro for single variables      */
#define IS_SINGLE_VAR(SNGVAR,CONDITION)                                       \
  IS_SINGLE_VAR__##CONDITION(SNGVAR)




/* *****************************************************************************
 DESCRIPTION: Macros to check limits of the matrix variables. It can be applied
              both in matrix configuration data and in matrix process values.
 MACROS:
              IS_MATRIX_VAR
 DESIGNER:    Juliano Varasquim
 CONFIG DATA
 OPTIONS:     HIGHER_THAN_MAX
              LOWER_THAN_MIN
              OUT_OF_RANGE
              INTO_OF_RANGE
 PROCESS VAL
 OPTIONS:     NORMAL
              ABNORMAL
              TOO_ABNORMAL
              TOO_HIGH
              HIGH
              LOW
              TOO_LOW
 EXAMPLE:
              if IS_MATRIX_VAR(Counter, 0, HIGHER_THAN_MAX)  {...}
              if IS_MATRIX_VAR(Baudrate, 4, OUT_OF_RANGE)    {...}
              if IS_MATRIX_VAR(Flow, 2, ABNORMAL)            {...}
              if IS_MATRIX_VAR(Temperature, 2, TOO_HIGH)     {...}
***************************************************************************** */
/* Only to config data matrix variables */
#define IS_MATRIX_VAR__HIGHER_THAN_RANGE(CDM,I)                               \
  (CDM.Current[I] > CDM.Max[I]) ? TRUE : FALSE
#define IS_MATRIX_VAR__LOWER_THAN_RANGE(CDM,I)                                \
  (CDM.Current[I] < CDM.Min[I]) ? TRUE : FALSE
#define IS_MATRIX_VAR__OUT_OF_RANGE(CDM,I)                                    \
  ((CDM.Current[I] > CDM.Max[I])||(CDM.Current[I] < CDM.Min[I])) ? TRUE : FALSE
#define IS_MATRIX_VAR__INTO_OF_RANGE(CDM,I)                                   \
  ((CDM.Current[I] > CDM.Max[I])||(CDM.Current[I] < CDM.Min[I])) ? FALSE : TRUE

/* Only to process value matrix variables */
#define IS_MATRIX_VAR__TOO_LOW(PVM,I)                                         \
  ( ( PVM.Value[I] < *(PVM.LowLow) ) ? TRUE : FALSE )
#define IS_MATRIX_VAR__LOW(PVM,I)                                             \
  ( ( PVM.Value[I] < *(PVM.Low) ) ? TRUE : FALSE )
#define IS_MATRIX_VAR__HIGH(PVM,I)                                            \
  ( ( PVM.Value[I] > *(PVM.High) ) ? TRUE : FALSE )
#define IS_MATRIX_VAR__TOO_HIGH(PVM,I)                                        \
  ( (PVM.Value[I] > *(PVM.HighHigh) ) ? TRUE : FALSE )
#define IS_MATRIX_VAR__TOO_ABNORMAL(PVM,I)                                    \
  ( ( ( PVM.Value[I] > *(PVM.HighHigh) ) || ( PVM.Value[I] < *(PVM.LowLow) ) ) ? TRUE : FALSE )
#define IS_MATRIX_VAR__ABNORMAL(PVM,I)                                        \
  ( ( ( PVM.Value[I] > *(PVM.High) ) || ( PVM.Value[I] < *(PVM.Low) ) ) ? TRUE : FALSE )
#define IS_MATRIX_VAR__NORMAL(PVM,I)                                          \
  ( ( ( PVM.Value[I] <= *(PVM.High) ) && ( PVM.Value[I] >= *(PVM.Low) ) ) ? TRUE : FALSE )

/* Master macro for matrix variables      */
#define IS_MATRIX_VAR(MATVAR,I,CONDITION)                                     \
  IS_MATRIX_VAR__##CONDITION(MATVAR,I)

/* *****************************************************************************
 DESCRIPTION: Macros to check if a given status variable is in a given range
              ( LoLo, Lo, Hi, HiHI )
 MACROS:
              IS_VAR_STATUS
 DESIGNER:    Andre F. N. Dainese
 EXAMPLE:
              if IS_VAR_STATUS(TankStatus, LowValue)         {...}
              if IS_VAR_STATUS(VolFlowStatus, TooHighValue)  {...}

***************************************************************************** */
#define IS_VAR_STATUS( VAR, STATUS )  ( ( VAR & STATUS ) == STATUS )

/* *****************************************************************************
 DESCRIPTION: Macros to apply limits to the "Data Configurations" variables.
              Makes no sense to use this kind of feature with "Process Values"
              and "Diagnostic Values", that is why it does not work with them.
 MACROS:      SATURATE_SINGLE_VAR
              SATURATE_SINGLE_PTR
              SATURATE_MATRIX_VAR
              SATURATE_MATRIX_PTR
 DESIGNER:    Juliano Varasquim
 EXAMPLE:
              APPLY_LIMIT(Mass);
              POINTER_APPLY_LIMIT((uint8 *)Ptr_Map[TableIndex].VarAddres);
***************************************************************************** */

#define SATURATE_SINGLE_VAR(CDS) CDS.Current = ( CDS.Current > CDS.Max) ? CDS.Max :   \
                                               ( ( CDS.Current < CDS.Min) ? CDS.Min : \
                                                 CDS.Current )

#define SATURATE_SINGLE_PTR(TYPE, SPTR)   {                                 \
        TYPE max, min, cur;                                                 \
        cur = *(TYPE *)((void *)SPTR + 0*sizeof(TYPE));                             \
        min = *(TYPE *)((void *)SPTR + 1*sizeof(TYPE));                             \
        max = *(TYPE *)((void *)SPTR + 2*sizeof(TYPE));                             \
        *(TYPE *)SPTR = ( cur > max) ? max : ( ( cur < min) ? min : cur );  \
        }

#define SATURATE_MATRIX_VAR(CDM,I)   CDM.Current[I] = ( CDM.Current[I] > CDM.Max[I]) ? CDM.Max[I] : \
                                                    ( ( CDM.Current[I] < CDM.Min[I]) ? CDM.Min[I] : \
                                                        CDM.Current[I] )

#define SATURATE_MATRIX_PTR(TYPE, MPTR, L) {                                  \
        TYPE max, min, cur;                                                   \
        cur = *(TYPE *)((void *)MPTR + 0*sizeof(TYPE)*L);                             \
        min = *(TYPE *)((void *)MPTR + 1*sizeof(TYPE)*L);                             \
        max = *(TYPE *)((void *)MPTR + 2*sizeof(TYPE)*L);                             \
        *(TYPE *)MPTR = ( cur > max) ? max : ( ( cur < min) ? min : cur );    \
        }




/* *****************************************************************************
 DESCRIPTION: Macro to initialize a variable with n positions.
 MACROS:      INITIALIZE_VARIABLE
 DESIGNER:    Andre F. N Dainese
              Daniel C. Rebeschini
 EXAMPLE:
              static uint8 PULSEMONITOR_Saved_ID[NUMBER_VOL_SENSORS] =
              INITIALIZE_VARIABLE(NUMBER_VOL_SENSORS, 0);
***************************************************************************** */
#define INITIALIZE_VARIABLE( SIZE, VALUE )      { [ 0 ... (SIZE - 1) ] = VALUE }
#define REPEATS_N_TIMES_VALUE( N, VALUE )         [ 0 ... (N - 1) ] = VALUE




/* *****************************************************************************
 DESCRIPTION: Macro that returns the number of elements in a sequence.
 MACROS:      GET_AMOUNT_OF_ITEMS
 DESIGNER:    Juliano Varasquim
 EXAMPLE:
              #define EXAMPLE_LIST 3,1,4,1,5,9,2
              VarAmount = GET_AMOUNT_OF_ITEMS( uint8, EXAMPLE_LIST)  //returns 7
              VarAmount = GET_AMOUNT_OF_ITEMS( uint16, EXAMPLE_LIST) //returns 7
              VarAmount = GET_AMOUNT_OF_ITEMS( float, EXAMPLE_LIST)  //returns 7
***************************************************************************** */
#define GET_AMOUNT_OF_ITEMS( TYPE, LIST ) ( sizeof( (TYPE[]){LIST} ) / sizeof(TYPE) )




/* *****************************************************************************
 DESCRIPTION: Set of macros used to calculates the boundary of arrays.
 MACROS:      ARRAY_FUL_LEN
              ARRAY_IDX_MAX
 DESIGNER:    Juliano Varasquim
 EXAMPLE:
              uint8 ExArray[] =
                {
                8,
                2,
                1,
                9
                }
              ArrayLen = ARRAY_FUL_LEN(ExArray)   //returns 4
              ArrayMax = ARRAY_IDX_MAX(ExArray)   //returns 3
***************************************************************************** */
#define ARRAY_FUL_LEN( ARR ) ( sizeof( ARR ) / sizeof( ARR[0] ) )
#define ARRAY_IDX_MAX( ARR ) ( ARRAY_FUL_LEN( ARR ) - 1 )



/* *****************************************************************************
 DESCRIPTION: Macro to declare a structure with the variables' address and size
 MACROS:      VAR_PTR_SIZE_PAIR
 DESIGNER:    Andre F. N Dainese
              Daniel C. Rebeschini
 EXAMPLE:
            VAR_PTR_SIZE_PAIR(MeterSerialNum.Value),
***************************************************************************** */
#define VAR_PTR_SIZE_PAIR(VAR)                 { &VAR, sizeof(VAR) }




/* *****************************************************************************
 DESCRIPTION: Macro to clear a variable's contents (every byte is cleared to
              zero).
 MACROS:      CLEAR_VARIABLE
 DESIGNER:    Andre F. N Dainese
 EXAMPLE:
         CLEAR_VARIABLE( VARIABLE );
***************************************************************************** */
#define CLEAR_VARIABLE(VAR)             ((void) memset(&VAR, 0x00, sizeof(VAR)))




/* *****************************************************************************
 DESCRIPTION: Macros to defines maximum and the minimum value between two
              variables
 MACROS:      MAX
              MIN
 DESIGNER:    Juliano Varasquim
 EXAMPLE:
              c = MAX(a, b);
WARNING:      The macro does not checks the type of the variables, therefore,
              make sure that what you are doing makes sense!
***************************************************************************** */
#define MAX(A,B)                 ((A>B) ? A : B )
#define MIN(A,B)                 ((A<B) ? A : B )




/* *****************************************************************************
 DESCRIPTION: Macro for when you set the logging event.
 MACROS:      SET_LOG_WHEN
 DESIGNER:    Andre F. N Dainese
              Daniel C. Rebeschini
 EXAMPLE:
          SET_LOG_WHEN((MeterSealState.Current == TRUE), ViolationDetected, LogDoneFlag[18], ExecuteLog);
************************************************************************************************************ */
#define SET_LOG_WHEN(COND, FLAG_ENUM, LOG_DONE_FLAG, EXECUTE_LOG){             \
      if(COND == FALSE)                                                        \
      {                                                                        \
          LOG_DONE_FLAG = FALSE;                                               \
      }                                                                        \
      else{                                                                    \
        if(LOG_DONE_FLAG == FALSE)                                             \
        {                                                                      \
          LogSourceFlagArray.Value |= FLAG_ENUM;                               \
          EXECUTE_LOG   = TRUE;                                                \
          LOG_DONE_FLAG = TRUE;                                                \
        }                                                                      \
      }                                                                        \
    }                                                                          \




/* *****************************************************************************
 DESCRIPTION: Macro for when you set the logging event - TRANSITION MODE.
              The event is logged WHEN it changes to the condition AND when
              leaves it
 MACROS:      SET_LOG_TRNS
 DESIGNER:    Andre F. N Dainese
 EXAMPLE:
              SET_LOG_TRNS((MeterSealState.Current == TRUE), ViolationDetected, LogDoneFlag[18], ExecuteLog);
***************************************************************************** */
#define SET_LOG_TRNS(COND, FLAG_ENUM, LOG_DONE_FLAG, EXECUTE_LOG){             \
      bool Condition = COND ;                                                  \
      if(Condition != LOG_DONE_FLAG)                                           \
      {                                                                        \
          LogSourceFlagArray.Value |= FLAG_ENUM;                               \
          EXECUTE_LOG = TRUE;                                                  \
      }                                                                        \
      LOG_DONE_FLAG = Condition;                                               \
    }                                                                          \




/* *****************************************************************************
 DESCRIPTION: Macro to check if current branch is supported by application.
 MACROS:      CHECK_BRANCH
 DESIGNER:    Denis Beraldo

 EXAMPLE:     #if CHECK_BRANCH(DATA_LOGGER_BRANCH_MASTER)
              #error "Datalogger branch error!!"
              #endif
***************************************************************************** */
#define CHECK_BRANCH  !defined




/* *****************************************************************************
 DESCRIPTION: Macro to check if current version is supported by application.
 MACROS:      CHECK_VERSION
 DESIGNER:    Denis Beraldo

 EXAMPLE:     #if CHECK_VERSION(DATA_LOGGER_VER, 1,10)
              #error "Datalogger version error!!"
              #endif
***************************************************************************** */
#ifndef __CONCAT
  #define __CONCAT1(x,y)  x ## y
  #define __CONCAT(x,y) __CONCAT1(x,y)
#endif

#define CHECK_VERSION(__PREFIX, __MAJOR, __MINOR)                     \
                    ((__CONCAT(__PREFIX##_, MAJOR) != __MAJOR)  ||    \
                     (__CONCAT(__PREFIX##_, MINOR) != __MINOR))       \




/* *****************************************************************************
 DESCRIPTION: Macros used to set and clear bits of variables based on masks
 MACROS:      SET_BITS
              CLR_BITS
 DESIGNER:    Juliano Varasquim
 EXAMPLE:
              #define FAIL 0x0400
              uint16 Status = 0x0000;
                                                             Status -> 0x0000
              SET_BITS(Status,USING_THE_MASK,FAIL);          Status -> 0x0400
              CLR_BITS(Status,USING_THE_MASK,FAIL);          Status -> 0x0000
              SET_BITS(Status,USING_NEG_MASK,FAIL);          Status -> 0xFBFF
              CLR_BITS(Status,USING_THE_MASK,0xF0);          Status -> 0xFB0F
              CLR_BITS(Status,USING_THE_MASK,0x0F);          Status -> 0xFB00
              CLR_BITS(Status,USING_THE_MASK,0xF000);        Status -> 0x0B00

WARNING:      The macro does not checks the type of the variables, therefore,
              make sure that what you are doing makes sense!
***************************************************************************** */
#define SET_BITS__USING_THE_MASK(VAR,MASK) VAR = VAR | (__typeof__(VAR))(MASK)
#define SET_BITS__USING_NEG_MASK(VAR,MASK) VAR = VAR | (__typeof__(VAR))(~MASK)
#define CLR_BITS__USING_THE_MASK(VAR,MASK) VAR = VAR & (__typeof__(VAR))(~MASK)
#define CLR_BITS__USING_NEG_MASK(VAR,MASK) VAR = VAR & (__typeof__(VAR))(MASK)

#define SET_BITS(VAR,USING,MASK) SET_BITS__##USING(VAR,MASK)
#define CLR_BITS(VAR,USING,MASK) CLR_BITS__##USING(VAR,MASK)



/* *****************************************************************************
 DESCRIPTION: Macro used to concatenate two uint8 numbers into an uint16.
              Useful when representing versions inside a word (major,minor)
 MACROS:      CONC2BYTE
 DESIGNER:    ???
 EXAMPLE:
              Var = CONC2BYTE( 15, 5 ) -> Var = 0x0f05
              CREATE_DIAGNOS_VAL_STRUCT( uint16, HardwareCode, CONC2BYTE(15,5));

***************************************************************************** */
#define CONC2BYTE(H,L)                                  \
           ( ( ( ( (uint16)H ) << 8 ) & 0xFF00 ) | ( ( (uint16)L  ) & 0x00FF ) )



/* *****************************************************************************
  DESCRIPTION:  Creates a copy of original variable with the same type
                (uint8, uint32, etc).
                The copied variable use DIAGNOS_VAL_STRUCT type.
  MACRO:        COPY_SINGLE_VAR
  DESIGNER:     Denis Beraldo

  PARAMETERS
  Name:         Input variable name.
  Prefix:       Prefix of output variable name.
  Local:        Declaration mode (INTERN / EXTERN).

  EXAMPLE:      COPY_SINGLE_VAR( Date,      SD,     INTERN );
***************************************************************************** */
#define COPY_SINGLE_VAR(_Name_, _Prefix_, _Local_)      \
    COPY_SINGLE_VAR_##_Local_(_Name_, _Prefix_)

/* Macro called by COPY_SINGLE_VAR - only internal use. */
#define COPY_SINGLE_VAR_INTERN(_Name_, _Prefix_)        \
    CREATE_DIAGNOS_VAL_STRUCT(__typeof__ (_Name_.Value), _Prefix_##_##_Name_, 0)

/* Macro called by COPY_SINGLE_VAR - only internal use. */
#define COPY_SINGLE_VAR_EXTERN(_Name_, _Prefix_)        \
    EXTERN_DIAGNOS_VAL_STRUCT(__typeof__ (_Name_.Value), _Prefix_##_##_Name_)
/* ************************************************************************** */


/* *****************************************************************************
  DESCRIPTION:  Creates a copy of original variable with the same type
                (uint8, uint32, etc).
                The copied variable use DIAGNOS_VAL_STRUCT type.
  MACRO:        COPY_MATRIX_VAR
  DESIGNER:     Denis Beraldo

  PARAMETERS
  Name:         Input variable name.
  Prefix:       Prefix of output variable name.
  Local:        Declaration mode (INTERN / EXTERN).
  Size:         Array size.

  EXAMPLE:      COPY_MATRIX_VAR( VolFlow_Avg,     SD,     INTERN,   5  );
***************************************************************************** */
#define COPY_MATRIX_VAR(_Name_, _Prefix_, _Local_, _Size_)  \
  COPY_MATRIX_VAR_##_Local_(_Name_, _Prefix_, _Size_)

/* Macro called by COPY_MATRIX_VAR - only internal use. */
#define COPY_MATRIX_VAR_INTERN(_Name_, _Prefix_, _Size_)  \
  CREATE_DIAGNOS_VAL_MATRIX(__typeof__(_Name_.Value[0]), _Prefix_##_##_Name_, _Size_, \
    [ 0 ... (_Size_ - 1) ] = 0)

/* Macro called by COPY_MATRIX_VAR - only internal use. */
#define COPY_MATRIX_VAR_EXTERN(_Name_, _Prefix_, _Size_)  \
  EXTERN_DIAGNOS_VAL_MATRIX(__typeof__(_Name_.Value[0]), _Prefix_##_##_Name_, _Size_)
/* ************************************************************************** */


/* *****************************************************************************
  DESCRIPTION:  Creates a copy of a single big float structure.
                The copied variable use DIAGNOS_VAL_STRUCT type.
  MACRO:        COPY_SINGLE_BIGF
  DESIGNER:     Denis Beraldo

  PARAMETERS
  Name:         Input variable name.
  Prefix:       Prefix of output variable name.
  Local:        Declaration mode (INTERN / EXTERN).

  EXAMPLE:      COPY_SINGLE_BIGF( Temp_Avg,     SD,     INTERN );
***************************************************************************** */
#define COPY_SINGLE_BIGF(_Name_, _Prefix_, _Local_) \
  COPY_SINGLE_BIGF_##_Local_(_Name_, _Prefix_)

/* Macro called by COPY_SINGLE_BIGF - only internal use. */
#define COPY_SINGLE_BIGF_INTERN(_Name_, _Prefix_) \
  CREATE_DIAGNOS_VAL_STRUCT(BigFloat_t,   _Prefix_##_##_Name_,  BIGFLOAT_INIT)

/* Macro called by COPY_SINGLE_BIGF - only internal use. */
#define COPY_SINGLE_BIGF_EXTERN(_Name_, _Prefix_) \
  EXTERN_DIAGNOS_VAL_STRUCT(BigFloat_t,   _Prefix_##_##_Name_)
/* ************************************************************************** */


/* *****************************************************************************
  DESCRIPTION:  Creates a copy of a single big float structure.
                The copied variable use DIAGNOS_VAL_STRUCT type.
  MACRO:        COPY_MATRIX_BIGF
  DESIGNER:     Denis Beraldo

  PARAMETERS
  Name:         Input variable name.
  Prefix:       Prefix of output variable name.
  Local:        Declaration mode (INTERN / EXTERN).
  Size:         Array size.

  EXAMPLE:      COPY_MATRIX_BIGF( VolFlow_Avg,      SD,     INTERN,   5  );
***************************************************************************** */
#define COPY_MATRIX_BIGF(_Name_, _Prefix_, _Local_, _Size_) \
  COPY_MATRIX_BIGF_##_Local_(_Name_, _Prefix_, _Size_)

/* Macro called by COPY_MATRIX_BIGF - only internal use. */
#define COPY_MATRIX_BIGF_INTERN(_Name_, _Prefix_, _Size_) \
  CREATE_DIAGNOS_VAL_MATRIX(BigFloat_t, _Prefix_##_##_Name_, _Size_,  \
    [0 ... (_Size_ - 1)] = BIGFLOAT_INIT)

/* Macro called by COPY_MATRIX_BIGF - only internal use. */
#define COPY_MATRIX_BIGF_EXTERN(_Name_, _Prefix_, _Size_) \
  EXTERN_DIAGNOS_VAL_MATRIX(BigFloat_t, _Prefix_##_##_Name_, _Size_)
/* ************************************************************************** */


/* *****************************************************************************
  DESCRIPTION:  Creates a pair of variables pointers.
                The variable pair follows the rule PREFIX"_"NAME.

  MACRO:        ADD_SINGLE_VAR
  DESIGNER:     Denis Beraldo

  PARAMETERS
  Name:         Input variable name.
  Prefix:       Name of pair with prefix.

  EXAMPLE:      ADD_SINGLE_VAR(   Date,   SD  ),
***************************************************************************** */
#define ADD_SINGLE_VAR(_Name_, _Prefix_)  \
  {                                       \
    &(_Name_.Value),                      \
    &(_Prefix_##_##_Name_.Value),         \
    sizeof(_Name_.Value)                  \
  }                                       \
/* ****************************************************************************/


/* *****************************************************************************
  DESCRIPTION:  Creates a pair of big floats pointers.
                The variable pair follows the rule PREFIX"_"NAME.

  MACRO:        ADD_SINGLE_BIGF
  DESIGNER:     Denis Beraldo

  PARAMETERS
  Name:         Input variable name.
  Type:         Structure element (Upper / Lower).
  Prefix:       Name of pair with prefix.

  EXAMPLE:      ADD_SINGLE_BIGF(  TotDiff_TotOut,   Upper,    SD  )
***************************************************************************** */
#define ADD_SINGLE_BIGF(_VarName_, _Type_, _Prefix_)    \
  {                                                     \
    &(_VarName_.Value._Type_),                          \
    &(_Prefix_##_##_VarName_.Value._Type_),             \
    sizeof(_Prefix_##_##_VarName_.Value._Type_)         \
  }                                                     \
/* ****************************************************************************/


/* *****************************************************************************
  DESCRIPTION:  Creates a pair of matrix variables pointers.
                The variable pair follows the rule PREFIX"_"NAME.

  MACRO:        ADD_MATRIX_VAR
  DESIGNER:     Denis Beraldo

  PARAMETERS
  Name:         Input variable name.
  Prefix:       Name of pair with prefix.
  IDx1:         Index of matrix 1.
  IDx2:         Index of matrix 2.

  EXAMPLE:      ADD_MATRIX_VAR( VolFlow_Avg,  SD, [2],  [2]),
***************************************************************************** */
#define ADD_MATRIX_VAR(_VarName_, _Prefix_, _IDx1_, IDx2_)    \
  {                                                           \
    &(_VarName_.Value _IDx1_ ),                               \
    &(_Prefix_##_##_VarName_.Value IDx2_ ),                   \
    sizeof(_Prefix_##_##_VarName_.Value IDx2_ )               \
  }                                                           \
/* ****************************************************************************/


/* *****************************************************************************
  DESCRIPTION:  Creates a pair of big floats pointers.
                The variable pair follows the rule PREFIX"_"NAME.

  MACRO:        ADD_MATRIX_BIGF
  DESIGNER:     Denis Beraldo

  PARAMETERS
  Name:         Input variable name.
  Type:         Structure element (Upper / Lower).
  Prefix:       Name of pair with prefix.
  IDx1:         Index of matrix 1.
  IDx2:         Index of matrix 2.

  EXAMPLE:      ADD_MATRIX_BIGF( TotDiff_TotOut,  Upper,  SD,  [3], [3]),
***************************************************************************** */
#define ADD_MATRIX_BIGF(_VarName_, _Type_, _Prefix_, _IDx1_, IDx2_)   \
  {                                                                   \
    &(_VarName_.Value _IDx1_ ._Type_),                                \
    &(_Prefix_##_##_VarName_.Value IDx2_ ._Type_),                    \
    sizeof(_Prefix_##_##_VarName_.Value IDx2_ ._Type_)                \
  }                                                                   \
/* ****************************************************************************/




/* *****************************************************************************
  DESCRIPTION:  Performs the source code sequence written in "ACT" only once.

  MACRO:        DO_ONLY_ONCE
  DESIGNER:     Juliano Varasquim

  PARAMETERS
    ACT:        Source code which will be processed only once

  EXAMPLE:      ADD_MATRIX_BIGF( TotDiff_TotOut,  Upper,  SD,   3,  3 ),
***************************************************************************** */
#define DO_ONLY_ONCE(ACT) {                                                     \
  static bool isFirstTimeHere = TRUE;                                           \
  if(isFirstTimeHere)                                                           \
    {                                                                           \
    ACT                                                                         \
    isFirstTimeHere = FALSE;                                                    \
    }                                                                           \
  }



/* *****************************************************************************
  DESCRIPTION:  Converts the four paratemers to a IPv4_t variable

  MACRO:        IPV4_VALUE
  DESIGNER:     Andre F. N. Dainese

  PARAMETERS
    Four IP bytes

  EXAMPLE:      IPV4_VALUE( 192, 168, 1, 0 )
***************************************************************************** */
#define IPV4_VALUE( B1, B2, B3, B4 ) (uint32)( ( ( B1 & 0xFF ) << 24 ) |        \
                                               ( ( B2 & 0xFF ) << 16 ) |        \
                                               ( ( B3 & 0xFF ) << 8  ) |        \
                                               ( ( B4 & 0xFF ) << 0  ) )




/* *****************************************************************************
  DESCRIPTION:  Returns the index of the highest element of the array

  MACRO:        HIGHEST_INDEX_OF
  DESIGNER:     Juliano Varasquim

  PARAMETERS
    Array name

  EXAMPLE:      HIGHEST_INDEX_OF( Array )
***************************************************************************** */
#define HIGHEST_INDEX_OF( ARR ) ( (sizeof( ARR ) / sizeof( ARR[0] ) ) - 1 )




/* *****************************************************************************
  DESCRIPTION:  Returns the value into the last element of the array

  MACRO:        VALUE_LAST_ELEM_OF
  DESIGNER:     Juliano Varasquim

  PARAMETERS
    Array name

  EXAMPLE:      VALUE_LAST_ELEM_OF( array )
***************************************************************************** */
#define VALUE_LAST_ELEM_OF( ARR ) ( ARR[ HIGHEST_INDEX_OF( ARR ) ] )




/* *****************************************************************************
  DESCRIPTION:  Coordinates a state machine variable according to the result
                  given, so that the state machine can be executed sequentially
                  and with less repeated code at each state.

  MACRO:        MANAGE_STATE_MACHINE_UNTIL_AND_RETURN
  DESIGNER:     Andre F. N. Dainese

  PARAMETERS
    VAR : Variable that coordinates the state machine. Can be incremented or
            cleared by this macro.
    END_STATE : Should hold the state machine's last state value PLUS ONE. If
                  state machine variable reaches this value then it'll be
                  cleared.
    RET : Variable that holds the state machine's return value result. According
            to it the state machine variable will be coordinated. Can be
            modified by this macro.

  DETAILS
  This macro is used at the end of several routines that are built as
   a sequence of steps. It simply keeps incrementing the value of a given 
   state machine variable until it reaches a limit. When it reaches there, it 
   puts the variable back to the first value. Meanwhile, it treats the return 
   value variable to keep returning that operation is running until the end.
  How the return variable finishes the state machine (i.e is passed to this
   macro) determines the macro's operation:
  - OPERATION_RUNNING : State is not changed.
  - ANSWERED_REQUEST : State is incremented. If it reaches the END_STATE, it
     returns to the first one (value zero) and keep the answered request
     answer. Otherwise, it changes it to operation running.
  - OPERATION_IDLE : State is returned to the first one and return is
     changed to answered request.
  - Else : State is returned to the first one and return value is kept.

  EXAMPLE:      Check the CR95HF or the SX1276_LORA_PPP source code.
***************************************************************************** */
#define MANAGE_STATE_MACHINE_UNTIL_AND_RETURN( VAR, END_STATE, RET )           \
{                                                                              \
  if( RET != OPERATION_RUNNING )                                               \
  {                                                                            \
    if( RET == ANSWERED_REQUEST )                                              \
    {                                                                          \
      VAR++;                                                                   \
                                                                               \
      if( VAR >= END_STATE ) { VAR = 0; }                                      \
      else                   { RET = OPERATION_RUNNING; }                      \
    }                                                                          \
    else if( RET == OPERATION_IDLE )                                           \
    {                                                                          \
      VAR = 0;                                                                 \
      RET = ANSWERED_REQUEST;                                                  \
    }                                                                          \
    else                                                                       \
    {                                                                          \
      VAR = 0;                                                                 \
    }                                                                          \
  }                                                                            \
}















/* *****************************************************************************

 #######                      #     #
 #     #  #       #####       ##   ##    ##     ####   #####    ####    ####
 #     #  #       #    #      # # # #   #  #   #    #  #    #  #    #  #
 #     #  #       #    #      #  #  #  #    #  #       #    #  #    #   ####
 #     #  #       #    #      #     #  ######  #       #####   #    #       #
 #     #  #       #    #      #     #  #    #  #    #  #   #   #    #  #    #
 #######  ######  #####       #     #  #    #   ####   #    #   ####    ####



***************************************************************************** */

/* *****************************************************************************
 *
 * THESE MACROS HAVE BEEN DISABLED AND SHOULD NOT BE USED, USE AS DECLARED ABOVE
 * MACROS.
 *
 *******************************************************************************
 DESCRIPTION:
 MACROS:    CONFIG_STRUCT
            EXTERN_CONFIG_STRUCT
            PROCESS_VAR_STRUCT
            EXTERN_PROCESS_VAR_STRUCT

 DESIGNER:  Daniel C. Rebeschini
 EXAMPLE:
          CONFIG_STRUCT(UnitMass_t, MassUnit, _gr, _kg, _ton);
          PROCESS_VAR_STRUCT(float, Mass, 1.0, 0.0, 10.0);
***************************************************************************** */

/* Configuration struct definition */
#define CONFIG_STRUCT(Type,Name,c,mi,ma,L) struct {           \
          Type Current##L;                                    \
          const Type Min##L;                                  \
          const Type Max##L;                                  \
          const Type Default##L;                              \
          } Name =                                            \
            {                                                 \
            c,                                                \
            mi,                                               \
            ma,                                               \
            c,                                                \
            }
#define EXTERN_CONFIG_STRUCT(Type,Name,L)  extern struct      \
          {                                                   \
          Type Current##L;                                    \
          const Type Min##L;                                  \
          const Type Max##L;                                  \
          const Type Default##L;                              \
          } Name

/* Process Variables struct definition */
#define PROCESS_VAR_STRUCT(Type,Name,c,mi,ma,L)  struct {     \
          Type Current##L;                                    \
          Type Min##L;                                        \
          Type Max##L;                                        \
          } Name =                                            \
            {                                                 \
            c,                                                \
            mi,                                               \
            ma,                                               \
            }
#define EXTERN_PROCESS_VAR_STRUCT(Type,Name,L)  extern struct \
          {                                                   \
          Type Current##L;                                    \
          Type Min##L;                                        \
          Type Max##L;                                        \
          } Name




/* *****************************************************************************
 *
 * THESE MACROS HAVE BEEN DISABLED AND SHOULD NOT BE USED, USE AS DECLARED ABOVE
 * MACROS.
 *
 *******************************************************************************
 DESCRIPTION:
 MACROS:    IS_OUT_OF_RANGE
            IS_HIGHER_THAN_RANGE
            IS_LOWER_THAN_RANGE
 DESIGNER:  Daniel C. Rebeschini
 EXAMPLE:
            if(IS_OUT_OF_RANGE(MODBUS_DeviceAddress)){}
            if(IS_OUT_OF_RANGE(MODBUS_DeviceAddress)){}
            if(IS_OUT_OF_RANGE(MODBUS_DeviceAddress)){}
***************************************************************************** */
#define IS_OUT_OF_RANGE(VARIABLE)  ( (VARIABLE.Current > VARIABLE.Max) || (VARIABLE.Current < VARIABLE.Min) ) ? TRUE : FALSE
#define IS_HIGHER_THAN_RANGE(VARIABLE)  (VARIABLE.Current > VARIABLE.Max) ? TRUE : FALSE
#define IS_LOWER_THAN_RANGE(VARIABLE)  (VARIABLE.Current < VARIABLE.Min) ? TRUE : FALSE




/* *****************************************************************************
 *
 * THESE MACROS HAVE BEEN DISABLED AND SHOULD NOT BE USED, USE AS DECLARED ABOVE
 * MACROS.
 *
 *******************************************************************************
 DESCRIPTION: Macros to apply limits to the variables
 MACROS:    APPLY_LIMIT
            POINTER_TO_ARRAY_APPLY_LIMIT
            POINTER_APPLY_LIMIT
 DESIGNER:  Daniel C. Rebeschini
 EXAMPLE:
         APPLY_LIMIT(Mass);
         POINTER_APPLY_LIMIT((uint8 *)Ptr_Map[TableIndex].VarAddres);
***************************************************************************** */
#define APPLY_LIMIT(VARIABLE) VARIABLE.Current = (VARIABLE.Current > VARIABLE.Max) ?  \
                                  VARIABLE.Max :                                      \
                                  ( (VARIABLE.Current < VARIABLE.Min) ?               \
                                      VARIABLE.Min :                                  \
                                      VARIABLE.Current)
#define POINTER_TO_ARRAY_APPLY_LIMIT(PTVAR,LENGTH)  *(PTVAR) = (*(PTVAR) > *(PTVAR+(2*LENGTH))) ? \
                                        *(PTVAR+(2*LENGTH)) :                                     \
                                        ( (*(PTVAR) < *(PTVAR+LENGTH)) ?                          \
                                          *(PTVAR+LENGTH) : *(PTVAR))
#define POINTER_APPLY_LIMIT(oPTVAR) POINTER_TO_ARRAY_APPLY_LIMIT(oPTVAR,1)


/* *****************************************************************************

*

* THESE MACROS IS USED TO EXECUTION TIME LIB ANALYZES ON STM32F407

*

*******************************************************************************/

/* DWT (Data Watchpoint and Trace) registers, only exists on ARM Cortex with a DWT unit */

  #define KIN1_DWT_CONTROL             (*((volatile uint32_t*)0xE0001000))

    /*!< DWT Control register */

  #define KIN1_DWT_CYCCNTENA_BIT       (1UL<<0)

    /*!< CYCCNTENA bit in DWT_CONTROL register */

  #define KIN1_DWT_CYCCNT              (*((volatile uint32_t*)0xE0001004))

    /*!< DWT Cycle Counter register */

  #define KIN1_DEMCR                   (*((volatile uint32_t*)0xE000EDFC))

    /*!< DEMCR: Debug Exception and Monitor Control Register */

  #define KIN1_TRCENA_BIT              (1UL<<24)

    /*!< Trace enable bit in DEMCR register */

 

#define KIN1_InitCycleCounter() \
  KIN1_DEMCR |= KIN1_TRCENA_BIT

  /*!< TRCENA: Enable trace and debug block DEMCR (Debug Exception and Monitor Control Register */

 

#define KIN1_ResetCycleCounter() ; \
//  KIN1_DWT_CYCCNT = 0
  /*!< Reset cycle counter */

 

#define KIN1_EnableCycleCounter() \
  KIN1_DWT_CONTROL |= KIN1_DWT_CYCCNTENA_BIT
  /*!< Enable cycle counter */

#define KIN1_DisableCycleCounter() \
  KIN1_DWT_CONTROL &= ~KIN1_DWT_CYCCNTENA_BIT
  /*!< Disable cycle counter */

#define KIN1_GetCycleCounter() \
  KIN1_DWT_CYCCNT

  /*!< Read cycle counter register */

#define START_EXECUTION_TIME_MEASUREMENT()\
  KIN1_InitCycleCounter(); \
  KIN1_ResetCycleCounter(); \
  KIN1_EnableCycleCounter(); \

#define GET_EXEC_TIME_US() \
KIN1_GetCycleCounter() \
//  0.00595*KIN1_GetCycleCounter();

#endif
