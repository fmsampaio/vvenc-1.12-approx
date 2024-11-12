#include "CommonDef.h"
#include "Unit.h"
#include "approx.h"


namespace vvenc {

#define ENABLE_ORIG_SB_APPROX 0

#define ORIG_SB_CONFIG 1

#define ORIG_SB_BUFFER_Y 1
#define ORIG_SB_BUFFER_CB 2
#define ORIG_SB_BUFFER_CR 3

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
        
        static void startGlobalLevel();
        static void endGlobalLevel();


};


}