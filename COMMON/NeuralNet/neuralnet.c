/* *****************************************************************************
 FILE_NAME:     NeuralNet.c
 DESCRIPTION:   Software library with neural network implementations
 DESIGNER:      Juliano Varasquim
 CREATION_DATE: 19/jul/2016
 VERSION:       2.1
********************************************************************************
Version 1.0:    19/jul/2016 - Juliano Varasquim
                - First version of the source code
Version 2.0:    17/nov/2016 - Juliano Varasquim
                - Changing of the format of the neural net processing function,
                  since this revision, the input data of the neural network and
                  the output pointer are passed as parameter to the function
Version 2.1:    18/nov/2016 - Juliano Varasquim
                - Correction at comment lines only, the source code were not
                  changed
***************************************************************************** */




/* *****************************************************************************
 *
 *        INCLUDES (and DEFINES for INCLUDES)
 *
***************************************************************************** */
#include "NeuralNet.h"
#include <math.h>




/* *****************************************************************************
 *
 *        DEFINES, ENUMS, STRUCTURES
 *
 **************************************************************************** */
  typedef float (*TransFncPtr)(float);




/* *****************************************************************************
 *
 *        LOCAL PROTOTYPES
 *
***************************************************************************** */
float purelinear(float in);
float logsigmoid(float n);
ReturnCode_t ProcessLayerMLP(nNetDataMLP_t *Input, nNetLayerMLP_t *Layer, nNetDataMLP_t *Output);




/* *****************************************************************************
 *
 *        FUNCTIONS AREA
 *
***************************************************************************** */




/* -----------------------------------------------------------------------------
RunNetworkMLP()
        Runs a multilayer perceptron neural network, which has up to three
        layer, being two hidden layers and one output layer. The implementation
        of the first hidden layer is mandatory to the the proper functioning of
        the function, differently to the second hidden layer, in which its
        implementation is optional.
        The output provided is the result of the input data processed by the
        neural network.
--------------------------------------------------------------------------------
Input:  NetInput
          Pointer to the data array which will be used as input to the neural
          network
        Net
          Pointer to the structure which has all the parameters required to the
          functioning of a multilayer perceptron neural network.
          Since, the neural network structure is too large and its comprehension
          requires good skill level at neural network implementations, it was
          decided to create some macros that are used to initialize the
          structure step by step, making this task easier to the user.
          So, it is strongly recommended the using of the macros listed below
          to "build" the neural network, since they are able to perform all the
          actions required to the network initialization, receiving as
          parameters the minimum data.
        NetOutput
          Pointer to the data array which will be used as output to the neural
          network

Macros:   MLP_CONFIG_INPUT(Net,Len)
          MLP_CONFIG_HIDDEN1(Net,WeiMat,BiaArr,TrF,Neurons)
          MLP_CONFIG_HIDDEN2(Net,WeiMat,BiaArr,TrF,Neurons)
          MLP_DISABLE_HIDDEN2(Net)
          MLP_CONFIG_OUTPUT(Net,WeiMat,BiaArr,TrF,Neurons)

Return: ERR_PARAM_INIT
          The network parameters are still not initialized
        ERR_PARAM_LENGTH
          The length of the some data array is not valid
        ERR_PARAM_ADDRESS
          The pointer to some data array was not initialized
        ERR_PARAM_ATTRIBUTE_SET
          The set transfer function of some layer of the network is invalid
        ANSWERED_REQUEST
          The operation has been successfully performed
----------------------------------------------------------------------------- */
ReturnCode_t RunNetworkMLP(float *NetInput, nNetMLP_t *Net, float *NetOutput)
  {
  ReturnCode_t  ReturnValue;
  static float Hidden1Layer[MAX_HIDDEN_NEURONS];
  static float Hidden2Layer[MAX_HIDDEN_NEURONS];
  static nNetDataMLP_t TmpData;
  bool Using2ndHiddenLayer = FALSE;

  /* ------------------------------------------------------------------------ */
  /* Check if the network was not fully initialized, at this case leaves the  */
  /* processing and returns an error, otherwise follows processing the data   */
  /* ------------------------------------------------------------------------ */
  if(Net->Initialized != Input_Hidd1_Hidd2_Outpt)
    {
    ReturnValue = ERR_PARAM_INIT;
    __DEBUGHALT();
    }
  /* ------------------------------------------------------------------------ */
  /* Net was fully initialized, so it is needed to check if the               */
  /* initialization values have coherence                                     */
  /* ------------------------------------------------------------------------ */
  else
    {
    /* Define the status of the second hidden layer                           */
    Using2ndHiddenLayer = (Net->Hidden2Data.ArrayLen > 0) ? TRUE : FALSE;

    /* If the network is not running, so performs a sanity check at its       */
    /* parameters                                                             */
    if(Net->isRunning == FALSE)
      {
      /* Check the sanity of the length of the data arrays                    */
      if( (Net->InputDataLen < 1)
        ||(Net->InputDataLen > MAX_INPUT_NEURONS)
        ||(Net->Hidden1Data.ArrayLen < 1)
        ||(Net->Hidden1Data.ArrayLen > MAX_HIDDEN_NEURONS)
        ||(Net->Hidden2Data.ArrayLen > MAX_HIDDEN_NEURONS)
        ||(Net->OutputDataLen < 1)
        ||(Net->OutputDataLen > MAX_OUTPUT_NEURONS) )
        {
        /* Network keeps not running and an error about the configured length */
        /* is returned                                                        */
        ReturnValue = ERR_PARAM_LENGTH;
        __DEBUGHALT();
        }
      /* Check the sanity of the memory address of the data arrays            */
      else if( (Net->Hidden1Layer.WeightMat == NULL)
        ||(Net->Hidden1Layer.BiasArr == NULL)
        ||( (Using2ndHiddenLayer) && (Net->Hidden2Layer.WeightMat == NULL) )
        ||( (Using2ndHiddenLayer) && (Net->Hidden2Layer.BiasArr == NULL) )
        ||(Net->OutputLayer.WeightMat == NULL)
        ||(Net->OutputLayer.BiasArr == NULL) )
        {
        /* Network keeps not running and an error about the address of the    */
        /* array is returned                                                  */
        ReturnValue = ERR_PARAM_ADDRESS;
        __DEBUGHALT();
        }
      /* Check the sanity of the memory address of the data arrays            */
      else if( (Net->Hidden1Layer.TransfFcn == disablelayer)
        ||(Net->Hidden1Layer.TransfFcn >= fcnamount)
        ||( (Using2ndHiddenLayer) && (Net->Hidden2Layer.TransfFcn == disablelayer) )
        ||( (Using2ndHiddenLayer) && (Net->Hidden2Layer.TransfFcn >= fcnamount) )
        ||(Net->OutputLayer.TransfFcn == disablelayer)
        ||(Net->OutputLayer.TransfFcn >= fcnamount) )
        {
        /* Network keeps not running and an error about the configured        */
        /* transfer function is returned                                      */
        ReturnValue = ERR_PARAM_ATTRIBUTE_SET;
        __DEBUGHALT();
        }
      /* All the parameters have been approved at the sanity check            */
      else
        {
        /* Performs automatic set of the address of the array used to store   */
        /* the hidden layers result                                           */
        if(Net->Hidden1Data.ArrayAddr == NULL)
          {
          Net->Hidden1Data.ArrayAddr = &Hidden1Layer[0];
          }
        if( Using2ndHiddenLayer && (Net->Hidden2Data.ArrayAddr == NULL) )
          {
          Net->Hidden2Data.ArrayAddr = &Hidden2Layer[0];
          }

        /* Enables the network to run and returns a success answer            */
        Net->isRunning = TRUE;
        ReturnValue = ANSWERED_REQUEST;
        }
      }


    /* If the running flag is true, process the data through the neural       */
    /* network                                                                */
    if(Net->isRunning)
      {
      TmpData.ArrayAddr = NetInput;
      TmpData.ArrayLen = Net->InputDataLen;
      (void)ProcessLayerMLP( &TmpData, &Net->Hidden1Layer, &Net->Hidden1Data);
      TmpData.ArrayAddr = NetOutput;
      TmpData.ArrayLen = Net->OutputDataLen;

      /* Follow with the network processing depending to the amount of hidden */
      /* layers used                                                          */
      if(Using2ndHiddenLayer)
        {
        (void)ProcessLayerMLP( &Net->Hidden1Data, &Net->Hidden2Layer, &Net->Hidden2Data);
        (void)ProcessLayerMLP( &Net->Hidden2Data, &Net->OutputLayer, &TmpData);
        }
      else
        {
        (void)ProcessLayerMLP( &Net->Hidden1Data, &Net->OutputLayer, &TmpData);
        }

      }
    }

  /* Job done. */
  return ReturnValue;
  }




/* *****************************************************************************
 *
 *        LOCAL FUNCTIONS AREA
 *
***************************************************************************** */




/* -----------------------------------------------------------------------------
ProcessLayerMLP()
        Process a layer from a Multi Layer Perceptron network and returns the
        processed value.
--------------------------------------------------------------------------------
Input:  Input.ArrayAddr
          Pointer to the float array (i.e. an one dimension matrix) with the
          input data. It MUST have the length of "Input.ArrayLen", which may be
          represented mathematically as:
              [ i1  i2  ... iX  ]     | where X = Input.ArrayLen
        Input.ArrayLen
          Integer value which indicates the amount of inputs that will be
          processed.
        Layer.WeightMat
          Pointer to the float matrix with the weight data. It MUST have the
          amount of rows equal to "Input.ArrayLen", as well as it MUST have the
          amount of columns equal to "Output.ArrayLen", which may be represented
          mathematically as:
              [ w11 w12 ... w1Y ]
              [ w21 w22 ... w2Y ]     | where X = Input.ArrayLen
              [ ... ... ... ... ]             Y = Output.ArrayLen
              [ wX1 wX2 ... wXY ]
        Layer.BiasArr
          Pointer to the float array (i.e. an one dimension matrix) with the
          bias data. It MUST have the length of "Output.ArrayLen", which may be
          represented mathematically as:
              [ b1  b2  ... bY  ]     | where Y = Output.ArrayLen
        Layer.TransfFcn
          Configures the function activation used by the perceptron where the
          options below are available:
            - disablelayer
            - purelin
            - tansig
            - logsig
        Output.ArrayLen
          Integer value which indicates the amount of outputs that will be
          processed.

Output: OutputArray
          Pointer to the float array (i.e. an one dimension matrix) with the
          output data. It MUST have the length of "Layer.OuputLen", which may be
          represented mathematically as:
              [ o1  o2  ... oY  ]     | where Y = Layer.OutputLen

Return: ANSWERED_REQUEST
          The operation has been successfully performed
----------------------------------------------------------------------------- */
ReturnCode_t ProcessLayerMLP(nNetDataMLP_t *Input, nNetLayerMLP_t *Layer, nNetDataMLP_t *Output)
  {
  uint32 mRow,mCol,MaxRow,MaxCol, MemOffset;
  float Weight, NeuroIn;
  TransFncPtr TransFunction[fcnamount] =
    {
    NULL,                               /* disablelayer                       */
    purelinear,                         /* purelin                            */
    tanhf,                              /* tansig                             */
    logsigmoid,                         /* logsig                             */
    };


  /* Define the quantity of rows and columns that the weight matrix has       */
  MaxRow = Input->ArrayLen;
  MaxCol = Output->ArrayLen;


  /* Calculates the output value of each neuron of the network.             */
  for(mCol = 0;  mCol < MaxCol;  mCol++)
    {
    /* Forces the initial value of the auxiliary value as zero              */
    NeuroIn = 0.0F;

    /* Calculates the product among each input and its corresponding weight */
    for(mRow = 0;  mRow < MaxRow;  mRow++)
      {
      /* Calculates offset among the memory address pointed by the pointer    */
      /* and the address of the memory position pointed by the indexes mRow   */
      /* and mColumn.                                                         */
      MemOffset = mRow*MaxCol + mCol;

      /* Get the weight which will be used at the calculation                 */
      Weight = *(Layer->WeightMat + MemOffset);

      /* Updates the value used as input to the nth neuron of the Layer       */
      NeuroIn += Input->ArrayAddr[mRow] * Weight;
      }

    /* Add the bias portion of the input of the nth neuron of the layer       */
    NeuroIn += Layer->BiasArr[mCol];

    /* Calculates the output of the nth neuron of the layer                   */

    Output->ArrayAddr[mCol] = (*TransFunction[Layer->TransfFcn])(NeuroIn);
    }


  /* Return a success answer                                                  */
  return ANSWERED_REQUEST;
  }




/* -----------------------------------------------------------------------------
purelinear()
        Only returns the same value received as parameter without any kind of
        additional processing.
--------------------------------------------------------------------------------
Input:  n
          Float value used as input

Output: NA

Return:
        The same value received as parameter
----------------------------------------------------------------------------- */
float purelinear(float n)
  {
  return n;
  }




/* -----------------------------------------------------------------------------
logsigmoid()
        Calculates the log-sigmoid of the input value
--------------------------------------------------------------------------------
Input:  in
          Float value used as input

Output: NA

Return:
        logsig(n)
----------------------------------------------------------------------------- */
float logsigmoid(float n)
  {
  return  1 / (1 + expf(-n));
  }
