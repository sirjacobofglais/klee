//===-- ExprStats.h ---------------------------------------------*- C++ -*-===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef KLEE_EXPRSTATS_H
#define KLEE_EXPRSTATS_H

#include "klee/Statistics/Statistic.h"

namespace klee {
namespace stats {

  // Number of successful rewrites in ExprBuilder
  // TODO: Enable only alongside opt flag (to be written)
  extern Statistic exprOpts;

  extern Statistic exprOpts1;
  extern Statistic exprOpts2;
  extern Statistic exprOpts3;
  extern Statistic exprOpts4;
  extern Statistic exprOpts5;


  // Number of exprOpts that produce a constant value
  extern Statistic constOpts;
}
}

#endif /* KLEE_EXPRSTATS_H */