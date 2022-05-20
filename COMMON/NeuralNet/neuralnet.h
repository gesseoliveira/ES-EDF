/* *****************************************************************************
 FILE_NAME:     NeuralNet.h
 DESCRIPTION:   Software library with neural network implementations
 DESIGNER:      Juliano Varasquim
 CREATION_DATE: 19/jul/2016
 VERSION:       2.1
***************************************************************************** */
#ifndef NEURALNET_H_INCLUDED
#define NEURALNET_H_INCLUDED




/* *****************************************************************************
 *
 *        INCLUDES (and DEFINES for INCLUDES)
 *
***************************************************************************** */
#include "Types.h"
#include "ReturnCode.h"
#include "Macros.h"
#include "setup.h"



/* *****************************************************************************
 *
 *        FIRMWARE VERSION
 *
***************************************************************************** */
#define COMMON_NEURALNET_VER_MAJOR	2
#define COMMON_NEURALNET_VER_MINOR	0
#define COMMON_BRANCH_MASTER




/* *****************************************************************************
 *
 *        DEFINES, ENUMS, STRUCTURES
 *
 **************************************************************************** */
#ifndef MAX_INPUT_NEURONS
  #define MAX_INPUT_NEURONS                       3
#endif

#ifndef MAX_HIDDEN_NEURONS
  #define MAX_HIDDEN_NEURONS                      12
#endif

#ifndef MAX_OUTPUT_NEURONS
  #define MAX_OUTPUT_NEURONS                      2
#endif


/* List of valid transference function to a layer from a MLP neural network   */
typedef enum nNetTransfFcn_e
  {
  disablelayer,
  purelin,
  tansig,
  logsig,
  fcnamount,
  } nNetTransfFcn_t;


/* List of possible states to a neural network structure                      */
typedef enum nNetInitialization_e
  {
  _______________________ = 0x00,
  __________________Outpt = 0x01,
  ____________Hidd2______ = 0x02,
  ____________Hidd2_Outpt = 0x03,
  ______Hidd1____________ = 0x04,
  ______Hidd1_______Outpt = 0x05,
  ______Hidd1_Hidd2______ = 0x06,
  ______Hidd1_Hidd2_Outpt = 0x07,
  Input__________________ = 0x08,
  Input_____________Outpt = 0x09,
  Input_______Hidd2______ = 0x0A,
  Input_______Hidd2_Outpt = 0x0B,
  Input_Hidd1____________ = 0x0C,
  Input_Hidd1_______Outpt = 0x0D,
  Input_Hidd1_Hidd2______ = 0x0E,
  Input_Hidd1_Hidd2_Outpt = 0x0F,
  } nNetInitialization_t;


/* Parameters to define any generic layer of a MLP neural network             */
typedef struct nNetGenLayer_s
  {
  float* WeightMat;
  float* BiasArr;
  nNetTransfFcn_t TransfFcn;
  } nNetLayerMLP_t;


/* Parameters of network data array, either input and output                  */
typedef struct nNetData_s
  {
  float* ArrayAddr;
  uint16 ArrayLen;
  } nNetDataMLP_t;


/* Configuration structure to be used with the Damping algorithm              */
typedef struct nNetMLP_s
  {
  uint16 InputDataLen;

  nNetLayerMLP_t Hidden1Layer;
  nNetDataMLP_t Hidden1Data;

  nNetLayerMLP_t Hidden2Layer;
  nNetDataMLP_t Hidden2Data;

  nNetLayerMLP_t OutputLayer;
  uint16 OutputDataLen;

  bool isRunning;
  nNetInitialization_t Initialized;
  } nNetMLP_t;




/* -----------------------------------------------------------------------------
MLP_CONFIG_INPUT()
        Macro used to initialize the input layer of the neural network, it
        should be executed only once.
--------------------------------------------------------------------------------
Input:  Net
          Name of the neural network that will have the input layer initialized
        Len
          Amount of inputs of the neural network.
----------------------------------------------------------------------------- */
#define MLP_CONFIG_INPUT(Net,Len)                                               \
    Net.InputDataLen = Len,                                                     \
    Net.isRunning = FALSE,                                                      \
    Net.Initialized |= Input__________________;


/* -----------------------------------------------------------------------------
MLP_CONFIG_HIDDEN1()
        Macro used to initialize the first hidden layer of the neural network,
        as the macro above, it should be executed only once.
--------------------------------------------------------------------------------
Input:  Net
          Name of the neural network that will have the first hidden layer
          initialized
        WeiMat
          Name of the float data array from which the address will be used as
          pointer to the weight of the first hidden layer of network.
        BiaArr
          Name of the float data array from which the address will be used as
          pointer to the bias of the first hidden layer of network.
        TrF
          Transfer function of the neurons of the first hidden layer.
        Neurons
          Amount of neurons of the first hidden layer.
----------------------------------------------------------------------------- */
#define MLP_CONFIG_HIDDEN1(Net,WeiMat,BiaArr,TrF,Neurons)                       \
    Net.Hidden1Layer.WeightMat = (float *)&WeiMat,                              \
    Net.Hidden1Layer.BiasArr = (float *)&BiaArr,                                \
    Net.Hidden1Layer.TransfFcn = TrF,                                           \
    Net.Hidden1Data.ArrayAddr = NULL,                                           \
    Net.Hidden1Data.ArrayLen = Neurons,                                         \
    Net.isRunning = FALSE,                                                      \
    Net.Initialized |= ______Hidd1____________;


/* -----------------------------------------------------------------------------
MLP_CONFIG_HIDDEN2()
        Macro used to initialize the second hidden layer of the neural network,
        as the macro above, it should be executed only once.
--------------------------------------------------------------------------------
Input:  Net
          Name of the neural network that will have the second hidden layer
          initialized
        WeiMat
          Name of the float data array from which the address will be used as
          pointer to the weight of the second hidden layer of network.
        BiaArr
          Name of the float data array from which the address will be used as
          pointer to the bias of the second hidden layer of network.
        TrF
          Transfer function of the neurons of the second hidden layer.
        Neurons
          Amount of neurons of the second hidden layer.
----------------------------------------------------------------------------- */
#define MLP_CONFIG_HIDDEN2(Net,WeiMat,BiaArr,TrF,Neurons)                       \
    Net.Hidden2Layer.WeightMat = (float *)&WeiMat,                              \
    Net.Hidden2Layer.BiasArr = (float *)&BiaArr,                                \
    Net.Hidden2Layer.TransfFcn = TrF,                                           \
    Net.Hidden2Data.ArrayAddr = NULL,                                           \
    Net.Hidden2Data.ArrayLen = Neurons,                                         \
    Net.isRunning = FALSE,                                                      \
    Net.Initialized |= ____________Hidd2______;


/* -----------------------------------------------------------------------------
MLP_DISABLE_HIDDEN2()
        Macro used to disable the second hidden layer of the neural network,
        as the macro above, it should be executed only once.
--------------------------------------------------------------------------------
Input:  Net
          Name of the neural network that will have the second hidden layer
          disabled
----------------------------------------------------------------------------- */
#define MLP_DISABLE_HIDDEN2(Net)                                                \
    Net.Hidden2Layer.WeightMat = (float *)NULL,                                 \
    Net.Hidden2Layer.BiasArr = (float *)NULL,                                   \
    Net.Hidden2Layer.TransfFcn = disablelayer,                                  \
    Net.Hidden2Data.ArrayAddr = NULL,                                           \
    Net.Hidden2Data.ArrayLen = 0,                                               \
    Net.isRunning = FALSE,                                                      \
    Net.Initialized |= ____________Hidd2______;


/* -----------------------------------------------------------------------------
MLP_CONFIG_OUTPUT()
        Macro used to initialize the output layer of the neural network, as the
        macro above, it should be executed only once.
--------------------------------------------------------------------------------
Input:  Net
          Name of the neural network that will have the output layer
          initialized
        WeiMat
          Name of the float data array from which the address will be used as
          pointer to the weight of the output layer of network.
        BiaArr
          Name of the float data array from which the address will be used as
          pointer to the bias of the output layer of network.
        TrF
          Transfer function of the neurons of the output layer.
        Neurons
          Amount of neurons of the output layer.
----------------------------------------------------------------------------- */
#define MLP_CONFIG_OUTPUT(Net,WeiMat,BiaArr,TrF,Neurons)                        \
    Net.OutputLayer.WeightMat = (float *)&WeiMat,                               \
    Net.OutputLayer.BiasArr = (float *)&BiaArr,                                 \
    Net.OutputLayer.TransfFcn = TrF,                                            \
    Net.OutputDataLen = Neurons,                                                \
    Net.isRunning = FALSE,                                                      \
    Net.Initialized |= __________________Outpt;




/* *****************************************************************************
 *
 *        PROTOTYPES
 *
***************************************************************************** */
  ReturnCode_t RunNetworkMLP(float *NetInput, nNetMLP_t *Net, float *NetOutput);




#endif /* NEURALNET_H_INCLUDED */
