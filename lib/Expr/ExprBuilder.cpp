//===-- ExprBuilder.cpp ---------------------------------------------------===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "klee/Expr/ExprBuilder.h"
#include "klee/Expr/ExprStats.h"

using namespace klee;

ExprBuilder::ExprBuilder() {
}

ExprBuilder::~ExprBuilder() {
}

namespace {
  class DefaultExprBuilder : public ExprBuilder {
    virtual ref<Expr> Constant(const llvm::APInt &Value) {
      return ConstantExpr::alloc(Value);
    }

    virtual ref<Expr> NotOptimized(const ref<Expr> &Index) {
      return NotOptimizedExpr::alloc(Index);
    }

    virtual ref<Expr> Read(const UpdateList &Updates,
                           const ref<Expr> &Index) {
      return ReadExpr::alloc(Updates, Index);
    }

    virtual ref<Expr> Select(const ref<Expr> &Cond,
                             const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return SelectExpr::alloc(Cond, LHS, RHS);
    }

    virtual ref<Expr> Concat(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return ConcatExpr::alloc(LHS, RHS);
    }

    virtual ref<Expr> Extract(const ref<Expr> &LHS,
                              unsigned Offset, Expr::Width W) {
      return ExtractExpr::alloc(LHS, Offset, W);
    }

    virtual ref<Expr> ZExt(const ref<Expr> &LHS, Expr::Width W) {
      return ZExtExpr::alloc(LHS, W);
    }

    virtual ref<Expr> SExt(const ref<Expr> &LHS, Expr::Width W) {
      return SExtExpr::alloc(LHS, W);
    }

    virtual ref<Expr> Add(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return AddExpr::alloc(LHS, RHS);
    }

    virtual ref<Expr> Sub(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return SubExpr::alloc(LHS, RHS);
    }

    virtual ref<Expr> Mul(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return MulExpr::alloc(LHS, RHS);
    }

    virtual ref<Expr> UDiv(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return UDivExpr::alloc(LHS, RHS);
    }

    virtual ref<Expr> SDiv(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return SDivExpr::alloc(LHS, RHS);
    }

    virtual ref<Expr> URem(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return URemExpr::alloc(LHS, RHS);
    }

    virtual ref<Expr> SRem(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return SRemExpr::alloc(LHS, RHS);
    }

    virtual ref<Expr> Not(const ref<Expr> &LHS) {
      return NotExpr::alloc(LHS);
    }

    virtual ref<Expr> And(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return AndExpr::alloc(LHS, RHS);
    }

    virtual ref<Expr> Or(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return OrExpr::alloc(LHS, RHS);
    }

    virtual ref<Expr> Xor(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return XorExpr::alloc(LHS, RHS);
    }

    virtual ref<Expr> Shl(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return ShlExpr::alloc(LHS, RHS);
    }

    virtual ref<Expr> LShr(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return LShrExpr::alloc(LHS, RHS);
    }

    virtual ref<Expr> AShr(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return AShrExpr::alloc(LHS, RHS);
    }

    virtual ref<Expr> Eq(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return EqExpr::alloc(LHS, RHS);
    }

    virtual ref<Expr> Ne(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return NeExpr::alloc(LHS, RHS);
    }

    virtual ref<Expr> Ult(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return UltExpr::alloc(LHS, RHS);
    }

    virtual ref<Expr> Ule(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return UleExpr::alloc(LHS, RHS);
    }

    virtual ref<Expr> Ugt(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return UgtExpr::alloc(LHS, RHS);
    }

    virtual ref<Expr> Uge(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return UgeExpr::alloc(LHS, RHS);
    }

    virtual ref<Expr> Slt(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return SltExpr::alloc(LHS, RHS);
    }

    virtual ref<Expr> Sle(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return SleExpr::alloc(LHS, RHS);
    }

    virtual ref<Expr> Sgt(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return SgtExpr::alloc(LHS, RHS);
    }

    virtual ref<Expr> Sge(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return SgeExpr::alloc(LHS, RHS);
    }
  };

  /// ChainedBuilder - Helper class for construct specialized expression
  /// builders, which implements (non-virtual) methods which forward to a base
  /// expression builder, for all expressions.
  class ChainedBuilder {
  protected:
    /// Builder - The builder that this specialized builder is contained
    /// within. Provided for convenience to clients.
    ExprBuilder *Builder;

    /// Base - The base builder class for constructing expressions.
    ExprBuilder *Base;

  public:
    ChainedBuilder(ExprBuilder *_Builder, ExprBuilder *_Base) 
      : Builder(_Builder), Base(_Base) {}
    ~ChainedBuilder() { delete Base; }

    ref<Expr> Constant(const llvm::APInt &Value) {
      return Base->Constant(Value);
    }

    ref<Expr> NotOptimized(const ref<Expr> &Index) {
      return Base->NotOptimized(Index);
    }

    ref<Expr> Read(const UpdateList &Updates,
                   const ref<Expr> &Index) {
      return Base->Read(Updates, Index);
    }

    ref<Expr> Select(const ref<Expr> &Cond,
                     const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return Base->Select(Cond, LHS, RHS);
    }

    ref<Expr> Concat(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return Base->Concat(LHS, RHS);
    }

    ref<Expr> Extract(const ref<Expr> &LHS,
                      unsigned Offset, Expr::Width W) {
      return Base->Extract(LHS, Offset, W);
    }

    ref<Expr> ZExt(const ref<Expr> &LHS, Expr::Width W) {
      return Base->ZExt(LHS, W);
    }

    ref<Expr> SExt(const ref<Expr> &LHS, Expr::Width W) {
      return Base->SExt(LHS, W);
    }

    ref<Expr> Add(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return Base->Add(LHS, RHS);
    }

    ref<Expr> Sub(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return Base->Sub(LHS, RHS);
    }

    ref<Expr> Mul(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return Base->Mul(LHS, RHS);
    }

    ref<Expr> UDiv(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return Base->UDiv(LHS, RHS);
    }

    ref<Expr> SDiv(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return Base->SDiv(LHS, RHS);
    }

    ref<Expr> URem(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return Base->URem(LHS, RHS);
    }

    ref<Expr> SRem(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return Base->SRem(LHS, RHS);
    }

    ref<Expr> Not(const ref<Expr> &LHS) {
      return Base->Not(LHS);
    }

    ref<Expr> And(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return Base->And(LHS, RHS);
    }

    ref<Expr> Or(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return Base->Or(LHS, RHS);
    }

    ref<Expr> Xor(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return Base->Xor(LHS, RHS);
    }

    ref<Expr> Shl(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return Base->Shl(LHS, RHS);
    }

    ref<Expr> LShr(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return Base->LShr(LHS, RHS);
    }

    ref<Expr> AShr(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return Base->AShr(LHS, RHS);
    }

    ref<Expr> Eq(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return Base->Eq(LHS, RHS);
    }

    ref<Expr> Ne(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return Base->Ne(LHS, RHS);
    }

    ref<Expr> Ult(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return Base->Ult(LHS, RHS);
    }

    ref<Expr> Ule(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return Base->Ule(LHS, RHS);
    }

    ref<Expr> Ugt(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return Base->Ugt(LHS, RHS);
    }

    ref<Expr> Uge(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return Base->Uge(LHS, RHS);
    }

    ref<Expr> Slt(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return Base->Slt(LHS, RHS);
    }

    ref<Expr> Sle(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return Base->Sle(LHS, RHS);
    }

    ref<Expr> Sgt(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return Base->Sgt(LHS, RHS);
    }

    ref<Expr> Sge(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      return Base->Sge(LHS, RHS);
    }
  };

  /// ConstantSpecializedExprBuilder - A base expression builder class which
  /// handles dispatching to a helper class, based on whether the arguments are
  /// constant or not.
  ///
  /// The SpecializedBuilder template argument should be a helper class which
  /// implements methods for all the expression construction functions. These
  /// methods can be specialized to take [Non]ConstantExpr when desired.
  template<typename SpecializedBuilder>
  class ConstantSpecializedExprBuilder : public ExprBuilder {
    SpecializedBuilder Builder;

  public:
    ConstantSpecializedExprBuilder(ExprBuilder *Base) : Builder(this, Base) {}
    ~ConstantSpecializedExprBuilder() {}

    virtual ref<Expr> Constant(const llvm::APInt &Value) {
      return Builder.Constant(Value);
    }

    virtual ref<Expr> NotOptimized(const ref<Expr> &Index) {
      return Builder.NotOptimized(Index);
    }

    virtual ref<Expr> Read(const UpdateList &Updates,
                           const ref<Expr> &Index) {
      // Roll back through writes when possible.
      auto UN = Updates.head;
      while (UN && Eq(Index, UN->index)->isFalse())
        UN = UN->next;

      if (ConstantExpr *CE = dyn_cast<ConstantExpr>(Index))
        return Builder.Read(UpdateList(Updates.root, UN), CE);

      return Builder.Read(UpdateList(Updates.root, UN), Index);
    }

    virtual ref<Expr> Select(const ref<Expr> &Cond,
                             const ref<Expr> &LHS, const ref<Expr> &RHS) {
      if (ConstantExpr *CE = dyn_cast<ConstantExpr>(Cond))
        return CE->isTrue() ? LHS : RHS;

      return Builder.Select(cast<NonConstantExpr>(Cond), LHS, RHS);
    }

    virtual ref<Expr> Concat(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      if (ConstantExpr *LCE = dyn_cast<ConstantExpr>(LHS)) {
        if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS))
          return LCE->Concat(RCE);
        return Builder.Concat(LCE, cast<NonConstantExpr>(RHS));
      } else if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS)) {
        return Builder.Concat(cast<NonConstantExpr>(LHS), RCE);
      }

      return Builder.Concat(cast<NonConstantExpr>(LHS),
                            cast<NonConstantExpr>(RHS));
    }

    virtual ref<Expr> Extract(const ref<Expr> &LHS,
                              unsigned Offset, Expr::Width W) {
      if (ConstantExpr *CE = dyn_cast<ConstantExpr>(LHS))
        return CE->Extract(Offset, W);

      return Builder.Extract(cast<NonConstantExpr>(LHS), Offset, W);
    }

    virtual ref<Expr> ZExt(const ref<Expr> &LHS, Expr::Width W) {
      if (ConstantExpr *CE = dyn_cast<ConstantExpr>(LHS))
        return CE->ZExt(W);

      return Builder.ZExt(cast<NonConstantExpr>(LHS), W);
    }

    virtual ref<Expr> SExt(const ref<Expr> &LHS, Expr::Width W) {
      if (ConstantExpr *CE = dyn_cast<ConstantExpr>(LHS))
        return CE->SExt(W);

      return Builder.SExt(cast<NonConstantExpr>(LHS), W);
    }

    virtual ref<Expr> Add(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      if (ConstantExpr *LCE = dyn_cast<ConstantExpr>(LHS)) {
        if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS))
          return LCE->Add(RCE);
        return Builder.Add(LCE, cast<NonConstantExpr>(RHS));
      } else if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS)) {
        return Builder.Add(cast<NonConstantExpr>(LHS), RCE);
      }

      return Builder.Add(cast<NonConstantExpr>(LHS),
                         cast<NonConstantExpr>(RHS));
    }

    virtual ref<Expr> Sub(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      if (ConstantExpr *LCE = dyn_cast<ConstantExpr>(LHS)) {
        if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS))
          return LCE->Sub(RCE);
        return Builder.Sub(LCE, cast<NonConstantExpr>(RHS));
      } else if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS)) {
        return Builder.Sub(cast<NonConstantExpr>(LHS), RCE);
      }

      return Builder.Sub(cast<NonConstantExpr>(LHS),
                         cast<NonConstantExpr>(RHS));
    }

    virtual ref<Expr> Mul(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      if (ConstantExpr *LCE = dyn_cast<ConstantExpr>(LHS)) {
        if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS))
          return LCE->Mul(RCE);
        return Builder.Mul(LCE, cast<NonConstantExpr>(RHS));
      } else if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS)) {
        return Builder.Mul(cast<NonConstantExpr>(LHS), RCE);
      }

      return Builder.Mul(cast<NonConstantExpr>(LHS),
                         cast<NonConstantExpr>(RHS));
    }

    virtual ref<Expr> UDiv(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      if (ConstantExpr *LCE = dyn_cast<ConstantExpr>(LHS)) {
        if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS))
          return LCE->UDiv(RCE);
        return Builder.UDiv(LCE, cast<NonConstantExpr>(RHS));
      } else if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS)) {
        return Builder.UDiv(cast<NonConstantExpr>(LHS), RCE);
      }

      return Builder.UDiv(cast<NonConstantExpr>(LHS),
                          cast<NonConstantExpr>(RHS));
    }

    virtual ref<Expr> SDiv(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      if (ConstantExpr *LCE = dyn_cast<ConstantExpr>(LHS)) {
        if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS))
          return LCE->SDiv(RCE);
        return Builder.SDiv(LCE, cast<NonConstantExpr>(RHS));
      } else if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS)) {
        return Builder.SDiv(cast<NonConstantExpr>(LHS), RCE);
      }

      return Builder.SDiv(cast<NonConstantExpr>(LHS),
                          cast<NonConstantExpr>(RHS));
    }

    virtual ref<Expr> URem(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      if (ConstantExpr *LCE = dyn_cast<ConstantExpr>(LHS)) {
        if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS))
          return LCE->URem(RCE);
        return Builder.URem(LCE, cast<NonConstantExpr>(RHS));
      } else if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS)) {
        return Builder.URem(cast<NonConstantExpr>(LHS), RCE);
      }

      return Builder.URem(cast<NonConstantExpr>(LHS),
                          cast<NonConstantExpr>(RHS));
    }

    virtual ref<Expr> SRem(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      if (ConstantExpr *LCE = dyn_cast<ConstantExpr>(LHS)) {
        if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS))
          return LCE->SRem(RCE);
        return Builder.SRem(LCE, cast<NonConstantExpr>(RHS));
      } else if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS)) {
        return Builder.SRem(cast<NonConstantExpr>(LHS), RCE);
      }

      return Builder.SRem(cast<NonConstantExpr>(LHS),
                          cast<NonConstantExpr>(RHS));
    }

    virtual ref<Expr> Not(const ref<Expr> &LHS) {
      // !!X ==> X
      if (NotExpr *DblNot = dyn_cast<NotExpr>(LHS))
        return DblNot->getKid(0);

      if (ConstantExpr *CE = dyn_cast<ConstantExpr>(LHS))
        return CE->Not();

      return Builder.Not(cast<NonConstantExpr>(LHS));
    }

    virtual ref<Expr> And(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      if (ConstantExpr *LCE = dyn_cast<ConstantExpr>(LHS)) {
        if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS))
          return LCE->And(RCE);
        return Builder.And(LCE, cast<NonConstantExpr>(RHS));
      } else if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS)) {
        return Builder.And(cast<NonConstantExpr>(LHS), RCE);
      }

      return Builder.And(cast<NonConstantExpr>(LHS),
                         cast<NonConstantExpr>(RHS));
    }

    virtual ref<Expr> Or(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      if (ConstantExpr *LCE = dyn_cast<ConstantExpr>(LHS)) {
        if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS))
          return LCE->Or(RCE);
        return Builder.Or(LCE, cast<NonConstantExpr>(RHS));
      } else if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS)) {
        return Builder.Or(cast<NonConstantExpr>(LHS), RCE);
      }

      return Builder.Or(cast<NonConstantExpr>(LHS),
                        cast<NonConstantExpr>(RHS));
    }

    virtual ref<Expr> Xor(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      if (ConstantExpr *LCE = dyn_cast<ConstantExpr>(LHS)) {
        if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS))
          return LCE->Xor(RCE);
        return Builder.Xor(LCE, cast<NonConstantExpr>(RHS));
      } else if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS)) {
        return Builder.Xor(cast<NonConstantExpr>(LHS), RCE);
      }

      return Builder.Xor(cast<NonConstantExpr>(LHS),
                         cast<NonConstantExpr>(RHS));
    }

    virtual ref<Expr> Shl(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      if (ConstantExpr *LCE = dyn_cast<ConstantExpr>(LHS)) {
        if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS))
          return LCE->Shl(RCE);
        return Builder.Shl(LCE, cast<NonConstantExpr>(RHS));
      } else if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS)) {
        return Builder.Shl(cast<NonConstantExpr>(LHS), RCE);
      }

      return Builder.Shl(cast<NonConstantExpr>(LHS),
                         cast<NonConstantExpr>(RHS));
    }

    virtual ref<Expr> LShr(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      if (ConstantExpr *LCE = dyn_cast<ConstantExpr>(LHS)) {
        if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS))
          return LCE->LShr(RCE);
        return Builder.LShr(LCE, cast<NonConstantExpr>(RHS));
      } else if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS)) {
        return Builder.LShr(cast<NonConstantExpr>(LHS), RCE);
      }

      return Builder.LShr(cast<NonConstantExpr>(LHS),
                          cast<NonConstantExpr>(RHS));
    }

    virtual ref<Expr> AShr(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      if (ConstantExpr *LCE = dyn_cast<ConstantExpr>(LHS)) {
        if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS))
          return LCE->AShr(RCE);
        return Builder.AShr(LCE, cast<NonConstantExpr>(RHS));
      } else if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS)) {
        return Builder.AShr(cast<NonConstantExpr>(LHS), RCE);
      }

      return Builder.AShr(cast<NonConstantExpr>(LHS),
                          cast<NonConstantExpr>(RHS));
    }

    virtual ref<Expr> Eq(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      if (ConstantExpr *LCE = dyn_cast<ConstantExpr>(LHS)) {
        if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS))
          return LCE->Eq(RCE);
        return Builder.Eq(LCE, cast<NonConstantExpr>(RHS));
      } else if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS)) {
        return Builder.Eq(cast<NonConstantExpr>(LHS), RCE);
      }

      return Builder.Eq(cast<NonConstantExpr>(LHS),
                        cast<NonConstantExpr>(RHS));
    }

    virtual ref<Expr> Ne(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      if (ConstantExpr *LCE = dyn_cast<ConstantExpr>(LHS)) {
        if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS))
          return LCE->Ne(RCE);
        return Builder.Ne(LCE, cast<NonConstantExpr>(RHS));
      } else if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS)) {
        return Builder.Ne(cast<NonConstantExpr>(LHS), RCE);
      }

      return Builder.Ne(cast<NonConstantExpr>(LHS),
                        cast<NonConstantExpr>(RHS));
    }

    virtual ref<Expr> Ult(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      if (ConstantExpr *LCE = dyn_cast<ConstantExpr>(LHS)) {
        if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS))
          return LCE->Ult(RCE);
        return Builder.Ult(LCE, cast<NonConstantExpr>(RHS));
      } else if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS)) {
        return Builder.Ult(cast<NonConstantExpr>(LHS), RCE);
      }

      return Builder.Ult(cast<NonConstantExpr>(LHS),
                         cast<NonConstantExpr>(RHS));
    }

    virtual ref<Expr> Ule(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      if (ConstantExpr *LCE = dyn_cast<ConstantExpr>(LHS)) {
        if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS))
          return LCE->Ule(RCE);
        return Builder.Ule(LCE, cast<NonConstantExpr>(RHS));
      } else if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS)) {
        return Builder.Ule(cast<NonConstantExpr>(LHS), RCE);
      }

      return Builder.Ule(cast<NonConstantExpr>(LHS),
                         cast<NonConstantExpr>(RHS));
    }

    virtual ref<Expr> Ugt(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      if (ConstantExpr *LCE = dyn_cast<ConstantExpr>(LHS)) {
        if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS))
          return LCE->Ugt(RCE);
        return Builder.Ugt(LCE, cast<NonConstantExpr>(RHS));
      } else if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS)) {
        return Builder.Ugt(cast<NonConstantExpr>(LHS), RCE);
      }

      return Builder.Ugt(cast<NonConstantExpr>(LHS),
                         cast<NonConstantExpr>(RHS));
    }

    virtual ref<Expr> Uge(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      if (ConstantExpr *LCE = dyn_cast<ConstantExpr>(LHS)) {
        if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS))
          return LCE->Uge(RCE);
        return Builder.Uge(LCE, cast<NonConstantExpr>(RHS));
      } else if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS)) {
        return Builder.Uge(cast<NonConstantExpr>(LHS), RCE);
      }

      return Builder.Uge(cast<NonConstantExpr>(LHS),
                         cast<NonConstantExpr>(RHS));
    }

    virtual ref<Expr> Slt(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      if (ConstantExpr *LCE = dyn_cast<ConstantExpr>(LHS)) {
        if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS))
          return LCE->Slt(RCE);
        return Builder.Slt(LCE, cast<NonConstantExpr>(RHS));
      } else if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS)) {
        return Builder.Slt(cast<NonConstantExpr>(LHS), RCE);
      }

      return Builder.Slt(cast<NonConstantExpr>(LHS),
                         cast<NonConstantExpr>(RHS));
    }

    virtual ref<Expr> Sle(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      if (ConstantExpr *LCE = dyn_cast<ConstantExpr>(LHS)) {
        if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS))
          return LCE->Sle(RCE);
        return Builder.Sle(LCE, cast<NonConstantExpr>(RHS));
      } else if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS)) {
        return Builder.Sle(cast<NonConstantExpr>(LHS), RCE);
      }

      return Builder.Sle(cast<NonConstantExpr>(LHS),
                         cast<NonConstantExpr>(RHS));
    }

    virtual ref<Expr> Sgt(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      if (ConstantExpr *LCE = dyn_cast<ConstantExpr>(LHS)) {
        if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS))
          return LCE->Sgt(RCE);
        return Builder.Sgt(LCE, cast<NonConstantExpr>(RHS));
      } else if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS)) {
        return Builder.Sgt(cast<NonConstantExpr>(LHS), RCE);
      }

      return Builder.Sgt(cast<NonConstantExpr>(LHS),
                         cast<NonConstantExpr>(RHS));
    }

    virtual ref<Expr> Sge(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      if (ConstantExpr *LCE = dyn_cast<ConstantExpr>(LHS)) {
        if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS))
          return LCE->Sge(RCE);
        return Builder.Sge(LCE, cast<NonConstantExpr>(RHS));
      } else if (ConstantExpr *RCE = dyn_cast<ConstantExpr>(RHS)) {
        return Builder.Sge(cast<NonConstantExpr>(LHS), RCE);
      }

      return Builder.Sge(cast<NonConstantExpr>(LHS),
                         cast<NonConstantExpr>(RHS));
    }
  };

  class ConstantFoldingBuilder :
    public ChainedBuilder {
  public:
    ConstantFoldingBuilder(ExprBuilder *Builder, ExprBuilder *Base)
      : ChainedBuilder(Builder, Base) {}

    // Used to mark Optimising rewrites only, not canonicalisations
    ref<Expr> record_opt(ref<Expr> val) {

      ++stats::exprOpts;
      return val;
    }

    // Used to mark rewrites that produce a constant
    ref<Expr> record_const_opt(ref<Expr> val) {
      ++stats::constOpts;
      return record_opt(val);
    }

    bool exactMatch(Expr *LHS, Expr *RHS) {
      return !LHS->compare(*RHS);
    }

    bool matchLeftChild(BinaryExpr *BE, ref<Expr> match) {
      return exactMatch(BE->left.get(), match.get());
    }

    bool matchRightChild(BinaryExpr *BE, ref<Expr> match) {
      return exactMatch(BE->right.get(), match.get());
    }

    // return child not matched if match, or nullptr
    ref<Expr> matchEitherChild(BinaryExpr *BE, ref<Expr> match) {
      if (matchLeftChild(BE, match))
        return BE->right;
      if (matchRightChild(BE, match))
        return BE->left;
      else return nullptr;
    }

    bool matchNegated(NotExpr *NE, ref<Expr> match) {
      return exactMatch(NE->expr.get(), match.get());
    }

    bool matchBinaryExprsChildren(BinaryExpr *LHS, BinaryExpr *RHS) {
      return ((exactMatch(LHS->left.get(), RHS->left.get()) && exactMatch(LHS->right.get(), RHS->right.get()))
              || (exactMatch(LHS->left.get(), RHS->right.get()) && exactMatch(LHS->right.get(), RHS->left.get()))); 
    }

    bool matchBinaryExprsChildWithNeg(BinaryExpr *LHS, BinaryExpr *RHS) {
      return ((exactMatch(LHS->left.get(), RHS->left.get()) && exactMatch(LHS->right.get(), RHS->right.get()))
              || (exactMatch(LHS->left.get(), RHS->right.get()) && exactMatch(LHS->right.get(), RHS->left.get()))); 
    }

    bool checkConstantZExtRange(const ref<ConstantExpr> &c, ZExtExpr *zExtExpr) {
      llvm::APInt val = llvm::APInt(c->getAPValue());
      llvm::APInt trunced = val.zextOrTrunc(zExtExpr->src->getWidth()).zextOrTrunc(zExtExpr->getWidth());

      return trunced.eq(c->getAPValue());
    }

    bool checkConstantSExtRange(const ref<ConstantExpr> &c, SExtExpr *sExtExpr) {
      llvm::APInt val = llvm::APInt(c->getAPValue());
      llvm::APInt trunced = val.sextOrTrunc(sExtExpr->src->getWidth()).sextOrTrunc(sExtExpr->getWidth());

      return trunced.eq(c->getAPValue());
    }

    ref<Expr> Add(const ref<ConstantExpr> &LHS,
                  const ref<NonConstantExpr> &RHS) {
      // 0 + X ==> X
      if (LHS->isZero())
        return record_opt(RHS);

      switch (RHS->getKind()) {
      default: break;

      case Expr::Add: {
        BinaryExpr *BE = cast<BinaryExpr>(RHS);
        // C_0 + (C_1 + X) ==> (C_0 + C1) + X
        if (ConstantExpr *CE = dyn_cast<ConstantExpr>(BE->left))
          return Builder->Add(LHS->Add(CE), BE->right);
        // C_0 + (X + C_1) ==> (C_0 + C1) + X
        if (ConstantExpr *CE = dyn_cast<ConstantExpr>(BE->right))
          return Builder->Add(LHS->Add(CE), BE->left);
        break;
      }

      case Expr::Sub: {
        BinaryExpr *BE = cast<BinaryExpr>(RHS);
        // C_0 + (C_1 - X) ==> (C_0 + C1) - X
        if (ConstantExpr *CE = dyn_cast<ConstantExpr>(BE->left))
          return Builder->Sub(LHS->Add(CE), BE->right);
        // C_0 + (X - C_1) ==> (C_0 - C1) + X
        if (ConstantExpr *CE = dyn_cast<ConstantExpr>(BE->right))
          return Builder->Add(LHS->Sub(CE), BE->left);
        break;
      }
      }

      return Base->Add(LHS, RHS);
    }

    ref<Expr> Add(const ref<NonConstantExpr> &LHS,
                  const ref<ConstantExpr> &RHS) {
      return Add(RHS, LHS);
    }

    ref<Expr> Add(const ref<NonConstantExpr> &LHS,
                  const ref<NonConstantExpr> &RHS) {

      if (exactMatch(LHS.get(), RHS.get())) {
        // X + X => X << 1
        return record_const_opt(Builder->Shl(LHS, Builder->Constant(1, LHS->getWidth())));
      }
  
      switch (LHS->getKind()) {
        default: break;

        case Expr::Add: {
          BinaryExpr *BE = cast<BinaryExpr>(LHS);
          // (X + Y) + Z ==> X + (Y + Z)

          if (exactMatch(BE->left.get(), RHS.get()))
          // Bring identical terms together as will match further
            return record_opt(Builder->Add(BE->right,
                              Builder->Add(BE->left, RHS))); 
          else return Builder->Add(BE->left,
                                  Builder->Add(BE->right, RHS));
        }

        case Expr::Sub: {
          BinaryExpr *BE = cast<BinaryExpr>(LHS);
          // (X - Y) + Z ==> X + (Z - Y)
          return Builder->Add(BE->left,
                              Builder->Sub(RHS, BE->right));
        }

        case Expr::Not: {
          NotExpr *NE = cast<NotExpr>(LHS);
          // ~X + X ==> -1
          if (matchNegated(NE, RHS)) {
              return record_const_opt(Builder->AllOnes(RHS->getWidth()));
          };
          break;
        }

        case Expr::Xor: {
          BinaryExpr *LBE = cast<BinaryExpr>(LHS);

          // (A ^ B) + (A & B) => A | B
          if (BinaryExpr *RBE = dyn_cast<BinaryExpr>(RHS)) 
            if (RBE->getKind() == Expr::And && matchBinaryExprsChildren(LBE, RBE))
              return record_opt(Builder->Or(LBE->left, LBE->right));
          break;
        }

        case Expr::Or: {
          BinaryExpr *LBE = cast<BinaryExpr>(LHS);

          // (A | B) + (A & B) => A + B
          if (BinaryExpr *RBE = dyn_cast<BinaryExpr>(RHS)) 
            if (RBE->getKind() == Expr::And && matchBinaryExprsChildren(LBE, RBE))
              return record_opt(Builder->Add(LBE->left, LBE->right));
          break;
        }
      }

      switch (RHS->getKind()) {
        default: break;

        case Expr::Add: {
          BinaryExpr *BE = cast<BinaryExpr>(RHS);
          // X + (C_0 + Y) ==> C_0 + (X + Y)
          if (ConstantExpr *CE = dyn_cast<ConstantExpr>(BE->left))
            return Builder->Add(CE, Builder->Add(LHS, BE->right));
          // X + (Y + C_0) ==> C_0 + (X + Y)
          if (ConstantExpr *CE = dyn_cast<ConstantExpr>(BE->right))
            return Builder->Add(CE, Builder->Add(LHS, BE->left));
          break;
        }

        case Expr::Sub: {
          BinaryExpr *BE = cast<BinaryExpr>(RHS);
          
          if (ConstantExpr *CE = dyn_cast<ConstantExpr>(BE->left))
            // X + (C_0 - Y) ==> C_0 + (X - Y)
            return Builder->Add(CE, Builder->Sub(LHS, BE->right));
          else if (exactMatch(BE->right.get(), LHS.get()))
            // X + (Y - X) => Y
            return record_opt(BE->left);
          
          if (ConstantExpr *CE = dyn_cast<ConstantExpr>(BE->right))
            // X + (Y - C_0) ==> -C_0 + (X + Y)
            return Builder->Add(CE->Neg(), Builder->Add(LHS, BE->left));
          break;
        }

        case Expr::Not: {
          NotExpr *NE = cast<NotExpr>(RHS);
          // X + ~X ==> -1
          if (matchNegated(NE, LHS)) {
              return record_const_opt(Builder->AllOnes(LHS->getWidth()));
          };
          break;
        }

        case Expr::Xor: {
          BinaryExpr *RBE = cast<BinaryExpr>(RHS);

          // (A & B) + (A ^ B) => A | B
          if (BinaryExpr *LBE = dyn_cast<BinaryExpr>(LHS)) 
            if (LBE->getKind() == Expr::And && matchBinaryExprsChildren(LBE, RBE))
              return record_opt(Builder->Or(LBE->left, LBE->right));
          break;
        }

        case Expr::Or: {
          BinaryExpr *RBE = cast<BinaryExpr>(RHS);

          // (A & B) + (A | B) => A + B
          if (BinaryExpr *LBE = dyn_cast<BinaryExpr>(LHS)) 
            if (LBE->getKind() == Expr::And && matchBinaryExprsChildren(LBE, RBE))
              return record_opt(Builder->Add(LBE->left, LBE->right));
          break;
        }
      }

      return Base->Add(LHS, RHS);
    }

    ref<Expr> Sub(const ref<ConstantExpr> &LHS,
                  const ref<NonConstantExpr> &RHS) {

      if (LHS->isAllOnes())
        // (allOnes) - X => ~X
        return record_opt(Builder->Not(RHS));

      if (LHS->isZero()) {

        switch (RHS->getKind()) {
          default: break;

          case Expr::Add: {
            BinaryExpr *BE = cast<BinaryExpr>(RHS);

            if (ConstantExpr *CE = dyn_cast<ConstantExpr>(BE->left)) {
              // -(C + X) => (-C) - X 
              return record_opt(Builder->Sub(CE->Neg(), BE->right));
            }
            break;
          }

          case Expr::Sub: {
            BinaryExpr *BE = cast<BinaryExpr>(RHS);

            // 0 - (X - Y) => Y - X
            return record_opt(Builder->Sub(BE->right, BE->left));
          }

          case Expr::Mul: {
            BinaryExpr *BE = cast<BinaryExpr>(RHS);

            if (ConstantExpr *CE = dyn_cast<ConstantExpr>(BE->left)) {
              // -(C * X) => (-C) * X
              return record_opt(Builder->Mul(CE->Neg(), BE->right));
            }
            break;
          }

          case Expr::UDiv: {
            BinaryExpr *BE = cast<BinaryExpr>(RHS);

            if (ConstantExpr *CE = dyn_cast<ConstantExpr>(BE->left)) {
              // -(C / X) => (-C) / X
              return record_opt(Builder->UDiv(CE->Neg(), BE->right));
            }
            else if (ConstantExpr *CE = dyn_cast<ConstantExpr>(BE->left)) {
              // -(X / C) => X / (-C)
              return record_opt(Builder->UDiv(BE->left, CE->Neg()));
            }
            break;
          }

          case Expr::SDiv: {
            BinaryExpr *BE = cast<BinaryExpr>(RHS);

            if (ConstantExpr *CE = dyn_cast<ConstantExpr>(BE->left)) {
              // -(C / X) => (-C) / X
              return record_opt(Builder->SDiv(CE->Neg(), BE->right));
            }
            else if (ConstantExpr *CE = dyn_cast<ConstantExpr>(BE->right)) {
              // -(X / C) => X / (-C)
              return record_opt(Builder->SDiv(BE->left, CE->Neg()));
            }
            break;
          }
        }
      }

      switch (RHS->getKind()) {
      default: break;

      case Expr::Not: {
        NotExpr *NE = cast<NotExpr>(RHS);

        // C - ~X => (1 + C) + X
        return record_opt(Builder->Add(LHS->Add(ConstantExpr::create(1, LHS->getWidth())), NE->expr));
      }

      case Expr::Add: {
        BinaryExpr *BE = cast<BinaryExpr>(RHS);
        // C_0 - (C_1 + X) ==> (C_0 - C1) - X
        if (ConstantExpr *CE = dyn_cast<ConstantExpr>(BE->left))
          return record_opt(Builder->Sub(LHS->Sub(CE), BE->right));
        // C_0 - (X + C_1) ==> (C_0 + C1) + X
        if (ConstantExpr *CE = dyn_cast<ConstantExpr>(BE->right))
          return record_opt(Builder->Sub(LHS->Sub(CE), BE->left));
        break;
      }

      case Expr::Sub: {
        BinaryExpr *BE = cast<BinaryExpr>(RHS);
        // C_0 - (C_1 - X) ==> (C_0 - C1) + X
        if (ConstantExpr *CE = dyn_cast<ConstantExpr>(BE->left))
          return record_opt(Builder->Add(LHS->Sub(CE), BE->right));
        // C_0 - (X - C_1) ==> (C_0 + C1) - X
        if (ConstantExpr *CE = dyn_cast<ConstantExpr>(BE->right))
          return record_opt(Builder->Sub(LHS->Add(CE), BE->left));
        break;
      }
      }

      return Base->Sub(LHS, RHS);
    }

    ref<Expr> Sub(const ref<NonConstantExpr> &LHS,
                  const ref<ConstantExpr> &RHS) {
        // X - C_0 ==> -C_0 + X
      return record_opt(Add(RHS->Neg(), LHS));
    }

    ref<Expr> Sub(const ref<NonConstantExpr> &LHS,
                  const ref<NonConstantExpr> &RHS) {

      if (exactMatch(LHS.get(), RHS.get()))
        // X - X => 0
        return record_opt(Base->Zero(LHS->getWidth()));

      switch (LHS->getKind()) {
        default: break;

        case Expr::Not: {
          NotExpr *LNE = cast<NotExpr>(LHS);

          if (NotExpr *RNE = dyn_cast<NotExpr>(RHS))
            // ~X - ~Y => Y - X
            return record_opt(Builder->Sub(RNE->expr, LNE->expr));
          break;
        }

        case Expr::Add: {
          BinaryExpr *BE = cast<BinaryExpr>(LHS);

          // (A + B) - (A | B) => A & B
          if (RHS->getKind() == Expr::Or) {
            BinaryExpr *RBE = cast<BinaryExpr>(RHS);
            if (matchBinaryExprsChildren(BE, RBE))
              return record_opt(Builder->And(BE->left, BE->right));
          }
          // (A + B) - (A & B) => A | B 
          else if (RHS->getKind() == Expr::And) {
            BinaryExpr *RBE = cast<BinaryExpr>(RHS);
            if (matchBinaryExprsChildren(BE, RBE))
              return record_opt(Builder->Or(BE->left, BE->right));
          }

          // (X + Y) - Z ==> X + (Y - Z)
          return Builder->Add(BE->left, Builder->Sub(BE->right, RHS));
        }

        case Expr::Sub: {
          BinaryExpr *BE = cast<BinaryExpr>(LHS);
          // (X - Y) - Z ==> X - (Y + Z)
          return Builder->Sub(BE->left, Builder->Add(BE->right, RHS));
        }

        case Expr::Mul: {
          BinaryExpr *BE = cast<BinaryExpr>(LHS);
          
          if (ConstantExpr *CE = dyn_cast<ConstantExpr>(BE->left))
            if (exactMatch(BE->right.get(), RHS.get()))
              // (C * X) - X => (C - 1) * X
              return record_opt(Builder->Mul(CE->Sub(ConstantExpr::create(1, CE->getWidth())), RHS));
          break;
        }

        case Expr::And: {
          BinaryExpr *LBE = cast<BinaryExpr>(LHS);

          // (A & B) - (A | B) => ~(A ^ B)
          if (RHS->getKind() == Expr::Or) {
            BinaryExpr *RBE = cast<BinaryExpr>(RHS);

            if (matchBinaryExprsChildren(LBE, RBE))
              return record_opt(Builder->Not(Builder->Xor(LBE->left, LBE->right)));
          }
          break;
        }

        case Expr::Or: {
          BinaryExpr *LBE = cast<BinaryExpr>(LHS);

          if (ref<Expr> otherChild = matchEitherChild(LBE, RHS))
            // (X | Y) - X => ~X & Y
            return record_opt(Builder->And(Builder->Not(RHS), otherChild));

          // (A | B) - (A & B) => A ^ B
          if (RHS->getKind() == Expr::And) {
            BinaryExpr *RBE = cast<BinaryExpr>(RHS);

            if (matchBinaryExprsChildren(LBE, RBE))
              return record_opt(Builder->Xor(LBE->left, LBE->right));
          }

          // (A | B) - (A ^ B) => A & B
          if (RHS->getKind() == Expr::Xor) {
            BinaryExpr *RBE = cast<BinaryExpr>(RHS);

            if (matchBinaryExprsChildren(LBE, RBE))
              return record_opt(Builder->And(LBE->left, LBE->right));
          }
          break;
        }

        case Expr::Xor: {
          BinaryExpr *LBE = cast<BinaryExpr>(LHS);

          // (A ^ B) - (A | B) => ~(A & B)
          if (RHS->getKind() == Expr::Or) {
            BinaryExpr *RBE = cast<BinaryExpr>(RHS);

            if (matchBinaryExprsChildren(LBE, RBE))
              return record_opt(Builder->Not(Builder->And(LBE->left, LBE->right)));
          }
          break;
        }
      }

      switch (RHS->getKind()) {
        default: break;

        case Expr::Add: {
          BinaryExpr *BE = cast<BinaryExpr>(RHS);
          // X - (C + Y) ==> -C + (X - Y)
          if (ConstantExpr *CE = dyn_cast<ConstantExpr>(BE->left))
            return Builder->Add(CE->Neg(), Builder->Sub(LHS, BE->right));
          
          // X - (Y + C) ==> -C + (X + Y)
          if (ConstantExpr *CE = dyn_cast<ConstantExpr>(BE->right))
            return Builder->Add(CE->Neg(), Builder->Sub(LHS, BE->left));
          break;
        }

        case Expr::Sub: {
          BinaryExpr *BE = cast<BinaryExpr>(RHS);
          // X - (C - Y) ==> -C + (X + Y)
          if (ConstantExpr *CE = dyn_cast<ConstantExpr>(BE->left)) 
            return Builder->Add(CE->Neg(), Builder->Add(LHS, BE->right));
          // X - (Y - C) ==> C + (X - Y)
          if (ConstantExpr *CE = dyn_cast<ConstantExpr>(BE->right))
            return Builder->Add(CE, Builder->Sub(LHS, BE->left));
          break;
        }

        case Expr::Mul: {
          BinaryExpr *BE = cast<BinaryExpr>(RHS);

          if (ConstantExpr *CE = dyn_cast<ConstantExpr>(BE->left))
            if (exactMatch(BE->right.get(), LHS.get()))
              // X - (C * X) => (1 - C) * X
              return record_opt(Builder->Mul(ConstantExpr::create(1, CE->getWidth())->Sub(CE), LHS));
          break;
        }

        case Expr::And: {
          BinaryExpr *BE = cast<BinaryExpr>(RHS);

          // X - (X & Y) => X & ~Y
          if (ref<Expr> otherChild = matchEitherChild(BE, LHS))
            return record_opt(Builder->And(LHS, Builder->Not(otherChild)));
          break;
        }
      }

      return Base->Sub(LHS, RHS);
    }

    ref<Expr> Mul(const ref<ConstantExpr> &LHS,
                  const ref<NonConstantExpr> &RHS) {
      if (LHS->isZero())
        // 0 * X => 0
        return record_opt(LHS);
      if (LHS->isOne())
        // 1 * X => X
        return record_opt(RHS);
      // FIXME: Unbalance nested muls, fold constants through
      // {sub,add}-with-constant, etc.
      return Base->Mul(LHS, RHS);
    }

    ref<Expr> Mul(const ref<NonConstantExpr> &LHS,
                  const ref<ConstantExpr> &RHS) {
      return Mul(RHS, LHS);
    }

    ref<Expr> Mul(const ref<NonConstantExpr> &LHS,
                  const ref<NonConstantExpr> &RHS) {
      return Base->Mul(LHS, RHS);
    }

    ref<Expr> UDiv (const ref<NonConstantExpr> &LHS,
                  const ref<ConstantExpr> &RHS) {
      if (RHS->isOne()) {
        // X / 1 => 1
        return record_opt(LHS);
      }
      return Base->UDiv(LHS, RHS);
    }

    ref<Expr> UDiv (const ref<ConstantExpr> &LHS,
                  const ref<NonConstantExpr> &RHS) {
      if (LHS->isZero()) {
        // 0 / X => 0
        return record_opt(LHS);
      }

      return Base->UDiv(LHS, RHS);
    }

    ref<Expr> UDiv(const ref<NonConstantExpr> &LHS,
                  const ref<NonConstantExpr> &RHS) {
      return Base->UDiv(LHS, RHS);
    }

    ref<Expr> SDiv(const ref<NonConstantExpr> &LHS,
                  const ref<ConstantExpr> &RHS) {
      if (RHS->isOne()) {
        // X / 1 => X
        return record_opt(LHS);
      }
      return Base->SDiv(LHS, RHS);
    }

    ref<Expr> SDiv(const ref<ConstantExpr> &LHS,
                  const ref<NonConstantExpr> &RHS) {
      
      if (LHS->isZero()) {
        // 0 / X => 0
        return record_opt(LHS);
      }
      
      return Base->SDiv(LHS, RHS);
    }

    ref<Expr> SDiv(const ref<NonConstantExpr> &LHS,
                  const ref<NonConstantExpr> &RHS) {
      return Base->SDiv(LHS, RHS);
    }

    ref<Expr> URem(const ref<NonConstantExpr> &LHS,
                  const ref<ConstantExpr> &RHS) {
      if (RHS->isOne()) {
        // X % 1 => 0
        return record_const_opt(Builder->Zero(RHS->getWidth()));
      }

      return Base->URem(LHS, RHS);
    }

    ref<Expr> URem(const ref<ConstantExpr> &LHS,
                  const ref<NonConstantExpr> &RHS) {
      if (LHS->isZero()) {
        // 0 % X => 0
        return record_opt(LHS);
      }

      return Base->URem(LHS, RHS);
    }

    ref<Expr> URem(const ref<NonConstantExpr> &LHS,
                  const ref<NonConstantExpr> &RHS) {
      return Base->URem(LHS, RHS);
    }

    ref<Expr> SRem(const ref<NonConstantExpr> &LHS,
                  const ref<ConstantExpr> &RHS) {
      if (RHS->isOne()) {
        // X % 1 => 0
        return record_const_opt(Builder->Zero(LHS->getWidth()));
      }

      return Base->SRem(LHS, RHS);
    }

    ref<Expr> SRem(const ref<ConstantExpr> &LHS,
                  const ref<NonConstantExpr> &RHS) {
      
      if (LHS->isZero()) {
        // 0 % X => 0
        return record_opt(LHS);
      }

      return Base->SRem(LHS, RHS);
    }

    ref<Expr> SRem(const ref<NonConstantExpr> &LHS,
                  const ref<NonConstantExpr> &RHS) {
      return Base->SRem(LHS, RHS);
    }


    ref<Expr> And(const ref<ConstantExpr> &LHS,
                  const ref<NonConstantExpr> &RHS) {
      if (LHS->isZero())
        // 0 & X => 0
        return record_opt(LHS);
      if (LHS->isAllOnes())
        // -1 & X => X 
        return record_opt(RHS);
      // FIXME: Unbalance nested ands, fold constants through
      // {and,or}-with-constant, etc.
      return Base->And(LHS, RHS);
    }

    ref<Expr> And(const ref<NonConstantExpr> &LHS,
                  const ref<ConstantExpr> &RHS) {
      return And(RHS, LHS);
    }

    ref<Expr> And(const ref<NonConstantExpr> &LHS,
                  const ref<NonConstantExpr> &RHS) {

      if (exactMatch(LHS.get(), RHS.get()))
        // X & X => X
        return record_opt(LHS);

      switch (LHS->getKind()) {
        default: break;

        case Expr::Or: {
          BinaryExpr *BE = cast<BinaryExpr>(LHS);

          if (matchEitherChild(BE, RHS))
            // (X | Y) & X => X
            return record_opt(RHS);
          break;
        }

        case Expr::Not: {
          NotExpr *NE = cast<NotExpr>(LHS);
          // ~X & X => 0
          if (matchNegated(NE, RHS)) {
            return record_const_opt(Builder->Zero(RHS->getWidth()));
          }
          break;
        }
      }

      switch (RHS->getKind()) {
        default: break;

        case Expr::Or: {
          BinaryExpr *BE = cast<BinaryExpr>(RHS);

          if(matchEitherChild(BE, LHS))
            // X & (Y | X) => X
            return record_opt(LHS);
          break;
        }
        case Expr::Not: {
          NotExpr *NE = cast<NotExpr>(RHS);
          // X & ~X => 0
          if (matchNegated(NE, LHS)) {
            return record_const_opt(Builder->Zero(LHS->getWidth()));
          }
          break;
        }
      }
      return Base->And(LHS, RHS);
    }

    ref<Expr> Or(const ref<ConstantExpr> &LHS,
                  const ref<NonConstantExpr> &RHS) {
      if (LHS->isZero())
        // 0 | X => X
        return record_opt(RHS);
      if (LHS->isAllOnes())
        // -1 | X => -1
        return record_opt(LHS);
      // FIXME: Unbalance nested ors, fold constants through
      // {and,or}-with-constant, etc.
      return Base->Or(LHS, RHS);
    }

    ref<Expr> Or(const ref<NonConstantExpr> &LHS,
                 const ref<ConstantExpr> &RHS) {
      return Or(RHS, LHS);
    }

    ref<Expr> Or(const ref<NonConstantExpr> &LHS,
                 const ref<NonConstantExpr> &RHS) {

      if (exactMatch(LHS.get(), RHS.get()))
        // X | X => X
        return record_opt(LHS);
      
      switch (LHS->getKind()) {
        default: break;

        case Expr::And: {
          BinaryExpr *BE = cast<BinaryExpr>(LHS);

          if(matchEitherChild(BE, RHS))
            // (X & Y) | X => X
            // (Y & X) | X => X
            return record_opt(RHS);
          break;
        }

        case Expr::Not: {
          NotExpr *NE = cast<NotExpr>(LHS);
          // ~X | X ==> -1
          if (matchNegated(NE, RHS)) {
            return record_const_opt(Builder->AllOnes(RHS->getWidth()));
          };

          switch(NE->expr->getKind()) {
            default: break;

            case Expr::And: {
              BinaryExpr *NBE = cast<BinaryExpr>(NE->expr);

              //  ~(X & Y) | X => -1; ~(Y & X) | X => -1
              if (matchEitherChild(NBE, RHS)) {
                return record_const_opt(Builder->AllOnes(RHS->getWidth()));
              }
              break;
            }

            case Expr::Xor: {
              BinaryExpr *NBE = cast<BinaryExpr>(NE->expr);

              if (RHS->getKind() == Expr::Or) {
                BinaryExpr *RBE = cast<BinaryExpr>(RHS);

                if (matchBinaryExprsChildren(NBE, RBE))
                  // ~(A ^ B) | (A | B) => allOnes and commut.
                  return record_const_opt(Builder->AllOnes(LHS->getWidth()));
              }

              break;
            }
          }
          break;
        }

        case Expr::Xor: {
          BinaryExpr *LBE = cast<BinaryExpr>(LHS);

          if (RHS->getKind() == Expr::Or) {
            BinaryExpr *RBE = cast<BinaryExpr>(RHS);

            if (matchBinaryExprsChildren(LBE, RBE))
               // (A ^ B) | (A | B) => (A | B) and commut.
               return record_opt(RHS);
          }
          break;
        }

        case Expr::Or: {
          BinaryExpr *LBE = cast<BinaryExpr>(LHS);

          if (RHS->getKind() == Expr::Xor) {
            BinaryExpr * RBE = cast<BinaryExpr>(RHS);

            if (matchBinaryExprsChildren(LBE, RBE))
               // (A | B) | (A ^ B) => (A | B) and commut.
               return record_opt(LHS);
          }

          if (RHS->getKind() == Expr::Not) {
            NotExpr *NBE = cast<NotExpr>(RHS);

            if (NBE->expr->getKind() == Expr::Xor) {
            BinaryExpr *RBE = cast<BinaryExpr>(NBE->expr);

            if (matchBinaryExprsChildren(LBE, RBE))
               // (A | B) | ~(A ^ B) => allOnes and commut.
               return record_const_opt(Builder->AllOnes(LHS->getWidth()));
            }
          }
          break;
        }
      }

      switch (RHS->getKind()) {
        default: break;

        case Expr::And: {
          BinaryExpr *BE = cast<BinaryExpr>(RHS);

          if(matchEitherChild(BE, LHS))
            // X | (X & Y) => X
            return record_opt(LHS);
          break;
        }

        case Expr::Not: {
          NotExpr *NE = cast<NotExpr>(RHS);
          // ~X | X ==> allOnes
          if (matchNegated(NE, LHS)) {
            return record_const_opt(Builder->AllOnes(LHS->getWidth()));
          }

          switch(NE->expr->getKind()) {
            default: break;

            case Expr::And:{
              BinaryExpr *NBE = cast<BinaryExpr>(NE->expr);

              // X | ~(X & Y) => -1; X | ~(Y & X) => allOnes
              if (matchEitherChild(NBE, LHS)) {
                return record_const_opt(Builder->AllOnes(LHS->getWidth()));
              }
            }
          }
          break;
        }        
      }

      return Base->Or(LHS, RHS);
    }

    ref<Expr> Xor(const ref<ConstantExpr> &LHS,
                  const ref<NonConstantExpr> &RHS) {
      if (LHS->isZero())
        // 0 ^ X => X
        return record_opt(RHS);
      // FIXME: Unbalance nested ors, fold constants through
      // {and,or}-with-constant, etc.
      return Base->Xor(LHS, RHS);
    }

    ref<Expr> Xor(const ref<NonConstantExpr> &LHS,
                  const ref<ConstantExpr> &RHS) {
      return Xor(RHS, LHS);
    }

    ref<Expr> Xor(const ref<NonConstantExpr> &LHS,
                  const ref<NonConstantExpr> &RHS) {
      
      if (exactMatch(LHS.get(), RHS.get()))
        // X ^ X => 0
        return record_const_opt(Builder->Zero(LHS->getWidth()));

      switch (LHS->getKind()) {
        default: break;

        case Expr::Not: {
          NotExpr * NE = cast<NotExpr>(LHS);
          
          if (matchNegated(NE, RHS)) 
            // X ^ ~X => -1
            return record_const_opt(Builder->AllOnes(RHS->getWidth()));
          break;
        }
      }

      switch (RHS->getKind()) {
        default: break;

        case Expr::Not: {
          NotExpr * NE = cast<NotExpr>(RHS);
          
          if (matchNegated(NE, LHS)) 
            // X ^ ~X => -1
            return record_const_opt(Builder->AllOnes(LHS->getWidth()));
          break;
        }
      }

      return Base->Xor(LHS, RHS);
    }

    ref<Expr> Shl(const ref<NonConstantExpr> &LHS, 
                 const ref<ConstantExpr> &RHS) {
      if (RHS->isZero()) {
        // X <<  0 => X
        return record_opt(LHS);
      }
      return Base->Shl(LHS, RHS);
    }

    ref<Expr> Shl(const ref<ConstantExpr> &LHS, 
                 const ref<NonConstantExpr> &RHS) {
      if (LHS->isZero() || LHS->isAllOnes()) 
        // 0 << X => 0
        // allOnes << X => allOnes
        return record_opt(LHS);

      return Base->Shl(LHS, RHS);
    }

    ref<Expr> Shl(const ref<NonConstantExpr> &LHS, 
                 const ref<NonConstantExpr> &RHS) {
      return Base->Shl(LHS, RHS);
    }

    ref<Expr> LShr(const ref<NonConstantExpr> &LHS, 
                 const ref<ConstantExpr> &RHS) {
      if (RHS->isZero()) {
        // X >> 0 => X
        return record_opt(LHS);
      }
      return Base->LShr(LHS, RHS);
    }

    ref<Expr> LShr(const ref<ConstantExpr> &LHS, 
                 const ref<NonConstantExpr> &RHS) {
      if (LHS->isZero() || LHS->isAllOnes()) 
        // 0 >> X => 0
        // allOnes >> X => allOnes
        return record_opt(LHS);

      return Base->LShr(LHS, RHS);
    }

    ref<Expr> LShr(const ref<NonConstantExpr> &LHS, 
                 const ref<NonConstantExpr> &RHS) {
      return Base->LShr(LHS, RHS);
    }

    ref<Expr> AShr(const ref<NonConstantExpr> &LHS, 
                 const ref<ConstantExpr> &RHS) {
      if (RHS->isZero()) {
        // X >> 0 => X
        return record_opt(LHS);
      }
      return Base->AShr(LHS, RHS);
    }

    ref<Expr> AShr(const ref<ConstantExpr> &LHS, 
                 const ref<NonConstantExpr> &RHS) {
      if (LHS->isZero() || LHS->isAllOnes()) 
        // 0 >> X => 0
        // allOnes >> X => allOnes
        return record_opt(LHS);

      return Base->AShr(LHS, RHS);
    }

    ref<Expr> AShr(const ref<NonConstantExpr> &LHS, 
                 const ref<NonConstantExpr> &RHS) {
      return Base->AShr(LHS, RHS);
    }

    ref<Expr> Eq(const ref<ConstantExpr> &LHS, 
                 const ref<NonConstantExpr> &RHS) {
      Expr::Width Width = LHS->getWidth();
      
      if (Width == Expr::Bool) {
        // true == X ==> X
        if (LHS->isTrue())
          return record_opt(RHS);

        // false == ... (not)
	      return record_opt(Base->Not(RHS));
      }

      switch (RHS->getKind()) {
        default: break;

        //TODO: Copy to Ult, Ule, Slt, Sle (different result in else case) Simplify:3867
        case Expr::ZExt: {
          ZExtExpr *ZE = cast<ZExtExpr>(RHS);

          if (checkConstantZExtRange(LHS, ZE))
            // ZExt X == C => X == ZExt C if types agree
            return record_opt(Builder->Eq(LHS->ZExt(ZE->src->getWidth()), ZE->src));

          // There is a bit set in the constant inside the ZExt range, so can't be equal
          else return record_const_opt(Builder->False());
        }

        
        //TODO: Copy to Ult, Ule, Slt, Sle (different result in else case) Simplify:3921
        case Expr::SExt: {
          SExtExpr *ZE = cast<SExtExpr>(RHS);

          if (checkConstantSExtRange(LHS, ZE))
            // SExt X == C => X == SExt C if types agree
            return record_opt(Builder->Eq(LHS->SExt(ZE->src->getWidth()), ZE->src));

          // There is a bit set in the constant inside the SExt range, so can't be equal
          else return record_const_opt(Builder->False());
        }
      }

      return Base->Eq(LHS, RHS);
    }

    ref<Expr> Eq(const ref<NonConstantExpr> &LHS, 
                 const ref<ConstantExpr> &RHS) {
      return Eq(RHS, LHS);
    }

    ref<Expr> Eq(const ref<NonConstantExpr> &LHS, 
                 const ref<NonConstantExpr> &RHS) {

      if (exactMatch(LHS.get(), RHS.get())) {
        // X == X => true
        return record_const_opt(Builder->True());
      }


      switch (LHS->getKind()) {
        default: break;

        case Expr::Add: {
          BinaryExpr *BE = cast<BinaryExpr>(LHS);

          if (ConstantExpr *CE = dyn_cast<ConstantExpr>(BE->left)) {
            if (exactMatch(BE->right.get(), RHS.get()))
              // C + X == X => C.isZero()
              return record_const_opt(Builder->Constant(CE->isZero(), Expr::Bool));
          }
          break;
        }

        case Expr::Sub: {
          BinaryExpr *BE = cast<BinaryExpr>(LHS);

          if (ConstantExpr *CE = dyn_cast<ConstantExpr>(BE->left)) {
            if (exactMatch(BE->right.get(), RHS.get()))
              // C - X == X => C.isZero()
              return record_const_opt(Builder->Constant(CE->isZero(), Expr::Bool));
          }
          break;
        }

        case Expr::URem: {
          BinaryExpr *BE = cast<BinaryExpr>(LHS);

          if (matchRightChild(BE, RHS))
            // (X URem Y) == Y => false
            return record_const_opt(Builder->False());
          break;
        }

        case Expr::ZExt: {
          // (ZExt X) == (ZExt Y) => X == Y if same width
          ZExtExpr *LE = dyn_cast<ZExtExpr>(LHS);
          if (ZExtExpr *RE = dyn_cast<ZExtExpr>(RHS))
            if (LE->src->getWidth() == RE->src->getWidth())
              return record_opt(Builder->Eq(LE->src, RE->src));
          break;
        }

        case Expr::SExt: {
          // (SExt X) == (SExt Y) => X == Y if same width
          SExtExpr *LE = dyn_cast<SExtExpr>(LHS);
          if (SExtExpr *RE = dyn_cast<SExtExpr>(RHS))
            if (LE->src->getWidth() == RE->src->getWidth())
              return record_opt(Builder->Eq(LE->src, RE->src));
          break;
        }
      }

      switch (RHS->getKind()) {
        default: break;

        case Expr::Add: {
          BinaryExpr *BE = cast<BinaryExpr>(RHS);

          if (ConstantExpr *CE = dyn_cast<ConstantExpr>(BE->left)) {
            if (exactMatch(BE->right.get(), LHS.get()))
              // X == C + X => C.isZero()
              return record_const_opt(Builder->Constant(CE->isZero(), Expr::Bool));
          }
          break;
        }

        case Expr::Sub: {
          BinaryExpr *BE = cast<BinaryExpr>(RHS);

          if (ConstantExpr *CE = dyn_cast<ConstantExpr>(BE->left)) {
            if (exactMatch(BE->right.get(), LHS.get()))
              // X == C - X => C.isZero()
              return record_const_opt(Builder->Constant(CE->isZero(), Expr::Bool));
          }
        }
        break;
      }

      // TODO: make helper and call for Ult and Ule also
      // (X + Z) == (Y + Z) => X == Y
      if (LHS->getKind() == Expr::Add &&
          RHS->getKind() >= Expr::Add) {
        BinaryExpr *LBE = cast<BinaryExpr>(LHS); 
        BinaryExpr *RBE = cast<BinaryExpr>(RHS);

        if (exactMatch(LBE->left.get(), RBE->left.get()))
          return record_opt(Builder->Eq(LBE->right, RBE->right));

        if (exactMatch(LBE->right.get(), RBE->right.get()))
          return record_opt(Builder->Eq(LBE->left, RBE->left));

        if (exactMatch(LBE->left.get(), RBE->right.get()))
          return record_opt(Builder->Eq(LBE->right, RBE->left));

        if (exactMatch(LBE->right.get(), RBE->left.get()))
          return record_opt(Builder->Eq(LBE->left, RBE->right));
      }

      return Base->Eq(LHS, RHS);
    }

    ref<Expr> Ult(const ref<ConstantExpr> &LHS, 
                 const ref<NonConstantExpr> &RHS) {
      return Base->Ult(LHS, RHS);
    }

    ref<Expr> Ult(const ref<NonConstantExpr> &LHS, 
                 const ref<ConstantExpr> &RHS) {

      // X < 0 ==> false
      if (RHS->isZero())
        return record_const_opt(Builder->False());

      return Base->Ult(LHS, RHS);
    }

    ref<Expr> Ult(const ref<NonConstantExpr> &LHS, 
                 const ref<NonConstantExpr> &RHS) {
      
      if (exactMatch(LHS.get(), RHS.get())) {
        // X < X => false
        return record_const_opt(Builder->False());
      }
      switch (LHS->getKind()) {
        default: break;

        case Expr::Or: {
          BinaryExpr *BE = cast<BinaryExpr>(LHS);


          if (matchEitherChild(BE, RHS))
            // (X | Y) <u X => false
            return record_opt(Base->False());
          break;
        }

        case Expr::URem: {
          BinaryExpr *BE = cast<BinaryExpr>(LHS);

          if (matchRightChild(BE, RHS)) 
            // (X URem Y) <u Y = true
            return record_const_opt(Builder->True());
          break;    
        }

        case Expr::ZExt: {
          // (ZExt X) < (ZExt Y) => X < Y if same width
          ZExtExpr *LE = dyn_cast<ZExtExpr>(LHS);
          if (ZExtExpr *RE = dyn_cast<ZExtExpr>(RHS))
            if (LE->src->getWidth() == RE->src->getWidth())
              return record_opt(Builder->Ult(LE->src, RE->src));
          break;
        }

        case Expr::SExt: {
          SExtExpr *LE = dyn_cast<SExtExpr>(LHS);

          // (SExt X) < (SExt Y) => X < Y if same width
          if (SExtExpr *RE = dyn_cast<SExtExpr>(RHS))
            if (LE->src->getWidth() == RE->src->getWidth())
              return record_opt(Builder->Ult(LE->src, RE->src));

          // (SExt X) < (ZExt X) => false
          if (ZExtExpr *RE = dyn_cast<ZExtExpr>(RHS))
            if (exactMatch(LE->src.get(), RE->src.get()))
              return record_const_opt(Builder->False());
          break;
        }
      }

      switch (RHS->getKind()) {
        default: break;

        case Expr::And: {
          BinaryExpr *BE = cast<BinaryExpr>(RHS);
          // X < (X & Y) => false
          if (matchEitherChild(BE, LHS))
            return record_const_opt(Builder->False());
          break;
        }

        case Expr::URem: {
          BinaryExpr *BE = cast<BinaryExpr>(RHS);

          if (matchEitherChild(BE, LHS)) 
            // X <u (X URem Y) = false
            // Y <u (X URem Y) = false
            return record_const_opt(Builder->False());
          break;
        }

        case Expr::UDiv: {
          BinaryExpr *BE = cast<BinaryExpr>(RHS);

          if (matchLeftChild(BE, LHS))
            // X < (X UDiv Y) -> false
            return record_const_opt(Builder->False());
          break;
        }
      }

      return Base->Ult(LHS, RHS);
    }

    ref<Expr> Ule(const ref<ConstantExpr> &LHS, 
                 const ref<NonConstantExpr> &RHS) {
      return Base->Ule(LHS, RHS);
    }

    ref<Expr> Ule(const ref<NonConstantExpr> &LHS, 
                 const ref<ConstantExpr> &RHS) {
      return Base->Ule(LHS, RHS);
    }

    ref<Expr> Ule(const ref<NonConstantExpr> &LHS, 
                 const ref<NonConstantExpr> &RHS) {
      
      if (exactMatch(LHS.get(), RHS.get())) {
        // X <= X => true
        return record_const_opt(Builder->True());
      }

      switch (LHS->getKind()) {
        default: break;

        case Expr::And: {
          BinaryExpr *BE = cast<BinaryExpr>(LHS);

          if (matchEitherChild(BE, RHS))
            // X <= (X | Y) => true
            return record_const_opt(Builder->True());
          break;
        }

        case Expr::URem: {
          BinaryExpr *BE = cast<BinaryExpr>(LHS);

          if (matchEitherChild(BE, RHS))
            // (X URem Y) <=u X = true
            // (X URem Y) <=u Y = true
            return record_const_opt(Builder->True());
          break;
        }

        case Expr::UDiv: {
          BinaryExpr *BE = cast<BinaryExpr>(LHS);

          if (matchLeftChild(BE, RHS)) 
            // (X UDiv Y) <=u X => true
            return record_const_opt(Builder->True());
          break;
        }

        case Expr::ZExt: {
          ZExtExpr *LE = dyn_cast<ZExtExpr>(LHS);

          // (ZExt X) <= (ZExt Y) => X <= Y if same width
          if (ZExtExpr *RE = dyn_cast<ZExtExpr>(RHS))
            if (LE->src->getWidth() == RE->src->getWidth())
              return record_opt(Builder->Ule(LE->src, RE->src));

          // (ZExt X) <= (SExt X) => true
          if (SExtExpr *RE = dyn_cast<SExtExpr>(RHS))
            if (exactMatch(LE->src.get(), RE->src.get()))
              return record_const_opt(Builder->True());
          break;
        }

        case Expr::SExt: {
          SExtExpr *LE = dyn_cast<SExtExpr>(LHS);

          // (ZExt X) <= (ZExt Y) => X <= Y if same width
          if (SExtExpr *RE = dyn_cast<SExtExpr>(RHS))
            if (LE->src->getWidth() == RE->src->getWidth())
              return record_opt(Builder->Ule(LE->src, RE->src));
          break;
        }
      }

      switch (RHS->getKind()) {
        default: break;

        case Expr::Or: {
          BinaryExpr *BE = cast<BinaryExpr>(RHS);

          // X <= (X | Y) => true
          return record_opt(Base->Constant((bool) matchEitherChild(BE, LHS), Expr::Bool));
        }

        case Expr::URem: {
          BinaryExpr *BE = cast<BinaryExpr>(RHS);

          if (matchRightChild(BE, LHS)) {
            // Y <=u (X URem Y) = false
            return record_const_opt(Builder->False());
          }
          break;
        }
      }
      
      return Base->Ule(LHS, RHS);
    }

    ref<Expr> Slt(const ref<ConstantExpr> &LHS, 
                 const ref<NonConstantExpr> &RHS) {
      return Base->Slt(LHS, RHS);
    }

    ref<Expr> Slt(const ref<NonConstantExpr> &LHS, 
                 const ref<ConstantExpr> &RHS) {
      return Base->Slt(LHS, RHS);
    }

    ref<Expr> Slt(const ref<NonConstantExpr> &LHS, 
                 const ref<NonConstantExpr> &RHS) {
      
      if (exactMatch(LHS.get(), RHS.get())) {
        // X <s X => false
        return record_const_opt(Builder->False());
      }

      switch (LHS->getKind()) {
        default: break;

        case Expr::ZExt: {
          // (ZExt X) < (ZExt Y) => X < Y if same width
          ZExtExpr *LE = dyn_cast<ZExtExpr>(LHS);
          if (ZExtExpr *RE = dyn_cast<ZExtExpr>(RHS))
            if (LE->src->getWidth() == RE->src->getWidth())
              return record_opt(Builder->Slt(LE->src, RE->src));
          break;
        }

        case Expr::SExt: {
          SExtExpr *LE = dyn_cast<SExtExpr>(LHS);

          // (SExt X) < (SExt Y) => X < Y if same width
          if (SExtExpr *RE = dyn_cast<SExtExpr>(RHS))
            if (LE->src->getWidth() == RE->src->getWidth())
              return record_opt(Builder->Slt(LE->src, RE->src));
          break;
        }
      }
      
      return Base->Slt(LHS, RHS);
    }

    ref<Expr> Sle(const ref<ConstantExpr> &LHS, 
                 const ref<NonConstantExpr> &RHS) {
      return Base->Sle(LHS, RHS);
    }

    ref<Expr> Sle(const ref<NonConstantExpr> &LHS, 
                 const ref<ConstantExpr> &RHS) {
      return Base->Sle(LHS, RHS);
    }

    ref<Expr> Sle(const ref<NonConstantExpr> &LHS, 
                 const ref<NonConstantExpr> &RHS) {
      
      if (exactMatch(LHS.get(), RHS.get())) {
        // X <=s X => true
        return record_const_opt(Builder->True());
      }

      switch (LHS->getKind()) {
        default: break;

        case Expr::ZExt: {
          ZExtExpr *LE = dyn_cast<ZExtExpr>(LHS);

          // (ZExt X) <= (ZExt Y) => X <= Y if same width
          if (ZExtExpr *RE = dyn_cast<ZExtExpr>(RHS))
            if (LE->src->getWidth() == RE->src->getWidth())
              return record_opt(Builder->Sle(LE->src, RE->src));

          // (ZExt X) <=s (SExt X) => false
          if (SExtExpr *RE = dyn_cast<SExtExpr>(RHS))
            if (exactMatch(LE->src.get(), RE->src.get()))
              return record_const_opt(Builder->False());
          break;
        }

        case Expr::SExt: {
          SExtExpr *LE = dyn_cast<SExtExpr>(LHS);

          // (SExt X) <=s (ZExt X) => true
          if (ZExtExpr *RE = dyn_cast<ZExtExpr>(RHS))
            if (exactMatch(LE->src.get(), RE->src.get()))
              return record_const_opt(Builder->True());

          // (SExt X) <= (SExt Y) => X <= Y if same width
          if (SExtExpr *RE = dyn_cast<SExtExpr>(RHS))
            if (LE->src->getWidth() == RE->src->getWidth())
              return record_opt(Builder->Sle(LE->src, RE->src));
          break;
        }
      }
      
      return Base->Sle(LHS, RHS);
    }

    ref<Expr> Select(const ref<Expr> &Cond, 
                     const ref<Expr> &LHS, 
                     const ref<Expr> &RHS) {
        
      if (LHS.get(), RHS.get()) {
        // Select Y X X => X
        return record_opt(LHS);
      }
      return Base->Select(Cond, LHS, RHS);
    }
  };

  typedef ConstantSpecializedExprBuilder<ConstantFoldingBuilder>
    ConstantFoldingExprBuilder;

  class SimplifyingBuilder : public ChainedBuilder {
  public:
    SimplifyingBuilder(ExprBuilder *Builder, ExprBuilder *Base)
      : ChainedBuilder(Builder, Base) {}

    ref<Expr> Eq(const ref<ConstantExpr> &LHS, 
                 const ref<NonConstantExpr> &RHS) {
      Expr::Width Width = LHS->getWidth();
      
      if (Width == Expr::Bool) {
        // true == X ==> X
        if (LHS->isTrue())
          return RHS;

        // false == X (not)
	      return Base->Not(RHS);
      }

      return Base->Eq(LHS, RHS);
    }

    ref<Expr> Eq(const ref<NonConstantExpr> &LHS, 
                 const ref<ConstantExpr> &RHS) {
      return Eq(RHS, LHS);
    }

    ref<Expr> Eq(const ref<NonConstantExpr> &LHS, 
                 const ref<NonConstantExpr> &RHS) {
      // X == X ==> true
      if (LHS == RHS)
          return Builder->True();
        
      

      return Base->Eq(LHS, RHS);
    }

    ref<Expr> Not(const ref<NonConstantExpr> &LHS) {
      // Transform !(a or b) ==> !a and !b.
      if (const OrExpr *OE = dyn_cast<OrExpr>(LHS))
	        return Builder->And(Builder->Not(OE->left),
			                        Builder->Not(OE->right));
      return Base->Not(LHS);
    }

    ref<Expr> Ne(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      // X != Y ==> !(X == Y)
      return Builder->Not(Builder->Eq(LHS, RHS));
    }

    ref<Expr> Ugt(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      // X u> Y ==> Y u< X
      return Builder->Ult(RHS, LHS);
    }

    ref<Expr> Uge(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      // X u>= Y ==> Y u<= X
      return Builder->Ule(RHS, LHS);
    }

    ref<Expr> Sgt(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      // X s> Y ==> Y s< X
      return Builder->Slt(RHS, LHS);
    }

    ref<Expr> Sge(const ref<Expr> &LHS, const ref<Expr> &RHS) {
      // X s>= Y ==> Y s<= X
      return Builder->Sle(RHS, LHS);
    }
  };

  typedef ConstantSpecializedExprBuilder<SimplifyingBuilder>
    SimplifyingExprBuilder;
}

ExprBuilder *klee::createDefaultExprBuilder() {
  return new DefaultExprBuilder();
}

ExprBuilder *klee::createConstantFoldingExprBuilder(ExprBuilder *Base) {
  return new ConstantFoldingExprBuilder(Base);
}

ExprBuilder *klee::createSimplifyingExprBuilder(ExprBuilder *Base) {
  return new SimplifyingExprBuilder(Base);
}

