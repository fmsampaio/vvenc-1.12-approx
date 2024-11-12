#include "ApproxHandler.h"

namespace vvenc {

Pel* ApproxHandler::approxIntraOrigBufferY;
Pel* ApproxHandler::approxIntraOrigBufferCb;
Pel* ApproxHandler::approxIntraOrigBufferCr;
const Pel* ApproxHandler::bkpIntraOrigBufferY;
const Pel* ApproxHandler::bkpIntraOrigBufferCb;
const Pel* ApproxHandler::bkpIntraOrigBufferCr;

void ApproxHandler::addApproxIntraOrigSB() {
    approxIntraOrigBufferY = xMalloc(Pel, 128 * 128);
    approxIntraOrigBufferCb = xMalloc(Pel, 64 * 64);
    approxIntraOrigBufferCr = xMalloc(Pel, 64 * 64);

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

void ApproxHandler::startGlobalLevel() {
    ApproxSS::start_level();
}

void ApproxHandler::endGlobalLevel() {
    ApproxSS::end_level();
}

}