#include "ApproxHandler.h"

namespace vvenc {

Pel* ApproxHandler::approxIntraOrigBufferY;
Pel* ApproxHandler::approxIntraOrigBufferCb;
Pel* ApproxHandler::approxIntraOrigBufferCr;
const Pel* ApproxHandler::bkpIntraOrigBufferY;
const Pel* ApproxHandler::bkpIntraOrigBufferCb;
const Pel* ApproxHandler::bkpIntraOrigBufferCr;

std::vector<int> ApproxHandler::dynApproxCfgs;
FILE* ApproxHandler::dynApproxCfgFile;

int FRAME_LEVEL_RA_GOP32[33] = {0, 5, 4, 5, 3, 5, 4, 5, 2, 5, 4, 5, 3, 5, 4, 5, 1, 5, 4, 5, 3, 5, 4, 5, 2, 5, 4, 5, 3, 5, 4, 5, 0};
int FRAME_LEVEL_RA_GOP16[17] = {0, 4, 3, 4, 2, 4, 3, 4, 1, 4, 3, 4, 2, 4, 3, 4, 0};


void ApproxHandler::allocIntraOrigSB() {
    approxIntraOrigBufferY = xMalloc(Pel, 128 * 128);
    approxIntraOrigBufferCb = xMalloc(Pel, 64 * 64);
    approxIntraOrigBufferCr = xMalloc(Pel, 64 * 64);
}

 /*********************************************
  * STATIC APPROXIMATION
  *********************************************/
void ApproxHandler::addApproxIntraOrigSB() {
    Pel* beginBufferY = approxIntraOrigBufferY;
    Pel* endBufferY = beginBufferY + (128 * 128);

    Pel* beginBufferCb = approxIntraOrigBufferCb;
    Pel* endBufferCb = beginBufferCb + (64 * 64);

    Pel* beginBufferCr = approxIntraOrigBufferCr;
    Pel* endBufferCr = beginBufferCr + (64 * 64);

    ApproxSS::add_approx((void *) beginBufferY, (void *) endBufferY, ORIG_SB_BUFFER_Y, ORIG_SB_CONFIG, sizeof(const Pel)); //Luma (1); Cb (2); Cr (3)
    ApproxSS::add_approx((void *) beginBufferCb, (void *) endBufferCb, ORIG_SB_BUFFER_CB, ORIG_SB_CONFIG, sizeof(const Pel)); //Luma (1); Cb (2); Cr (3)
    ApproxSS::add_approx((void *) beginBufferCr, (void *) endBufferCr, ORIG_SB_BUFFER_CR, ORIG_SB_CONFIG, sizeof(const Pel)); //Luma (1); Cb (2); Cr (3)
}

/*********************************************
  * DYNAMIC APPROXIMATION
  *********************************************/
void ApproxHandler::addApproxIntraOrigSB(int framePoc) {
  Pel* beginBufferY = approxIntraOrigBufferY;
  Pel* endBufferY = beginBufferY + (128 * 128);

  Pel* beginBufferCb = approxIntraOrigBufferCb;
  Pel* endBufferCb = beginBufferCb + (64 * 64);

  Pel* beginBufferCr = approxIntraOrigBufferCr;
  Pel* endBufferCr = beginBufferCr + (64 * 64);

  int approxLevel = dynApproxCfgs[FRAME_LEVEL_RA_GOP32[framePoc]];

  ApproxSS::add_approx((void *) beginBufferY, (void *) endBufferY, ORIG_SB_BUFFER_Y, approxLevel, sizeof(const Pel)); //Luma (1); Cb (2); Cr (3)
  ApproxSS::add_approx((void *) beginBufferCb, (void *) endBufferCb, ORIG_SB_BUFFER_CB, approxLevel, sizeof(const Pel)); //Luma (1); Cb (2); Cr (3)
  ApproxSS::add_approx((void *) beginBufferCr, (void *) endBufferCr, ORIG_SB_BUFFER_CR, approxLevel, sizeof(const Pel)); //Luma (1); Cb (2); Cr (3)
}

void ApproxHandler::removeApproxIntraOrigSB() {
  Pel*    beginBufferY = approxIntraOrigBufferY;
  Pel* endBufferY = beginBufferY + (128 * 128);

  Pel* beginBufferCb = approxIntraOrigBufferCb;
  Pel* endBufferCb = beginBufferCb + (64 * 64);

  Pel* beginBufferCr = approxIntraOrigBufferCr;
  Pel* endBufferCr = beginBufferCr + (64 * 64);

  ApproxSS::remove_approx((void *) beginBufferY, (void *) endBufferY);
  ApproxSS::remove_approx((void *) beginBufferCb, (void *) endBufferCb);
  ApproxSS::remove_approx((void *) beginBufferCr, (void *) endBufferCr);
}

Pel* ApproxHandler::initIntraOrigSB(CPelBuf origBuffer, ComponentID comp) {
  int bufferStride = origBuffer.stride * origBuffer.height;

  if(comp == COMP_Y) {
    bkpIntraOrigBufferY = origBuffer.buf;
    for (size_t i = 0; i < bufferStride; i++) {
      approxIntraOrigBufferY[i]  = origBuffer.buf[i];
    }
    return approxIntraOrigBufferY;
  }

  else if(comp == COMP_Cb) {
    bkpIntraOrigBufferCb = origBuffer.buf;
    for (size_t i = 0; i < bufferStride; i++) {
      approxIntraOrigBufferCb[i] = origBuffer.buf[i];
    }
    return approxIntraOrigBufferCb;
  }

  else { //comp == COMP_Cr
    bkpIntraOrigBufferCr = origBuffer.buf;
    for (size_t i = 0; i < bufferStride; i++) {
      approxIntraOrigBufferCr[i] = origBuffer.buf[i];
    }
    return approxIntraOrigBufferCr;
  }
}

const Pel* ApproxHandler::restoreIntraOrigSB(ComponentID comp) {
    if(comp == COMP_Y) {
      return bkpIntraOrigBufferY;
    }
    else if(comp == COMP_Cb) {
      return bkpIntraOrigBufferCb;
    }
    else { //comp == COMP_Cr
      return bkpIntraOrigBufferCr;
    }
}

void ApproxHandler::addApproxIntraNeighSB(Pel* refBuffer, ComponentID comp) {
    // size: (MAX_CU_SIZE * 2 + 1 + MAX_REF_LINE_IDX) * 2

    int bufferStride = (MAX_CU_SIZE * 2 + 1 + MAX_REF_LINE_IDX) * 2 - 1;

    Pel* beginNeighBuffer = refBuffer;
    Pel* endNeighBuffer = beginNeighBuffer + bufferStride;

    if(comp == COMP_Y) {
        ApproxSS::add_approx((void *) beginNeighBuffer, (void *) endNeighBuffer, NEIGH_SB_BUFFER_Y, NEIGH_SB_CONFIG, sizeof(Pel));
    }
    else if(comp == COMP_Cb) {
        ApproxSS::add_approx((void *) beginNeighBuffer, (void *) endNeighBuffer, NEIGH_SB_BUFFER_CB, NEIGH_SB_CONFIG, sizeof(Pel));
    }
    else {
        ApproxSS::add_approx((void *) beginNeighBuffer, (void *) endNeighBuffer, NEIGH_SB_BUFFER_CR, NEIGH_SB_CONFIG, sizeof(Pel));
    }
}

void ApproxHandler::addApproxIntraNeighSB(Pel* refBuffer, ComponentID comp, int framePoc) {
    // size: (MAX_CU_SIZE * 2 + 1 + MAX_REF_LINE_IDX) * 2

    int bufferStride = (MAX_CU_SIZE * 2 + 1 + MAX_REF_LINE_IDX) * 2 - 1;

    Pel* beginNeighBuffer = refBuffer;
    Pel* endNeighBuffer = beginNeighBuffer + bufferStride;

    int approxLevel = dynApproxCfgs[FRAME_LEVEL_RA_GOP32[framePoc]];

    if(comp == COMP_Y) {
        ApproxSS::add_approx((void *) beginNeighBuffer, (void *) endNeighBuffer, NEIGH_SB_BUFFER_Y, approxLevel, sizeof(Pel));
    }
    else if(comp == COMP_Cb) {
        ApproxSS::add_approx((void *) beginNeighBuffer, (void *) endNeighBuffer, NEIGH_SB_BUFFER_CB, approxLevel, sizeof(Pel));
    }
    else {
        ApproxSS::add_approx((void *) beginNeighBuffer, (void *) endNeighBuffer, NEIGH_SB_BUFFER_CR, approxLevel, sizeof(Pel));
    }
}

void ApproxHandler::removeApproxIntraNeighSB(Pel* refBuffer) {
    int bufferStride = (MAX_CU_SIZE * 2 + 1 + MAX_REF_LINE_IDX) * 2 - 1;

    Pel* beginNeighBuffer = refBuffer;
    Pel* endNeighBuffer = beginNeighBuffer + bufferStride;

    ApproxSS::remove_approx((void *) beginNeighBuffer, (void *) endNeighBuffer);
}

void ApproxHandler::startGlobalLevel() {
    ApproxSS::start_level();
}

void ApproxHandler::endGlobalLevel() {
    ApproxSS::end_level();
}

void ApproxHandler::initDynApprox(const char fileName[]) {
//void ApproxHandler::initDynApprox() {
  for (int i = 0; i < NUM_RA_FRAME_LEVELS; i++) { 
    dynApproxCfgs.push_back(SRAM_LOSSLESS);
  }  

  // dynApproxCfgFile = fopen(fileName.c_str(), "r");
  dynApproxCfgFile = fopen(fileName, "r");

  int frameLevel = -1;
  int approxLevel = -1;

  while(fscanf(dynApproxCfgFile, "%d;%d\n", &frameLevel, &approxLevel) != EOF) {
    dynApproxCfgs[frameLevel] = approxLevel;
  }

  std::cout << "\n|nDYNAMIC APPROX LEVELS\n";
  for (int i = 0; i < NUM_RA_FRAME_LEVELS; i++) { 
    std::cout << i << " : " << dynApproxCfgs[i] << std::endl;
  } 
  
}



}