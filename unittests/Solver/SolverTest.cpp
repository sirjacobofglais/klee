//===-- SolverTest.cpp ----------------------------------------------------===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "gtest/gtest.h"

#include "klee/Expr/ArrayCache.h"
#include "klee/Expr/Constraints.h"
#include "klee/Expr/Expr.h"
#include "klee/Expr/ExprBuilder.h"
#include "klee/Solver/Solver.h"
#include "klee/Solver/SolverCmdLine.h"

#include "llvm/ADT/StringExtras.h"

#include <iostream>

using namespace klee;

namespace {

const int g_constants[] = { -1, 1, 4, 17, 0 };
const Expr::Width g_types[] = { Expr::Bool,
				Expr::Int8,
				Expr::Int16,
				Expr::Int32,
				Expr::Int64 };

ref<Expr> getConstant(int value, Expr::Width width) {
  int64_t ext = value;
  uint64_t trunc = ext & (((uint64_t) -1LL) >> (64 - width));
  return exprBuilder->Constant(trunc, width);
}

// We have to have the cache globally scopped (and not in ``testOperation``)
// because the Solver (i.e. in STP's case the STPBuilder) holds on to pointers
// to allocated Arrays.
ArrayCache ac;

template<class T>
void testOperation(Solver &solver,
                   int value,
                   Expr::Width operandWidth,
                   Expr::Width resultWidth) {
  std::vector<Expr::CreateArg> symbolicArgs;
  
  for (unsigned i = 0; i < T::numKids; i++) {
    if (!T::isValidKidWidth(i, operandWidth))
      return;

    unsigned size = Expr::getMinBytesForWidth(operandWidth);
    static uint64_t id = 0;
    const Array *array = ac.CreateArray("arr" + llvm::utostr(++id), size);
    symbolicArgs.push_back(Expr::CreateArg(Expr::createTempRead(array, 
                                                                operandWidth)));
  }
  
  if (T::needsResultType())
    symbolicArgs.push_back(Expr::CreateArg(resultWidth));
  
  ref<Expr> fullySymbolicExpr = Expr::createFromKind(T::kind, symbolicArgs);

  // For each kid, replace the kid with a constant value and verify
  // that the fully symbolic expression is equivalent to it when the
  // replaced value is appropriated constrained.
  for (unsigned kid = 0; kid < T::numKids; kid++) {
    std::vector<Expr::CreateArg> partiallyConstantArgs(symbolicArgs);
    partiallyConstantArgs[kid] = getConstant(value, operandWidth);

    ref<Expr> expr = 
      exprBuilder->NotOptimized(exprBuilder->Eq(partiallyConstantArgs[kid].expr,
                                              symbolicArgs[kid].expr));
    
    ref<Expr> partiallyConstantExpr =
      Expr::createFromKind(T::kind, partiallyConstantArgs);
    
    ref<Expr> queryExpr = exprBuilder->Eq(fullySymbolicExpr, 
                                         partiallyConstantExpr);

    ConstraintSet constraints;
    ConstraintManager cm(constraints);
    cm.addConstraint(expr);
    bool res;
    bool success = solver.mustBeTrue(Query(constraints, queryExpr), res);
    EXPECT_EQ(true, success) << "Constraint solving failed";

    if (success) {
      EXPECT_EQ(true, res) << "Evaluation failed!\n" 
                           << "query " << queryExpr 
                           << " with " << expr;
    }
  }
}

template<class T>
void testOpcode(Solver &solver, bool tryBool = true, bool tryZero = true, 
                unsigned maxWidth = 64) {
  for (unsigned j=0; j<sizeof(g_types)/sizeof(g_types[0]); j++) {
    Expr::Width type = g_types[j]; 

    if (type > maxWidth) continue;

    for (unsigned i=0; i<sizeof(g_constants)/sizeof(g_constants[0]); i++) {
      int value = g_constants[i];
      if (!tryZero && !value) continue;
      if (type == Expr::Bool && !tryBool) continue;

      if (!T::needsResultType()) {
        testOperation<T>(solver, value, type, type);
        continue;
      }

      for (unsigned k=0; k<sizeof(g_types)/sizeof(g_types[0]); k++) {
        Expr::Width resultType = g_types[k];
          
        // nasty hack to give only Trunc/ZExt/SExt the right types
        if (T::kind == Expr::SExt || T::kind == Expr::ZExt) {
          if (Expr::getMinBytesForWidth(type) >= 
              Expr::getMinBytesForWidth(resultType)) 
            continue;
        }
            
        testOperation<T>(solver, value, type, resultType);
      }
    }
  }
}

TEST(SolverTest, Evaluation) {
  auto solver = klee::createCoreSolver(CoreSolverToUse);

  solver = createCexCachingSolver(std::move(solver));
  solver = createCachingSolver(std::move(solver));
  solver = createIndependentSolver(std::move(solver));

  testOpcode<SelectExpr>(*solver);
  testOpcode<ZExtExpr>(*solver);
  testOpcode<SExtExpr>(*solver);
  
  testOpcode<AddExpr>(*solver);
  testOpcode<SubExpr>(*solver);
  testOpcode<MulExpr>(*solver, false, true, 8);
  testOpcode<SDivExpr>(*solver, false, false, 8);
  testOpcode<UDivExpr>(*solver, false, false, 8);
  testOpcode<SRemExpr>(*solver, false, false, 8);
  testOpcode<URemExpr>(*solver, false, false, 8);
  testOpcode<ShlExpr>(*solver, false);
  testOpcode<LShrExpr>(*solver, false);
  testOpcode<AShrExpr>(*solver, false);
  testOpcode<AndExpr>(*solver);
  testOpcode<OrExpr>(*solver);
  testOpcode<XorExpr>(*solver);

  testOpcode<EqExpr>(*solver);
  testOpcode<NeExpr>(*solver);
  testOpcode<UltExpr>(*solver);
  testOpcode<UleExpr>(*solver);
  testOpcode<UgtExpr>(*solver);
  testOpcode<UgeExpr>(*solver);
  testOpcode<SltExpr>(*solver);
  testOpcode<SleExpr>(*solver);
  testOpcode<SgtExpr>(*solver);
  testOpcode<SgeExpr>(*solver);
}

}
