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

std::map<int, int*> ApproxHandler::intraMaps;
int ApproxHandler::frameWidth, ApproxHandler::frameHeight, ApproxHandler::numOfFrames;

int ApproxHandler::cuLevelApproxLevel;

int FRAME_REFERENCES[33] = {-1, 2, 4, 2, 8, 4, 4, 6, 16, 10, 12, 10, 8, 14, 12, 14, -1, 18, 20, 18, 24, 22, 20, 22, 16, 26, 28, 26, 24, 30, 28, 30, -1 };

/*************************************
 * BASELINE APPROXIMATION FUNCTIONS
 *************************************/

void ApproxHandler::allocIntraOrigSB() {
    approxIntraOrigBufferY = xMalloc(Pel, 128 * 128);
    approxIntraOrigBufferCb = xMalloc(Pel, 64 * 64);
    approxIntraOrigBufferCr = xMalloc(Pel, 64 * 64);
}

void ApproxHandler::addBaselineApproxIntraOrigSB(ComponentID comp, int approxLevel) {  
  if(comp == COMP_Y) {
    Pel* beginBufferY = approxIntraOrigBufferY;
    Pel* endBufferY = beginBufferY + (128 * 128);
    ApproxSS::add_approx((void *) beginBufferY, (void *) endBufferY, ORIG_SB_BUFFER_Y, approxLevel, sizeof(const Pel)); //Luma (1); Cb (2); Cr (3)
  }
  else if(comp == COMP_Cb) {
    Pel* beginBufferCb = approxIntraOrigBufferCb;
    Pel* endBufferCb = beginBufferCb + (64 * 64);
    ApproxSS::add_approx((void *) beginBufferCb, (void *) endBufferCb, ORIG_SB_BUFFER_CB, approxLevel, sizeof(const Pel)); //Luma (1); Cb (2); Cr (3)

  }
  else { // comp == COMP_Cr
    Pel* beginBufferCr = approxIntraOrigBufferCr;
    Pel* endBufferCr = beginBufferCr + (64 * 64);
    ApproxSS::add_approx((void *) beginBufferCr, (void *) endBufferCr, ORIG_SB_BUFFER_CR, approxLevel, sizeof(const Pel)); //Luma (1); Cb (2); Cr (3)
  } 
}

void ApproxHandler::addBaselineApproxIntraNeighSB(Pel* refBuffer, ComponentID comp, int filt, int approxLevel) {
    // size: (MAX_CU_SIZE * 2 + 1 + MAX_REF_LINE_IDX) * 2
   int bufferStride = (MAX_CU_SIZE * 2 + 1 + MAX_REF_LINE_IDX) * 2 - 1;

    Pel* beginNeighBuffer = refBuffer;
    Pel* endNeighBuffer = beginNeighBuffer + bufferStride;

    if(comp == COMP_Y) {
      if(filt == 0) {
          ApproxSS::add_approx((void *) beginNeighBuffer, (void *) endNeighBuffer, NEIGH_SB_BUFFER_Y, approxLevel, sizeof(Pel));
      }
      else {
          ApproxSS::add_approx((void *) beginNeighBuffer, (void *) endNeighBuffer, NEIGH_SB_BUFFER_Y_FILT, approxLevel, sizeof(Pel));
      }
    }
    else if(comp == COMP_Cb) {
        ApproxSS::add_approx((void *) beginNeighBuffer, (void *) endNeighBuffer, NEIGH_SB_BUFFER_CB, approxLevel, sizeof(Pel));
    }
    else {
        ApproxSS::add_approx((void *) beginNeighBuffer, (void *) endNeighBuffer, NEIGH_SB_BUFFER_CR, approxLevel, sizeof(Pel));
    }
}

void ApproxHandler::removeApproxIntraOrigSB(ComponentID comp) {
  if(comp == COMP_Y) {
    Pel* beginBufferY = approxIntraOrigBufferY;
    Pel* endBufferY = beginBufferY + (128 * 128);
    ApproxSS::remove_approx((void *) beginBufferY, (void *) endBufferY);
  }
  else if(comp == COMP_Cb) {
    Pel* beginBufferCb = approxIntraOrigBufferCb;
    Pel* endBufferCb = beginBufferCb + (64 * 64);
    ApproxSS::remove_approx((void *) beginBufferCb, (void *) endBufferCb);
  }
  else { // comp == COMP_Cr
    Pel* beginBufferCr = approxIntraOrigBufferCr;
    Pel* endBufferCr = beginBufferCr + (64 * 64);
    ApproxSS::remove_approx((void *) beginBufferCr, (void *) endBufferCr);
  } 
}

void ApproxHandler::removeApproxIntraNeighSB(Pel* refBuffer) {
  int bufferStride = (MAX_CU_SIZE * 2 + 1 + MAX_REF_LINE_IDX) * 2 - 1;

  Pel* beginNeighBuffer = refBuffer;
  Pel* endNeighBuffer = beginNeighBuffer + bufferStride;

  ApproxSS::remove_approx((void *) beginNeighBuffer, (void *) endNeighBuffer);
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

void ApproxHandler::startGlobalLevel() {
  ApproxSS::start_level();
}

void ApproxHandler::endGlobalLevel() {
  ApproxSS::end_level();
}

/*************************************
 * STATIC APPROXIMATION FUNCTIONS
 *************************************/

void ApproxHandler::addApproxIntraOrigSB(ComponentID comp) {  
  addBaselineApproxIntraOrigSB(comp, ORIG_SB_CONFIG);
}

void ApproxHandler::addApproxIntraNeighSB(Pel* refBuffer, ComponentID comp, int filt) {
  addBaselineApproxIntraNeighSB(refBuffer, comp, filt, NEIGH_SB_CONFIG);
}



/*************************************
 * FRAME LEVEL DYNAMIC APPROXIMATION FUNCTIONS
 *************************************/

void ApproxHandler::initDynApprox(const char fileName[]) {
//void ApproxHandler::initDynApprox() {
  for (int i = 0; i < NUM_RA_FRAME_LEVELS; i++) { 
    dynApproxCfgs.push_back(SRAM_LOSSLESS);
  }  

  // dynApproxCfgFile = fopen(fileName.c_str(), "r");
  dynApproxCfgFile = fopen(fileName, "r");

  int frameLevel = -1;
  int approxLevel = -1;

  int res = fscanf(dynApproxCfgFile, "%d\n", &cuLevelApproxLevel);

  while(fscanf(dynApproxCfgFile, "%d;%d\n", &frameLevel, &approxLevel) != EOF) {
    dynApproxCfgs[frameLevel] = approxLevel;
  }

  std::cout << "\n\nDYNAMIC APPROX LEVELS\n";
  std::cout << "CU Level: " << cuLevelApproxLevel << std::endl;

  for (int i = 0; i < NUM_RA_FRAME_LEVELS; i++) { 
    std::cout << i << " : " << dynApproxCfgs[i] << std::endl;
  } 

  fclose(dynApproxCfgFile);
  
}

void ApproxHandler::addApproxIntraOrigSB(ComponentID comp, int frameLevel) {
  int approxLevel = dynApproxCfgs[frameLevel];
  // std::cout << "ORIG APPROX: Frame " << " " << approxLevel << "\n";
  
  addBaselineApproxIntraOrigSB(comp, approxLevel);
}

void ApproxHandler::addApproxIntraNeighSB(Pel* refBuffer, ComponentID comp, int filt, int frameLevel) {
  int approxLevel = dynApproxCfgs[frameLevel];
  // std::cout << "NEIGH APPROX: Frame " << approxLevel << "\n";

  addBaselineApproxIntraNeighSB(refBuffer, comp, filt, approxLevel);
}

/*************************************
 * CU LEVEL DYNAMIC APPROXIMATION FUNCTIONS
 *************************************/

void ApproxHandler::initCuLevelApprox(int width, int height, int nf) {
  frameWidth = width;
  frameHeight = height;
  numOfFrames = nf;

  for (int f = 0; f < numOfFrames; f++)
  {
    intraMaps[f] = NULL;
  }
}

void ApproxHandler::updateIntraMap(int framePoc, int xCU, int yCU, int wCU, int hCU) {  
  int intraMapAllocSize = (frameWidth / INTRA_MAP_RESOLUTION) * (frameHeight / INTRA_MAP_RESOLUTION);

  int xBegin = xCU / INTRA_MAP_RESOLUTION;
  int yBegin = yCU / INTRA_MAP_RESOLUTION;
  int xEnd = (xCU + wCU) / INTRA_MAP_RESOLUTION;
  int yEnd = (yCU + hCU) / INTRA_MAP_RESOLUTION;

  // std::cout << "[DBG] Before expanding: " << xBegin << " " << xEnd << " " << yBegin << " " << yEnd << std::endl;

  applyExpandFactor(&xBegin, &yBegin, &xEnd, &yEnd);
  
  // std::cout << "[DBG] After expanding: " << xBegin << " " << xEnd << " " << yBegin << " " << yEnd << std::endl;

  if(intraMaps[framePoc] == NULL) {
    // std::cout << "[DBG] Allocing intra map for frame " << framePoc << std::endl;
    intraMaps[framePoc] = (int*) malloc(intraMapAllocSize * sizeof(int));
    std::fill(intraMaps[framePoc], intraMaps[framePoc] + intraMapAllocSize, 0);
  }

  for(int x = xBegin; x < xEnd; x++) {
    for(int y = yBegin; y < yEnd; y++) {
      int pos = x + (y * (frameWidth / INTRA_MAP_RESOLUTION));
      intraMaps[framePoc][pos] = 1;
    }
  }
}

void ApproxHandler::reportIntraMap(int framePoc) {
  if(intraMaps[framePoc] == NULL) {
      std::cout << "No intra map. Skipping...\n";
      return;
  }
  std::cout << "[DBG] INTRA MAP REPORT\n";
  std::cout << "Frame " << framePoc << std::endl;
  for (int y = 0; y < (frameHeight / INTRA_MAP_RESOLUTION); y++) {
      for (int x = 0; x < (frameWidth / INTRA_MAP_RESOLUTION); x++) {
          int pos = x + (y * (frameWidth / INTRA_MAP_RESOLUTION));
          std::cout << intraMaps[framePoc][pos] << " ";
      }
      std::cout << std::endl;
  }  
}

void ApproxHandler::applyExpandFactor(int *xBegin, int *yBegin, int *xEnd, int *yEnd) {
  if(INTRA_MAP_EXPAND_FACTOR > 0) {
    int mapWidth = frameWidth / INTRA_MAP_RESOLUTION;
    int mapHeight = frameHeight / INTRA_MAP_RESOLUTION;

    *xBegin = (*xBegin) - INTRA_MAP_EXPAND_FACTOR < 0 ? 0 : (*xBegin) - INTRA_MAP_EXPAND_FACTOR;
    *yBegin = (*yBegin) - INTRA_MAP_EXPAND_FACTOR < 0 ? 0 : (*yBegin) - INTRA_MAP_EXPAND_FACTOR;

    *xEnd = (*xEnd) + INTRA_MAP_EXPAND_FACTOR >= mapWidth ? mapWidth - 1 : (*xEnd) + INTRA_MAP_EXPAND_FACTOR;
    *yEnd = (*yEnd) + INTRA_MAP_EXPAND_FACTOR >= mapHeight ? mapHeight - 1 : (*yEnd) + INTRA_MAP_EXPAND_FACTOR;
  }
}

bool ApproxHandler::checkReferenceIsIntra(int framePoc, int xCU, int yCU, int wCU, int hCU) {
  int refFrame = FRAME_REFERENCES[framePoc];

  if(refFrame == -1) 
    return false;
  
  int xBegin = xCU / INTRA_MAP_RESOLUTION;
  int yBegin = yCU / INTRA_MAP_RESOLUTION;
  int xEnd = (xCU + wCU) / INTRA_MAP_RESOLUTION;
  int yEnd = (yCU + hCU) / INTRA_MAP_RESOLUTION;
  
  for(int x = xBegin; x < xEnd; x++) {
    for(int y = yBegin; y < yEnd; y++) {
      int pos = x + (y * (frameWidth / INTRA_MAP_RESOLUTION));
      if(intraMaps[refFrame][pos] == 1) {
        return true;
      }
    }
  }
  return false;
}

void ApproxHandler::addApproxIntraNeighSB(Pel* refBuffer, ComponentID comp, int filt, int frameLevel, int framePoc, int xCU, int yCU, int wCU, int hCU) {
  bool isRefIntra = checkReferenceIsIntra(framePoc, xCU, yCU, wCU, hCU);

  // std::cout << "CU [" << framePoc << "] (" << xCU << "," << yCU << ") --> ";
  // std::cout << ((comp == COMP_Y) ? "Y" : "C") << " ";
  
  if(isRefIntra) {
    // std::cout << "CU-Level --> " << cuLevelApproxLevel << std::endl;
    addBaselineApproxIntraNeighSB(refBuffer, comp, filt, cuLevelApproxLevel);
  }
  else {
    // std::cout << "Frame-Level --> " << dynApproxCfgs[frameLevel] << std::endl;
    addApproxIntraNeighSB(refBuffer, comp, filt, frameLevel);
  }
}

void ApproxHandler::addApproxIntraOrigSB(ComponentID comp, int frameLevel, int framePoc, int xCU, int yCU, int wCU, int hCU) {
  bool isRefIntra = checkReferenceIsIntra(framePoc, xCU, yCU, wCU, hCU);
  
  if(isRefIntra) {
    addBaselineApproxIntraOrigSB(comp, cuLevelApproxLevel);
  }
  else {
    addApproxIntraOrigSB(comp, frameLevel);
  }
}

}
