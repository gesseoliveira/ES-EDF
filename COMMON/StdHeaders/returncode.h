/*____________________________________________________________________________

 FILE_NAME:     ReturnCode.h
 DESCRIPTION:   Common return code values for all projects
 DESIGNER:      Andre F. N Dainese / Daniel C. Rebeschini
 CREATION_DATE: 08/2015

 VERSION:       0.1
______________________________________________________________________________*/

#ifndef RETURN_CODE_H
#define RETURN_CODE_H

/***** FIRMWARE VERSION ***************************************************** */
#define COMMON_RETURNCODE_VER_MAJOR		0
#define COMMON_RETURNCODE_VER_MINOR		1
#define COMMON_BRANCH_MASTER

/* This header file contains generic return codes, designed to fit the needs  */
/*  from all projects, so that all of them can use these values to represent  */
/*  their information.                                                        */
/* It consists of an enumeration, where each value has its fixed code. The    */
/*  return values can represent 'error' or 'information' values. There is an  */
/*  item in the enumeration that separates them.                              */

typedef enum
{
/***** INFORMATION TYPE RETURN LIST *******************************************/
/* NAME --------------------------- VALUE ---- DESCRIPTION                    */
  ANSWERED_REQUEST                = 0x0000, /* Usual answer. Conclusion.      */
  OPERATION_IDLE                  = 0x0001, /* No specific operation is running.                              */
  OPERATION_RUNNING               = 0x0002, /* Routine is busy with its task or with the caller-related task. */
  FUNCTION_BUSY                   = 0x0003, /* Routine is busy with a task related with another caller.       */
  VARIABLE_UPDATED                = 0x0004,
/******** TYPE DELIMITER ITEM *************************************************/
  RETURN_ERROR_VALUE              = 0x8000, /* From this value onwards the answer is of error type.           */
/******* ERROR TYPE RETURN LIST ***********************************************/
  ERR_SPEED                       = 0x8001, /* This device does not work in the active speed mode.            */
  ERR_RANGE                       = 0x8002, /* Parameter out of range.                                        */ 
  ERR_VALUE                       = 0x8003, /* Parameter of incorrect value.                                  */
  ERR_OVERFLOW                    = 0x8004, /* Timer overflow.                                                */
  ERR_MATH                        = 0x8005, /* Mathematical error.                                            */
  ERR_ENABLED                     = 0x8006, /* Device is enabled.                                             */
  ERR_DISABLED                    = 0x8007, /* Device is disabled.                                            */
  ERR_BUSY                        = 0x8008, /* Device is busy.                                                */
  ERR_NOTAVAIL                    = 0x8009, /* Requested value or method not available.                       */
  ERR_RXEMPTY                     = 0x800A, /* No data in receiver.                                           */
  ERR_TXFULL                      = 0x800B, /* Transmitter is full.                                           */
  ERR_BUSOFF                      = 0x800C, /* Bus not available.                                             */
  ERR_OVERRUN                     = 0x800D, /* Overrun error is detected.                                     */
  ERR_FRAMING                     = 0x800E, /* Framing error is detected.                                     */
  ERR_PARITY                      = 0x800F, /* Parity error is detected.                                      */
  ERR_NOISE                       = 0x8010, /* Noise error is detected.                                       */
  ERR_IDLE                        = 0x8011, /* Idle error is detected.                                        */
  ERR_FAULT                       = 0x8012, /* Fault error is detected.                                       */
  ERR_BREAK                       = 0x8013, /* Break char is received during communication.                   */
  ERR_CRC                         = 0x8014, /* CRC error is detected.                                         */
  ERR_ARBITR                      = 0x8015, /* A node losts arbitration (communication conflict).             */
  ERR_PROTECT                     = 0x8016, /* Protection error is detected.                                  */
  ERR_UNDERFLOW                   = 0x8017, /* Underflow error is detected.                                   */
  ERR_UNDERRUN                    = 0x8018, /* Underrun error is detected.                                    */
  ERR_COMMON                      = 0x8019, /* Common error of a device.                                      */
  ERR_LINSYNC                     = 0x801A, /* LIN synchronization error is detected.                         */
  ERR_FAILED                      = 0x801B, /* Requested functionality or process failed.                     */
  ERR_QFULL                       = 0x801C, /* Queue is full.                                                 */
  ERR_PARAM_MASK                  = 0x8080, /* Invalid mask.                                                  */
  ERR_PARAM_MODE                  = 0x8081, /* Invalid mode.                                                  */
  ERR_PARAM_INDEX                 = 0x8082, /* Invalid index.                                                 */
  ERR_PARAM_DATA                  = 0x8083, /* Invalid data.                                                  */
  ERR_PARAM_SIZE                  = 0x8084, /* Invalid size.                                                  */
  ERR_PARAM_VALUE                 = 0x8085, /* Invalid value.                                                 */
  ERR_PARAM_RANGE                 = 0x8086, /* Invalid parameter's range or parameters' combination.          */
  ERR_PARAM_LOW_VALUE             = 0x8087, /* Invalid value (LOW part).                                      */
  ERR_PARAM_HIGH_VALUE            = 0x8088, /* Invalid value (HIGH part).                                     */
  ERR_PARAM_ADDRESS               = 0x8089, /* Invalid address.                                               */
  ERR_PARAM_PARITY                = 0x808A, /* Invalid parity.                                                */
  ERR_PARAM_WIDTH                 = 0x808B, /* Invalid width.                                                 */
  ERR_PARAM_LENGTH                = 0x808C, /* Invalid length.                                                */
  ERR_PARAM_ADDRESS_TYPE          = 0x808D, /* Invalid address type.                                          */
  ERR_PARAM_COMMAND_TYPE          = 0x808E, /* Invalid command type.                                          */
  ERR_PARAM_COMMAND               = 0x808F, /* Invalid command.                                               */
  ERR_PARAM_RECIPIENT             = 0x8090, /* Invalid recipient.                                             */
  ERR_PARAM_BUFFER_COUNT          = 0x8091, /* Invalid buffer count.                                          */
  ERR_PARAM_ID                    = 0x8092, /* Invalid ID.                                                    */
  ERR_PARAM_GROUP                 = 0x8093, /* Invalid group.                                                 */
  ERR_PARAM_CHIP_SELECT           = 0x8094, /* Invalid chip select.                                           */
  ERR_PARAM_ATTRIBUTE_SET         = 0x8095, /* Invalid set of attributes.                                     */
  ERR_PARAM_SAMPLE_COUNT          = 0x8096, /* Invalid sample count.                                          */
  ERR_PARAM_CONDITION             = 0x8097, /* Invalid condition.                                             */
  ERR_PARAM_TICKS                 = 0x8098, /* Invalid ticks parameter.                                       */
  ERR_PARAM_INIT                  = 0x8099, /* Invalid init parameter.                                        */
  ERR_PARAM_CONFIG                = 0x809A, /* Invalid config parameter.                                      */
  ERR_DEVICE                      = 0x809B, /* Error with target device.                                      */
	ERR_NO_FILE                     = 0x809C, /* Could not find the file.                                       */
	ERR_FILE_EMPTY                  = 0x809D, /* File empty.                                                    */
	ERR_MEM_EMPTY                   = 0x809E, /* Memory empty.                                                  */
	ERR_LEVEL_0                     = 0x80A0, /* Error description from the severity perspective - Level 0      */
	ERR_LEVEL_1                     = 0x80A1, /* Error description from the severity perspective - Level 1      */
	ERR_LEVEL_2                     = 0x80A2, /* Error description from the severity perspective - Level 2      */
	ERR_LEVEL_3                     = 0x80A3, /* Error description from the severity perspective - Level 3      */
	ERR_LEVEL_4                     = 0x80A4, /* Error description from the severity perspective - Level 4      */
	ERR_LEVEL_5                     = 0x80A5, /* Error description from the severity perspective - Level 5      */
	ERR_LEVEL_6                     = 0x80A6, /* Error description from the severity perspective - Level 6      */
	ERR_LEVEL_7                     = 0x80A7, /* Error description from the severity perspective - Level 7      */
	ERR_LEVEL_8                     = 0x80A8, /* Error description from the severity perspective - Level 8      */
	ERR_LEVEL_9                     = 0x80A9, /* Error description from the severity perspective - Level 9      */
} ReturnCode_t;


#endif


