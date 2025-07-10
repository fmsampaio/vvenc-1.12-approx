#include "CommonDef.h"
#include "Unit.h"
#include "approx.h"

#include <vector>
#include <map>

namespace vvenc {

#define SRAM_LOSSLESS                           1
#define SRAM_ECC_APPROX_LOW_CONFIG              2            // SRAM 0.7V  ECC
#define SRAM_ECC_APPROX_MEDIUM_CONFIG           3            // SRAM 0.6V  ECC
#define SRAM_ECC_APPROX_HIGH_CONFIG             4            // SRAM 0.55V ECC

#define NUM_RA_FRAME_LEVELS                     6

#define APPROX_STRATEGY                         2            // 0 - Static; 1 - Dynamic; 2 - Dynamic (CU Level)

#define ENABLE_ORIG_SB_APPROX                   1         
#define ENABLE_NEIGH_SB_APPROX                  1


#define ORIG_SB_CONFIG                          1            // Static Approximation
#define ORIG_SB_BUFFER_Y                        1
#define ORIG_SB_BUFFER_CB                       2
#define ORIG_SB_BUFFER_CR                       3

#define NEIGH_SB_CONFIG                         2            // Static Approximation
#define NEIGH_SB_BUFFER_Y                       4
#define NEIGH_SB_BUFFER_Y_FILT                  5
#define NEIGH_SB_BUFFER_CB                      6
#define NEIGH_SB_BUFFER_CR                      7

#define ENABLE_INTRA_MAPS_REPORT                0

#define INTRA_MAP_RESOLUTION                    4
#define INTRA_MAP_EXPAND_FACTOR                 0


class ApproxHandler {
    public:
        static Pel* approxIntraOrigBufferY;
        static Pel* approxIntraOrigBufferCb;
        static Pel* approxIntraOrigBufferCr;
        static const Pel* bkpIntraOrigBufferY;
        static const Pel* bkpIntraOrigBufferCb;
        static const Pel* bkpIntraOrigBufferCr;

        static std::vector<int> dynApproxCfgs;
        static int cuLevelApproxLevel;
        static FILE* dynApproxCfgFile;      

        static int frameWidth, frameHeight, numOfFrames;
        static std::map<int, int*> intraMaps;  

        // BASELINE APPROXIMATION FUNCTIONS
        static void allocIntraOrigSB();
        static void addBaselineApproxIntraOrigSB(ComponentID comp, int approxLevel);
        static void addBaselineApproxIntraNeighSB(Pel* refBuffer, ComponentID comp, int filt, int approxLevel);
        static void removeApproxIntraOrigSB(ComponentID comp);
        static Pel* initIntraOrigSB(CPelBuf origBuffer, ComponentID comp);
        static const Pel* restoreIntraOrigSB(ComponentID comp);
        static void removeApproxIntraNeighSB(Pel* refBuffer);
        static void startGlobalLevel();
        static void endGlobalLevel();
        
        // STATIC APPROXIMATION FUNCTIONS
        static void addApproxIntraOrigSB(ComponentID comp);
        static void addApproxIntraNeighSB(Pel* refBuffer, ComponentID comp, int filt);

        // FRAME-LEVEL DYNAMIC APPROXIMATION FUNCTIONS
        static void initDynApprox(const char fileName[]);
        static void addApproxIntraOrigSB(ComponentID comp, int frameLevel);
        static void addApproxIntraNeighSB(Pel* refBuffer, ComponentID comp, int filt, int frameLevel);
        
        // CU-LEVEL DYNAMIC APPROXIMATION FUNCTIONS
        static void initCuLevelApprox(int width, int height, int nf);
        static void updateIntraMap(int framePoc, int xCU, int yCU, int wCU, int hCU);
        static void applyExpandFactor(int *xCU, int *yCU, int *wCU, int *hCU);
        static void reportIntraMap(int framePoc);
        static bool checkReferenceIsIntra(int framePoc, int xCU, int yCU, int wCU, int hCU);
        static void addApproxIntraNeighSB(Pel* refBuffer, ComponentID comp, int filt, int frameLevel, int framePoc, int xCU, int yCU, int wCU, int hCU);
        static void addApproxIntraOrigSB(ComponentID comp, int frameLevel, int framePoc, int xCU, int yCU, int wCU, int hCU);

};


}