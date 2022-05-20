/* *****************************************************************************
 FILE_NAME:     ZeroCross.c
 DESCRIPTION:   Software library for zero crossing detection
 DESIGNER:      Felipe Zanoni
 CREATION_DATE: 27/sep/2018
 VERSION:       1.0
 ********************************************************************************
Version 1.0:    27/Sep/2018 - Felipe Zanoni
                - First version of the source code
 ***************************************************************************** */


/* *****************************************************************************
 *
 *        INCLUDES (and DEFINES for INCLUDES)
 *
 ***************************************************************************** */

#include "ZeroCross.h"
#include "math.h"

/* *****************************************************************************
 *
 *        INTERNAL TYPES DEFINITION AREA
 *
 ***************************************************************************** */

/**
 * Signal zero crossing state
 */
typedef enum
{
  ZC_SIG_NO = 0, //!< No zero crossing detected
  ZC_SIG_POS,    //!< Signal is in positive half
  ZC_SIG_NEG     //!< Signal is in negative half
} ZeroCross_Signal_State;

/**
 * Zero crossing algorithm state
 */
typedef enum
{
  CY_WARM_UP_1 = 0, //!< Warming up the algorithm stage one
  CY_WARM_UP_2,     //!< Warming up the algorithm stage two
  CY_CALC           //!< Algorithm processing
} ZeroCross_Cycle;

/**
 * Struct to hold sample index and an inter-sample offset
 */
typedef struct
{
  uint32_t i;   //!< Whole samples after the zero crossing has happened */
  float    di;  //!< Zero crossing happened this fraction before sample detected */
}
ZeroCross_Pos;

/**
 * struct to hold tracking information for each signal
 */
typedef struct
{
  ZeroCross_Signal_State state; //!< internal record ZC state
  ZeroCross_Cycle cycle_state;  //!< for period and frequency calc
  ZeroCross_Pos zc[3];          //!< last zero crossing position, and previous in each direction
  float sig[4];                 //!< last sample, min & max storage and previous value for quadratic peak fit
  int peaksearch;               //!< Flag set when should calculate the peak
  float ampl;                   //!< Signal amplitude
  float Ti[3];                  //!<
}
ZeroCross_Track_State;

/** A set of variables needed to track a pair of sinusoidal signals */
typedef struct
{
  ZeroCross_Track_State s1_par; //!< Tracking state for signal S1
  ZeroCross_Track_State s2_par; //!< Tracking state for signal S2
  ZeroCross_Track_State ws_par; //!< Tracking state for signal WS
  float ws_period_samples;      //!< Number of samples for 1 total period (may be a fraction)
  float t1p;                    //!< Last positive zero cross time difference between s1 and ws
  float t2p;                    //!< Last positive zero cross time difference between s2 and ws
  float t1n;                    //!< Last negative zero cross time difference between s1 and ws
  float t2n;                    //!< Last negative zero cross time difference between s2 and ws
  float time_diff[3];           //!< Time diff for LAST, SIG_POS and SIG_NEG
  float delta_i;                //!<
} ZeroCross_SigTrack;

/** Index for Last Signal value (.sig, .time_diff, .zc, .Ti) */
#define LAST 0

/** Index for Previous Signal value (.sig) */
#define PREV 3

/* 10Hz @5kHz   */
#define FREQ_CALC_MAX_CYCLE (19200.0F) // (192000.0F/10.0F)

/* 2000Hz @192kHz */
#define FREQ_CALC_MIN_CYCLE (2.5F) // (5000.0F/2000.0F)

/** Zero crossing signal tracking data */
static ZeroCross_SigTrack Zero_Cross_Data;

/** Sampling frequency */
static float Sample_Freq;



static inline ZeroCross_Signal_State ZC_track  (const float sig, ZeroCross_Track_State * const p);

static inline ZeroCross_Signal_State ZC_period (const float sig, ZeroCross_Track_State * const p);

static inline float quadraticpeakfit(float y1, float y2, float y3);

static inline ZeroCross_Signal_State step (const float s1, const float s2, const float ws);

/* *****************************************************************************
 *
 *        FUNCTIONS AREA
 *
 ***************************************************************************** */

/* -----------------------------------------------------------------------------
ZeroCross_Handler()
        Run the zero crossing detection algorithm
--------------------------------------------------------------------------------
Input:  s1
          Float vector of signal s1 pre-filtered.
        s2
          Float vector of signal s2 pre-filtered.
        ws
          Float vector of signal s1 and s2 weighted sum.
        samples
          Float variable indicating vector size for s1, s2 and ws.
          All signals must have same length.

Output: results
          Pointer to the variable that will receive all the data results from
           the zero crossing detection algorithm. At most 3 zero crossing will
           be returned, but usually it is only one and rarely.
           The zero crossing data will be valid for the last 3 detections, but the
           counter can be more than this.
----------------------------------------------------------------------------- */
void ZeroCross__Run(const float s1[],
                            const float s2[],
                            const float ws[],
                            uint16_t samples,
                            ZeroCross_Results *results)
{
  // Counter for zero crossing detection
  int zcIdx = 0;

  // Initialize output variables
  memset(results, 0x00, sizeof(ZeroCross_Results));

  // For each sample do
  for (int i = 0; i < samples; i++) {

    // Process each sample
    ZeroCross_Signal_State zc = step(s1[i], s2[i], ws[i]);

    // Have we got a zero crossing event?
    if (zc != ZC_SIG_NO)
    {
      // Is it a valid one?
      if (Zero_Cross_Data.ws_period_samples > 1.0F)
      {
        // Get data from crossing
        if (zcIdx > 2)
        {
          memcpy(&results->data[0], &results->data[1], sizeof(ZeroCross_Result_Detail));
          memcpy(&results->data[1], &results->data[2], sizeof(ZeroCross_Result_Detail));

          zcIdx=2;
        }
        results->data[zcIdx].timeDiff = Zero_Cross_Data.time_diff[LAST] / Sample_Freq;
        results->data[zcIdx].phaseDiff = Zero_Cross_Data.time_diff[LAST] / Zero_Cross_Data.ws_period_samples * 360.0F;
        results->data[zcIdx].freq = Sample_Freq / Zero_Cross_Data.ws_period_samples;
        results->data[zcIdx].sampleIdx = i;
        results->data[zcIdx].ampWeiSum = Zero_Cross_Data.ws_par.ampl;
        results->data[zcIdx].ampSign01 = Zero_Cross_Data.s1_par.ampl;
        results->data[zcIdx].ampSign02 = Zero_Cross_Data.s2_par.ampl;
        results->zcFoundCnt++;
        zcIdx++;
      }
    }
  }
}

/* -----------------------------------------------------------------------------
ZeroCross__Init()
        Initialize all zero crossing detection altorithm variables
--------------------------------------------------------------------------------
  Input:  freq
          Sampling rate frequency in Hz
----------------------------------------------------------------------------- */
void ZeroCross__Init(const float freq)
{
  memset(&Zero_Cross_Data, 0x00, sizeof(ZeroCross_SigTrack));
  Zero_Cross_Data.ws_period_samples = 1.0F; // to prevent Div/0 on first use elsewhere

  Sample_Freq = freq;
} 

/* -----------------------------------------------------------------------------
step()
        Runs the altorithm for one sample of each signal
--------------------------------------------------------------------------------
  Input:  s1
          One sample from signal s1
          s2
          One sample from signal s2
          ws
          One sample from signal ws

  Return: The ZeroCross_Signal_State for the signal ws
----------------------------------------------------------------------------- */
static inline ZeroCross_Signal_State step (const float s1, const float s2, const float ws)
{
  /* check each signal for zero crossings don't bother with frequency of individual sensors, for now */
  ZeroCross_Signal_State zc_s1 = ZC_track (s1, &Zero_Cross_Data.s1_par);
  ZeroCross_Signal_State zc_s2 = ZC_track (s2, &Zero_Cross_Data.s2_par);

  /* Get period and frequency from weighted SUM */
  ZeroCross_Signal_State zc_ws = ZC_period (ws, &Zero_Cross_Data.ws_par);

  if (zc_ws) /* just had a second crossing on WS */
  {
    if (zc_ws == ZC_SIG_POS)
    {
      if (Zero_Cross_Data.s1_par.state == ZC_SIG_POS) // already had s1 crossing
      {
        Zero_Cross_Data.t1p = (float)Zero_Cross_Data.s1_par.zc[ZC_SIG_POS].i
            + Zero_Cross_Data.s1_par.zc[LAST].di
            - Zero_Cross_Data.ws_par.zc[LAST].di;
      }

      if (Zero_Cross_Data.s2_par.state == ZC_SIG_POS)
      {
        Zero_Cross_Data.t2p = (float)Zero_Cross_Data.s2_par.zc[ZC_SIG_POS].i
            + Zero_Cross_Data.s2_par.zc[LAST].di
            - Zero_Cross_Data.ws_par.zc[LAST].di;
      }
    }

    if (zc_ws == ZC_SIG_NEG)
    {
      if (Zero_Cross_Data.s1_par.state == ZC_SIG_NEG)
      {
        Zero_Cross_Data.t1n = (float)Zero_Cross_Data.s1_par.zc[ZC_SIG_NEG].i
            + Zero_Cross_Data.s1_par.zc[LAST].di
            - Zero_Cross_Data.ws_par.zc[LAST].di;
      }

      if (Zero_Cross_Data.s2_par.state == ZC_SIG_NEG)
      {
        Zero_Cross_Data.t2n = (float)Zero_Cross_Data.s2_par.zc[ZC_SIG_NEG].i
            + Zero_Cross_Data.s2_par.zc[LAST].di
            - Zero_Cross_Data.ws_par.zc[LAST].di;
      }
    }
  }
  else  /* record negative phase if ZC occurs after the WS */
  {
    if (zc_s1)
    {
      if ( (zc_s1 == ZC_SIG_POS) && (Zero_Cross_Data.ws_par.state == ZC_SIG_POS) )
      {
        Zero_Cross_Data.t1p = Zero_Cross_Data.s1_par.zc[LAST].di
            - (float)Zero_Cross_Data.ws_par.zc[ZC_SIG_POS].i
            - Zero_Cross_Data.ws_par.zc[LAST].di;
      }

      if ( (zc_s1 == ZC_SIG_NEG) && (Zero_Cross_Data.ws_par.state == ZC_SIG_NEG) )
      {
        Zero_Cross_Data.t1n =  Zero_Cross_Data.s1_par.zc[LAST].di
            - (float)Zero_Cross_Data.ws_par.zc[ZC_SIG_NEG].i
            - Zero_Cross_Data.ws_par.zc[LAST].di;
      }
    }

    if (zc_s2)
    {
      if ( (zc_s2 == ZC_SIG_POS) && (Zero_Cross_Data.ws_par.state == ZC_SIG_POS) )
      {
        Zero_Cross_Data.t2p = Zero_Cross_Data.s2_par.zc[LAST].di
            - (float)Zero_Cross_Data.ws_par.zc[ZC_SIG_POS].i
            - Zero_Cross_Data.ws_par.zc[LAST].di;
      }

      if ( (zc_s2 == ZC_SIG_NEG) && (Zero_Cross_Data.ws_par.state == ZC_SIG_NEG) )
      {
        Zero_Cross_Data.t2n = Zero_Cross_Data.s2_par.zc[LAST].di
            - (float)Zero_Cross_Data.ws_par.zc[ZC_SIG_NEG].i
            - Zero_Cross_Data.ws_par.zc[LAST].di;
      }
    }
  }

  if (zc_ws)
  {
    Zero_Cross_Data.ws_period_samples = Zero_Cross_Data.ws_par.Ti[LAST];
    Zero_Cross_Data.delta_i = Zero_Cross_Data.ws_par.zc[LAST].di;
    Zero_Cross_Data.time_diff[zc_ws] = (Zero_Cross_Data.t1p + Zero_Cross_Data.t1n - Zero_Cross_Data.t2p - Zero_Cross_Data.t2n) * 0.25; // half average over last cycle
    Zero_Cross_Data.time_diff[LAST] = Zero_Cross_Data.time_diff[ZC_SIG_POS] + Zero_Cross_Data.time_diff[ZC_SIG_NEG]; // average of last 2 in each direction
  }

  return (zc_ws);
}

/* -----------------------------------------------------------------------------
quadraticpeakfit()
        Fits a quadratic to the y-values assuming uniformly spaced
        x coordinates are -1, 0, 1
        Fits y = a * x^2 + b * x + c
        Peak yv when 2 * a * xv + b = 0
--------------------------------------------------------------------------------
  Input:  y1
          Y value for X = -1
          y2
          Y value for X = 0
          y3
          Y value for X = 1

  Return: The yv calculated
----------------------------------------------------------------------------- */
inline float quadraticpeakfit(float y1, float y2, float y3)
{
  float c = y2;
  float b = (y3 - y1) * 0.5;
  float a = y3 - b - c;

  return c - b * b * 0.25 / a;
}

/* -----------------------------------------------------------------------------
ZC_track()
        Tracks a signal, identifying zero crossings using linear interpolation between
        sample points, and keeping track of the maximum amplitude in each half cycle.
--------------------------------------------------------------------------------
  Input:  sig
          Current signal
          p
          Stores the state

  Return: The zero crossing state for the signal
----------------------------------------------------------------------------- */
inline ZeroCross_Signal_State ZC_track (const float sig, ZeroCross_Track_State * const p)
{
  ZeroCross_Signal_State ZC_found = ZC_SIG_NO;

  switch (p->state)
  {
  default:
  case ZC_SIG_NO:
    p->sig[ZC_SIG_POS]  = p->sig[ZC_SIG_NEG]  = 0.0F;
    p->zc[ZC_SIG_POS].i = p->zc[ZC_SIG_NEG].i = 0;
    p->peaksearch = 0;

    if (sig > 0.0F)
      p->state = ZC_SIG_POS;
    else if (sig < 0.0F)
      p->state = ZC_SIG_NEG;
    /* else identically zero let's decide next update */

    break;

  case ZC_SIG_POS:
    if (sig > 0.0F)
    {
      if (sig > p->sig[ZC_SIG_POS])
      {
        p->sig[ZC_SIG_POS] = sig; /* assume well filtered, keep maximum for amplitude estimate */
        p->peaksearch = 1;
      }
      else if (p->peaksearch)
      {
        p->sig[ZC_SIG_POS] = quadraticpeakfit(p->sig[PREV], p->sig[LAST], sig);
        p->peaksearch = 0;
      }
    }
    else
    {
      ZC_found = ZC_SIG_NEG;
    }

    break;

  case ZC_SIG_NEG:
    if (sig < 0.0F)
    {
      if (sig < p->sig[ZC_SIG_NEG])
      {
        p->sig[ZC_SIG_NEG] = sig;
        p->peaksearch = 1;
      }
      else if (p->peaksearch)
      {
        p->sig[ZC_SIG_NEG] = quadraticpeakfit(p->sig[PREV], p->sig[LAST], sig);
        p->peaksearch = 0;
      }
    }
    else
    {
      ZC_found = ZC_SIG_POS;
    }

    break;
  }

  /* increment count samples since last ZC */
  p->zc[ZC_SIG_POS].i++;
  p->zc[ZC_SIG_NEG].i++;

  if (ZC_found)  /* either direction */
  {
    p->ampl = (p->sig[ZC_SIG_POS] - p->sig[ZC_SIG_NEG]) * 0.5F; /* half p-p ignoring noise spikes */
    p->peaksearch = 0;
    p->zc[LAST].i = p->zc[ZC_found].i;  // save current
    p->state = ZC_found;
    p->zc[ZC_found].i = 0;  // reset
    p->sig[ZC_found] = 0.0F; /* reset amp tracking */

    /* linear fit last 2 points */
    float den = sig - p->sig[LAST];

    /* delta index that ZC occurred before current sample */
    if (fabsf (den) > (1.0e-15F))
    {
      p->zc[LAST].di = sig / den;  /* how much earlier was the actual ZC */
    }
    else
    {
      p->zc[LAST].di = 0.0F;
    }
  }

  /* Local memory */
  p->sig[PREV] = p->sig[LAST];
  p->sig[LAST] = sig; // Save last value

  return (ZC_found);
}

/* -----------------------------------------------------------------------------
ZC_period()
        Computes the period between 2 zero crossings. The state variable pointed
        to by p is used for safe starting return a variable indicating an update,
        and the direction of zero crossing
--------------------------------------------------------------------------------
  Input:  sig
          Current signal
          p
          Stores the state

  Return: ZC_SIG_POS or ZC_SIG_NEG when a zero crossing is detected.
----------------------------------------------------------------------------- */
inline ZeroCross_Signal_State ZC_period (const float sig, ZeroCross_Track_State * const p)
{
  ZeroCross_Signal_State updated = ZC_SIG_NO;
  ZeroCross_Signal_State zc_type = ZC_track (sig, p);

  if (zc_type)
  {
    switch (p->cycle_state)
    {
    default:
    case CY_WARM_UP_1:
      p->cycle_state = CY_WARM_UP_2;
      break;

    case CY_WARM_UP_2:
      p->cycle_state = CY_CALC; /* assumes we now have seen a full half cycle */
      break;

    case CY_CALC:
    {
      /* distance before count of 0, less distance before current count */
      float Ti = (float)p->zc[LAST].i - p->zc[LAST].di + p->zc[zc_type].di;

      /* loose limits, calling routine should range check */
      if (   (Ti > FREQ_CALC_MIN_CYCLE)
          && (Ti < FREQ_CALC_MAX_CYCLE))
      {
        p->Ti[zc_type] = Ti;
        /* Average last 2 measurements, fixes an issue with value seen to be toggling
         * when amplitude changing in presense of a DC offset */
        p->Ti[LAST] = (p->Ti[ZC_SIG_POS] + p->Ti[ZC_SIG_NEG]) * 0.5; //was Ti;
        updated = zc_type;
      }
      else
      {
        p->Ti[LAST] = 1.0F; /* Safe default for division elsewhere... */
      }
    }

    break;
    }

    /* Save delta ZC of each type for next time */
    p->zc[zc_type].di = p->zc[LAST].di;
  }

  return (updated);
}

