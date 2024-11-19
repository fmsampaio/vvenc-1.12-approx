#include "CommonDef.h"
#include "Unit.h"
#include "approx.h"


namespace vvenc {

#define ENABLE_ORIG_SB_APPROX 1
#define ENABLE_NEIGH_SB_APPROX 1

#define ORIG_SB_CONFIG 1
#define ORIG_SB_BUFFER_Y 1
#define ORIG_SB_BUFFER_CB 2
#define ORIG_SB_BUFFER_CR 3

#define NEIGH_SB_CONFIG 2
#define NEIGH_SB_BUFFER_Y 4
#define NEIGH_SB_BUFFER_CB 5
#define NEIGH_SB_BUFFER_CR 6

class ApproxHandler {
    public:
        static Pel* approxIntraOrigBufferY;
        static Pel* approxIntraOrigBufferCb;
        static Pel* approxIntraOrigBufferCr;
        static const Pel* bkpIntraOrigBufferY;
        static const Pel* bkpIntraOrigBufferCb;
        static const Pel* bkpIntraOrigBufferCr;

        static void addApproxIntraOrigSB();
        static void removeApproxIntraOrigSB();

        static Pel* initIntraOrigSB(CPelBuf origBuffer, ComponentID comp);
        static const Pel* restoreIntraOrigSB(ComponentID comp);

        // static void addApproxIntraNeighSB(Pel* neighY, Pel* neighCb, Pel* neighCr);
        // static void removeApproxIntraNeighSB(Pel* neighY, Pel* neighCb, Pel* neighCr);

        static void addApproxIntraNeighSB(Pel* refBuffer, ComponentID comp);
        static void removeApproxIntraNeighSB(Pel* refBuffer);
        
        static void startGlobalLevel();
        static void endGlobalLevel();

};


}