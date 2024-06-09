//===-- ExprStats.cpp -----------------------------------------------------===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "klee/Expr/ExprStats.h"

using namespace klee;

Statistic stats::exprOpts("ExprOpts", "ExO");

Statistic stats::constOpts("constOpts", "CnO");
