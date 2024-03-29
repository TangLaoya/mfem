// Copyright (c) 2010, Lawrence Livermore National Security, LLC. Produced at
// the Lawrence Livermore National Laboratory. LLNL-CODE-443211. All Rights
// reserved. See file COPYRIGHT for details.
//
// This file is part of the MFEM library. For more information and source code
// availability see http://mfem.googlecode.com.
//
// MFEM is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License (as published by the Free
// Software Foundation) version 2.1 dated February 1999.

// Implementation of Bilinear Form Integrators

#include "fem.hpp"
#include <cmath>
#include <algorithm>

using namespace std;

namespace mfem
{

void BilinearFormIntegrator::AssembleElementMatrix (
   const FiniteElement &el, ElementTransformation &Trans,
   DenseMatrix &elmat )
{
   mfem_error ("BilinearFormIntegrator::AssembleElementMatrix (...)\n"
               "   is not implemented fot this class.");
}

void BilinearFormIntegrator::AssembleElementMatrix2 (
   const FiniteElement &el1, const FiniteElement &el2,
   ElementTransformation &Trans, DenseMatrix &elmat )
{
   mfem_error ("BilinearFormIntegrator::AssembleElementMatrix2 (...)\n"
               "   is not implemented fot this class.");
}

void BilinearFormIntegrator::AssembleFaceMatrix (
   const FiniteElement &el1, const FiniteElement &el2,
   FaceElementTransformations &Trans, DenseMatrix &elmat)
{
   mfem_error ("BilinearFormIntegrator::AssembleFaceMatrix (...)\n"
               "   is not implemented fot this class.");
}

void BilinearFormIntegrator::AssembleFaceMatrix(
   const FiniteElement &trial_face_fe, const FiniteElement &test_fe1,
   const FiniteElement &test_fe2, FaceElementTransformations &Trans,
   DenseMatrix &elmat)
{
   MFEM_ABORT("AssembleFaceMatrix (mixed form) is not implemented for this"
              " Integrator class.");
}

void BilinearFormIntegrator::AssembleElementVector(
   const FiniteElement &el, ElementTransformation &Tr, const Vector &elfun,
   Vector &elvect)
{
   mfem_error("BilinearFormIntegrator::AssembleElementVector\n"
              "   is not implemented fot this class.");
}


void TransposeIntegrator::AssembleElementMatrix (
   const FiniteElement &el, ElementTransformation &Trans, DenseMatrix &elmat)
{
   bfi -> AssembleElementMatrix (el, Trans, bfi_elmat);
   // elmat = bfi_elmat^t
   elmat.Transpose (bfi_elmat);
}

void TransposeIntegrator::AssembleElementMatrix2 (
   const FiniteElement &trial_fe, const FiniteElement &test_fe,
   ElementTransformation &Trans, DenseMatrix &elmat)
{
   bfi -> AssembleElementMatrix2 (test_fe, trial_fe, Trans, bfi_elmat);
   // elmat = bfi_elmat^t
   elmat.Transpose (bfi_elmat);
}

void TransposeIntegrator::AssembleFaceMatrix (
   const FiniteElement &el1, const FiniteElement &el2,
   FaceElementTransformations &Trans, DenseMatrix &elmat)
{
   bfi -> AssembleFaceMatrix (el1, el2, Trans, bfi_elmat);
   // elmat = bfi_elmat^t
   elmat.Transpose (bfi_elmat);
}

void LumpedIntegrator::AssembleElementMatrix (
   const FiniteElement &el, ElementTransformation &Trans, DenseMatrix &elmat)
{
   bfi -> AssembleElementMatrix (el, Trans, elmat);
   elmat.Lump();
}

void InverseIntegrator::AssembleElementMatrix(
   const FiniteElement &el, ElementTransformation &Trans, DenseMatrix &elmat)
{
   integrator->AssembleElementMatrix(el, Trans, elmat);
   elmat.Invert();
}

void SumIntegrator::AssembleElementMatrix(
   const FiniteElement &el, ElementTransformation &Trans, DenseMatrix &elmat)
{
   MFEM_ASSERT(integrators.Size() > 0, "empty SumIntegrator.");

   integrators[0]->AssembleElementMatrix(el, Trans, elmat);
   for (int i = 1; i < integrators.Size(); i++)
   {
      integrators[i]->AssembleElementMatrix(el, Trans, elem_mat);
      elmat += elem_mat;
   }
}

SumIntegrator::~SumIntegrator()
{
   if (own_integrators)
   {
      for (int i = 0; i < integrators.Size(); i++)
         delete integrators[i];
   }
}


void DiffusionIntegrator::AssembleElementMatrix
( const FiniteElement &el, ElementTransformation &Trans,
  DenseMatrix &elmat )
{
   int nd = el.GetDof();
   int dim = el.GetDim();
   int spaceDim = Trans.GetSpaceDim();
   bool square = (dim == spaceDim);
   double w;

#ifdef MFEM_THREAD_SAFE
   DenseMatrix dshape(nd,dim), dshapedxt(nd,spaceDim), invdfdx(dim,spaceDim);
#else
   dshape.SetSize(nd,dim);
   dshapedxt.SetSize(nd,spaceDim);
   invdfdx.SetSize(dim,spaceDim);
#endif
   elmat.SetSize(nd);

   const IntegrationRule *ir = IntRule;
   if (ir == NULL)
   {
      int order;
      if (el.Space() == FunctionSpace::Pk)
         order = 2*el.GetOrder() - 2;
      else
         // order = 2*el.GetOrder() - 2;  // <-- this seems to work fine too
         order = 2*el.GetOrder() + dim - 1;

      if (el.Space() == FunctionSpace::rQk)
         ir = &RefinedIntRules.Get(el.GetGeomType(), order);
      else
         ir = &IntRules.Get(el.GetGeomType(), order);
   }

   elmat = 0.0;
   for (int i = 0; i < ir->GetNPoints(); i++)
   {
      const IntegrationPoint &ip = ir->IntPoint(i);
      el.CalcDShape(ip, dshape);

      Trans.SetIntPoint(&ip);
      // Compute invdfdx = / adj(J),         if J is square
      //                   \ adj(J^t.J).J^t, otherwise
      CalcAdjugate(Trans.Jacobian(), invdfdx);
      w = Trans.Weight();
      w = ip.weight / (square ? w : w*w*w);
      Mult(dshape, invdfdx, dshapedxt);
      if (!MQ)
      {
         if (Q)
            w *= Q->Eval(Trans, ip);
         AddMult_a_AAt(w, dshapedxt, elmat);
      }
      else
      {
         MQ->Eval(invdfdx, Trans, ip);
         invdfdx *= w;
         Mult(dshapedxt, invdfdx, dshape);
         AddMultABt(dshape, dshapedxt, elmat);
      }
   }
}

void DiffusionIntegrator::AssembleElementMatrix2(
   const FiniteElement &trial_fe, const FiniteElement &test_fe,
   ElementTransformation &Trans, DenseMatrix &elmat)
{
   int tr_nd = trial_fe.GetDof();
   int te_nd = test_fe.GetDof();
   int dim = trial_fe.GetDim();
   int spaceDim = Trans.GetSpaceDim();
   bool square = (dim == spaceDim);
   double w;

#ifdef MFEM_THREAD_SAFE
   DenseMatrix dshape(tr_nd, dim), dshapedxt(tr_nd, spaceDim);
   DenseMatrix te_dshape(te_nd, dim), te_dshapedxt(te_nd, spaceDim);
   DenseMatrix invdfdx(dim, spaceDim);
#else
   dshape.SetSize(tr_nd, dim);
   dshapedxt.SetSize(tr_nd, spaceDim);
   te_dshape.SetSize(te_nd, dim);
   te_dshapedxt.SetSize(te_nd, spaceDim);
   invdfdx.SetSize(dim, spaceDim);
#endif
   elmat.SetSize(te_nd, tr_nd);

   const IntegrationRule *ir = IntRule;
   if (ir == NULL)
   {
      int order;
      if (trial_fe.Space() == FunctionSpace::Pk)
         order = trial_fe.GetOrder() + test_fe.GetOrder() - 2;
      else
         order = trial_fe.GetOrder() + test_fe.GetOrder() + dim - 1;

      if (trial_fe.Space() == FunctionSpace::rQk)
         ir = &RefinedIntRules.Get(trial_fe.GetGeomType(), order);
      else
         ir = &IntRules.Get(trial_fe.GetGeomType(), order);
   }

   elmat = 0.0;
   for (int i = 0; i < ir->GetNPoints(); i++)
   {
      const IntegrationPoint &ip = ir->IntPoint(i);
      trial_fe.CalcDShape(ip, dshape);
      test_fe.CalcDShape(ip, te_dshape);

      Trans.SetIntPoint(&ip);
      CalcAdjugate(Trans.Jacobian(), invdfdx);
      w = Trans.Weight();
      w = ip.weight / (square ? w : w*w*w);
      Mult(dshape, invdfdx, dshapedxt);
      Mult(te_dshape, invdfdx, te_dshapedxt);
      // invdfdx, dshape, and te_dshape no longer needed
      if (!MQ)
      {
         if (Q)
            w *= Q->Eval(Trans, ip);
         dshapedxt *= w;
         AddMultABt(te_dshapedxt, dshapedxt, elmat);
      }
      else
      {
         MQ->Eval(invdfdx, Trans, ip);
         invdfdx *= w;
         Mult(te_dshapedxt, invdfdx, te_dshape);
         AddMultABt(te_dshape, dshapedxt, elmat);
      }
   }
}

void DiffusionIntegrator::AssembleElementVector(
   const FiniteElement &el, ElementTransformation &Tr, const Vector &elfun,
   Vector &elvect)
{
   int nd = el.GetDof();
   int dim = el.GetDim();
   double w;

#ifdef MFEM_THREAD_SAFE
   DenseMatrix dshape(nd,dim), invdfdx(dim), mq(dim);
#else
   dshape.SetSize(nd,dim);
   invdfdx.SetSize(dim);
   mq.SetSize(dim);
#endif
   vec.SetSize(dim);
   pointflux.SetSize(dim);

   elvect.SetSize(nd);

   const IntegrationRule *ir = IntRule;
   if (ir == NULL)
   {
      int order;
      if (el.Space() == FunctionSpace::Pk)
         order = 2*el.GetOrder() - 2;
      else
         // order = 2*el.GetOrder() - 2;  // <-- this seems to work fine too
         order = 2*el.GetOrder() + dim - 1;

      if (el.Space() == FunctionSpace::rQk)
         ir = &RefinedIntRules.Get(el.GetGeomType(), order);
      else
         ir = &IntRules.Get(el.GetGeomType(), order);
   }

   elvect = 0.0;
   for (int i = 0; i < ir->GetNPoints(); i++)
   {
      const IntegrationPoint &ip = ir->IntPoint(i);
      el.CalcDShape(ip, dshape);

      Tr.SetIntPoint(&ip);
      CalcAdjugate(Tr.Jacobian(), invdfdx); // invdfdx = adj(J)
      w = ip.weight / Tr.Weight();

      if (!MQ)
      {
         dshape.MultTranspose(elfun, vec);
         invdfdx.MultTranspose(vec, pointflux);
         if (Q)
            w *= Q->Eval(Tr, ip);
      }
      else
      {

         dshape.MultTranspose(elfun, pointflux);
         invdfdx.MultTranspose(pointflux, vec);
         MQ->Eval(mq, Tr, ip);
         mq.Mult(vec, pointflux);
      }
      pointflux *= w;
      invdfdx.Mult(pointflux, vec);
      dshape.AddMult(vec, elvect);
   }
}

void DiffusionIntegrator::ComputeElementFlux
( const FiniteElement &el, ElementTransformation &Trans,
  Vector &u, const FiniteElement &fluxelem, Vector &flux, int wcoef )
{
   int i, j, nd, dim, spaceDim, fnd;

   nd = el.GetDof();
   dim = el.GetDim();
   spaceDim = Trans.GetSpaceDim();

#ifdef MFEM_THREAD_SAFE
   DenseMatrix dshape(nd,dim), invdfdx(dim, spaceDim);
#else
   dshape.SetSize(nd,dim);
   invdfdx.SetSize(dim, spaceDim);
#endif
   vec.SetSize(dim);
   pointflux.SetSize(spaceDim);

   const IntegrationRule &ir = fluxelem.GetNodes();
   fnd = ir.GetNPoints();
   flux.SetSize( fnd * dim );

   for (i = 0; i < fnd; i++)
   {
      const IntegrationPoint &ip = ir.IntPoint(i);
      el.CalcDShape(ip, dshape);
      dshape.MultTranspose(u, vec);

      Trans.SetIntPoint (&ip);
      CalcInverse(Trans.Jacobian(), invdfdx);
      invdfdx.MultTranspose(vec, pointflux);

      if (!wcoef)
      {
         for (j = 0; j < dim; j++)
            flux(fnd*j+i) = pointflux(j);
      }
      else if (!MQ)
      {
         if (Q)
            pointflux *= Q->Eval(Trans,ip);
         for (j = 0; j < dim; j++)
            flux(fnd*j+i) = pointflux(j);
      }
      else
      {
         MQ->Eval(invdfdx, Trans, ip);
         invdfdx.Mult(pointflux, vec);
         for (j = 0; j < dim; j++)
            flux(fnd*j+i) = vec(j);
      }
   }
}

double DiffusionIntegrator::ComputeFluxEnergy
( const FiniteElement &fluxelem, ElementTransformation &Trans,
  Vector &flux)
{
   int i, j, k, nd, dim, order;
   double energy, co;

   nd = fluxelem.GetDof();
   dim = fluxelem.GetDim();

#ifdef MFEM_THREAD_SAFE
   DenseMatrix invdfdx;
#endif

   shape.SetSize(nd);
   pointflux.SetSize(dim);
   if (MQ)
   {
      invdfdx.SetSize(dim);
      vec.SetSize(dim);
   }

   order = 2 * fluxelem.GetOrder(); // <--
   const IntegrationRule *ir = &IntRules.Get(fluxelem.GetGeomType(), order);

   energy = 0.0;
   for (i = 0; i < ir->GetNPoints(); i++)
   {
      const IntegrationPoint &ip = ir->IntPoint(i);
      fluxelem.CalcShape(ip, shape);

      pointflux = 0.0;
      for (k = 0; k < dim; k++)
         for (j = 0; j < nd; j++)
            pointflux(k) += flux(k*nd+j)*shape(j);

      Trans.SetIntPoint (&ip);
      co = Trans.Weight() * ip.weight;

      if (!MQ)
      {
         co *= ( pointflux * pointflux );
         if (Q)
            co *= Q->Eval(Trans, ip);
      }
      else
      {
         MQ->Eval(invdfdx, Trans, ip);
         co *= invdfdx.InnerProduct(pointflux, pointflux);
      }

      energy += co;
   }

   return energy;
}


void MassIntegrator::AssembleElementMatrix
( const FiniteElement &el, ElementTransformation &Trans,
  DenseMatrix &elmat )
{
   int nd = el.GetDof();
   // int dim = el.GetDim();
   double w;

   elmat.SetSize(nd);
   shape.SetSize(nd);

   const IntegrationRule *ir = IntRule;
   if (ir == NULL)
   {
      // int order = 2 * el.GetOrder();
      int order = 2 * el.GetOrder() + Trans.OrderW();

      if (el.Space() == FunctionSpace::rQk)
         ir = &RefinedIntRules.Get(el.GetGeomType(), order);
      else
         ir = &IntRules.Get(el.GetGeomType(), order);
   }

   elmat = 0.0;
   for (int i = 0; i < ir->GetNPoints(); i++)
   {
      const IntegrationPoint &ip = ir->IntPoint(i);
      el.CalcShape(ip, shape);

      Trans.SetIntPoint (&ip);
      w = Trans.Weight() * ip.weight;
      if (Q)
         w *= Q -> Eval(Trans, ip);

      AddMult_a_VVt(w, shape, elmat);
   }
}

void MassIntegrator::AssembleElementMatrix2(
   const FiniteElement &trial_fe, const FiniteElement &test_fe,
   ElementTransformation &Trans, DenseMatrix &elmat)
{
   int tr_nd = trial_fe.GetDof();
   int te_nd = test_fe.GetDof();
   // int dim = trial_fe.GetDim();
   double w;

   elmat.SetSize (te_nd, tr_nd);
   shape.SetSize (tr_nd);
   te_shape.SetSize (te_nd);

   const IntegrationRule *ir = IntRule;
   if (ir == NULL)
   {
      int order = trial_fe.GetOrder() + test_fe.GetOrder() + Trans.OrderW();

      ir = &IntRules.Get(trial_fe.GetGeomType(), order);
   }

   elmat = 0.0;
   for (int i = 0; i < ir->GetNPoints(); i++)
   {
      const IntegrationPoint &ip = ir->IntPoint(i);
      trial_fe.CalcShape(ip, shape);
      test_fe.CalcShape(ip, te_shape);

      Trans.SetIntPoint (&ip);
      w = Trans.Weight() * ip.weight;
      if (Q)
         w *= Q -> Eval(Trans, ip);

      te_shape *= w;
      AddMultVWt(te_shape, shape, elmat);
   }
}


void ConvectionIntegrator::AssembleElementMatrix(
   const FiniteElement &el, ElementTransformation &Trans, DenseMatrix &elmat)
{
   int nd = el.GetDof();
   int dim = el.GetDim();

   elmat.SetSize(nd);
   dshape.SetSize(nd,dim);
   adjJ.SetSize(dim);
   shape.SetSize(nd);
   vec2.SetSize(dim);
   BdFidxT.SetSize(nd);

   Vector vec1;

   const IntegrationRule *ir = IntRule;
   if (ir == NULL)
   {
      int order = Trans.OrderGrad(&el) + Trans.Order() + el.GetOrder();
      ir = &IntRules.Get(el.GetGeomType(), order);
   }

   Q.Eval(Q_ir, Trans, *ir);

   elmat = 0.0;
   for (int i = 0; i < ir->GetNPoints(); i++)
   {
      const IntegrationPoint &ip = ir->IntPoint(i);
      el.CalcDShape(ip, dshape);
      el.CalcShape(ip, shape);

      Trans.SetIntPoint(&ip);
      CalcAdjugate(Trans.Jacobian(), adjJ);
      Q_ir.GetColumnReference(i, vec1);
      vec1 *= alpha * ip.weight;

      adjJ.Mult(vec1, vec2);
      dshape.Mult(vec2, BdFidxT);

      AddMultVWt(shape, BdFidxT, elmat);
   }
}


void GroupConvectionIntegrator::AssembleElementMatrix(
   const FiniteElement &el, ElementTransformation &Trans, DenseMatrix &elmat)
{
   int nd = el.GetDof();
   int dim = el.GetDim();

   elmat.SetSize(nd);
   dshape.SetSize(nd,dim);
   adjJ.SetSize(dim);
   shape.SetSize(nd);
   grad.SetSize(nd,dim);

   const IntegrationRule *ir = IntRule;
   if (ir == NULL)
   {
      int order = Trans.OrderGrad(&el) + el.GetOrder();
      ir = &IntRules.Get(el.GetGeomType(), order);
   }

   Q.Eval(Q_nodal, Trans, el.GetNodes()); // sets the size of Q_nodal

   elmat = 0.0;
   for (int i = 0; i < ir->GetNPoints(); i++)
   {
      const IntegrationPoint &ip = ir->IntPoint(i);
      el.CalcDShape(ip, dshape);
      el.CalcShape(ip, shape);

      Trans.SetIntPoint(&ip);
      CalcAdjugate(Trans.Jacobian(), adjJ);

      Mult(dshape, adjJ, grad);

      double w = alpha * ip.weight;

      // elmat(k,l) += \sum_s w*shape(k)*Q_nodal(s,k)*grad(l,s)
      for (int k = 0; k < nd; k++)
      {
         double wsk = w*shape(k);
         for (int l = 0; l < nd; l++)
         {
            double a = 0.0;
            for (int s = 0; s < dim; s++)
               a += Q_nodal(s,k)*grad(l,s);
            elmat(k,l) += wsk*a;
         }
      }
   }
}


void VectorMassIntegrator::AssembleElementMatrix
( const FiniteElement &el, ElementTransformation &Trans,
  DenseMatrix &elmat )
{
   int nd   = el.GetDof();
   int dim  = el.GetDim();
   int vdim;

   double norm;

   // Get vdim from the ElementTransformation Trans ?
   vdim = (VQ) ? (VQ -> GetVDim()) : ((MQ) ? (MQ -> GetVDim()) : (dim));

   elmat.SetSize(nd*vdim);
   shape.SetSize(nd);
   partelmat.SetSize(nd);
   if (VQ)
      vec.SetSize(vdim);
   else if (MQ)
      mcoeff.SetSize(vdim);

   const IntegrationRule *ir = IntRule;
   if (ir == NULL)
   {
      int order = 2 * el.GetOrder() + Trans.OrderW() + Q_order;

      if (el.Space() == FunctionSpace::rQk)
         ir = &RefinedIntRules.Get(el.GetGeomType(), order);
      else
         ir = &IntRules.Get(el.GetGeomType(), order);
   }

   elmat = 0.0;
   for (int s = 0; s < ir->GetNPoints(); s++)
   {
      const IntegrationPoint &ip = ir->IntPoint(s);
      el.CalcShape(ip, shape);

      Trans.SetIntPoint (&ip);
      norm = ip.weight * Trans.Weight();

      MultVVt(shape, partelmat);

      if (VQ)
      {
         VQ->Eval(vec, Trans, ip);
         for (int k = 0; k < vdim; k++)
            elmat.AddMatrix(norm*vec(k), partelmat, nd*k, nd*k);
      }
      else if (MQ)
      {
         MQ->Eval(mcoeff, Trans, ip);
         for (int i = 0; i < vdim; i++)
            for (int j = 0; j < vdim; j++)
               elmat.AddMatrix(norm*mcoeff(i,j), partelmat, nd*i, nd*j);
      }
      else
      {
         if (Q)
            norm *= Q->Eval(Trans, ip);
         partelmat *= norm;
         for (int k = 0; k < vdim; k++)
            elmat.AddMatrix(partelmat, nd*k, nd*k);
      }
   }
}

void VectorMassIntegrator::AssembleElementMatrix2(
   const FiniteElement &trial_fe, const FiniteElement &test_fe,
   ElementTransformation &Trans, DenseMatrix &elmat)
{
   int tr_nd = trial_fe.GetDof();
   int te_nd = test_fe.GetDof();
   int dim   = trial_fe.GetDim();
   int vdim;

   double norm;

   // Get vdim from the ElementTransformation Trans ?
   vdim = (VQ) ? (VQ -> GetVDim()) : ((MQ) ? (MQ -> GetVDim()) : (dim));

   elmat.SetSize(te_nd*vdim, tr_nd*vdim);
   shape.SetSize(tr_nd);
   te_shape.SetSize(te_nd);
   partelmat.SetSize(te_nd, tr_nd);
   if (VQ)
      vec.SetSize(vdim);
   else if (MQ)
      mcoeff.SetSize(vdim);

   const IntegrationRule *ir = IntRule;
   if (ir == NULL)
   {
      int order = (trial_fe.GetOrder() + test_fe.GetOrder() +
                   Trans.OrderW() + Q_order);

      if (trial_fe.Space() == FunctionSpace::rQk)
         ir = &RefinedIntRules.Get(trial_fe.GetGeomType(), order);
      else
         ir = &IntRules.Get(trial_fe.GetGeomType(), order);
   }

   elmat = 0.0;
   for (int s = 0; s < ir->GetNPoints(); s++)
   {
      const IntegrationPoint &ip = ir->IntPoint(s);
      trial_fe.CalcShape(ip, shape);
      test_fe.CalcShape(ip, te_shape);

      Trans.SetIntPoint(&ip);
      norm = ip.weight * Trans.Weight();

      MultVWt(te_shape, shape, partelmat);

      if (VQ)
      {
         VQ->Eval(vec, Trans, ip);
         for (int k = 0; k < vdim; k++)
            elmat.AddMatrix(norm*vec(k), partelmat, te_nd*k, tr_nd*k);
      }
      else if (MQ)
      {
         MQ->Eval(mcoeff, Trans, ip);
         for (int i = 0; i < vdim; i++)
            for (int j = 0; j < vdim; j++)
               elmat.AddMatrix(norm*mcoeff(i,j), partelmat, te_nd*i, tr_nd*j);
      }
      else
      {
         if (Q)
            norm *= Q->Eval(Trans, ip);
         partelmat *= norm;
         for (int k = 0; k < vdim; k++)
            elmat.AddMatrix(partelmat, te_nd*k, tr_nd*k);
      }
   }
}

void VectorFEDivergenceIntegrator::AssembleElementMatrix2(
   const FiniteElement &trial_fe, const FiniteElement &test_fe,
   ElementTransformation &Trans, DenseMatrix &elmat)
{
   int trial_nd = trial_fe.GetDof(), test_nd = test_fe.GetDof(), i;

#ifdef MFEM_THREAD_SAFE
   Vector divshape(trial_nd), shape(test_nd);
#else
   divshape.SetSize(trial_nd);
   shape.SetSize(test_nd);
#endif

   elmat.SetSize(test_nd, trial_nd);

   const IntegrationRule *ir = IntRule;
   if (ir == NULL)
   {
      int order = trial_fe.GetOrder() + test_fe.GetOrder() - 1; // <--
      ir = &IntRules.Get(trial_fe.GetGeomType(), order);
   }

   elmat = 0.0;
   for (i = 0; i < ir->GetNPoints(); i++)
   {
      const IntegrationPoint &ip = ir->IntPoint(i);
      trial_fe.CalcDivShape(ip, divshape);
      test_fe.CalcShape(ip, shape);
      double w = ip.weight;
      if (Q)
      {
         Trans.SetIntPoint(&ip);
         w *= Q->Eval(Trans, ip);
      }
      shape *= w;
      AddMultVWt(shape, divshape, elmat);
   }
}

void VectorFECurlIntegrator::AssembleElementMatrix2(
   const FiniteElement &trial_fe, const FiniteElement &test_fe,
   ElementTransformation &Trans, DenseMatrix &elmat)
{
   int trial_nd = trial_fe.GetDof(), test_nd = test_fe.GetDof(), i;
   int dim = trial_fe.GetDim();

#ifdef MFEM_THREAD_SAFE
   DenseMatrix curlshapeTrial(trial_nd, dim);
   DenseMatrix curlshapeTrial_dFT(trial_nd, dim);
   DenseMatrix vshapeTest(test_nd, dim);
#else
   curlshapeTrial.SetSize(trial_nd, dim);
   curlshapeTrial_dFT.SetSize(trial_nd, dim);
   vshapeTest.SetSize(test_nd, dim);
#endif

   elmat.SetSize(test_nd, trial_nd);

   const IntegrationRule *ir = IntRule;
   if (ir == NULL)
   {
      int order = trial_fe.GetOrder() + test_fe.GetOrder() - 1; // <--
      ir = &IntRules.Get(trial_fe.GetGeomType(), order);
   }

   elmat = 0.0;
   for (i = 0; i < ir->GetNPoints(); i++)
   {
      const IntegrationPoint &ip = ir->IntPoint(i);
      Trans.SetIntPoint(&ip);
      trial_fe.CalcCurlShape(ip, curlshapeTrial);
      MultABt(curlshapeTrial, Trans.Jacobian(), curlshapeTrial_dFT);
      test_fe.CalcVShape(Trans, vshapeTest);
      double w = ip.weight;
      if (Q)
         w *= Q->Eval(Trans, ip);
      vshapeTest *= w;
      AddMultABt(vshapeTest, curlshapeTrial_dFT, elmat);
   }
}

void DerivativeIntegrator::AssembleElementMatrix2 (
   const FiniteElement &trial_fe,
   const FiniteElement &test_fe,
   ElementTransformation &Trans,
   DenseMatrix &elmat)
{
   int dim = trial_fe.GetDim();
   int trial_nd = trial_fe.GetDof();
   int test_nd = test_fe.GetDof();

   int i, l;
   double det;

   elmat.SetSize (test_nd,trial_nd);
   dshape.SetSize (trial_nd,dim);
   dshapedxt.SetSize(trial_nd,dim);
   dshapedxi.SetSize(trial_nd);
   invdfdx.SetSize(dim);
   shape.SetSize (test_nd);

   const IntegrationRule *ir = IntRule;
   if (ir == NULL)
   {
      int order;
      if (trial_fe.Space() == FunctionSpace::Pk)
         order = trial_fe.GetOrder() + test_fe.GetOrder() - 1;
      else
         order = trial_fe.GetOrder() + test_fe.GetOrder() + dim;

      if (trial_fe.Space() == FunctionSpace::rQk)
         ir = &RefinedIntRules.Get(trial_fe.GetGeomType(), order);
      else
         ir = &IntRules.Get(trial_fe.GetGeomType(), order);
   }

   elmat = 0.0;
   for(i = 0; i < ir->GetNPoints(); i++) {
      const IntegrationPoint &ip = ir->IntPoint(i);

      trial_fe.CalcDShape(ip, dshape);

      Trans.SetIntPoint (&ip);
      CalcInverse (Trans.Jacobian(), invdfdx);
      det = Trans.Weight();
      Mult (dshape, invdfdx, dshapedxt);

      test_fe.CalcShape(ip, shape);

      for (l = 0; l < trial_nd; l++)
         dshapedxi(l) = dshapedxt(l,xi);

      shape *= Q.Eval(Trans,ip) * det * ip.weight;
      AddMultVWt (shape, dshapedxi, elmat);
   }
}

void CurlCurlIntegrator::AssembleElementMatrix
( const FiniteElement &el, ElementTransformation &Trans,
  DenseMatrix &elmat )
{
   int nd = el.GetDof();
   int dim = el.GetDim();
   double w;

#ifdef MFEM_THREAD_SAFE
   DenseMatrix Curlshape(nd,dim), Curlshape_dFt(nd,dim);
#else
   Curlshape.SetSize(nd,dim);
   Curlshape_dFt.SetSize(nd,dim);
#endif
   elmat.SetSize(nd);

   const IntegrationRule *ir = IntRule;
   if (ir == NULL)
   {
      int order;
      if (el.Space() == FunctionSpace::Pk)
         order = 2*el.GetOrder() - 2;
      else
         order = 2*el.GetOrder();

      ir = &IntRules.Get(el.GetGeomType(), order);
   }

   elmat = 0.0;
   for (int i = 0; i < ir->GetNPoints(); i++)
   {
      const IntegrationPoint &ip = ir->IntPoint(i);
      el.CalcCurlShape(ip, Curlshape);

      Trans.SetIntPoint (&ip);

      w = ip.weight / Trans.Weight();

      MultABt(Curlshape, Trans.Jacobian(), Curlshape_dFt);

      if (Q)
         w *= Q->Eval(Trans, ip);

      AddMult_a_AAt(w, Curlshape_dFt, elmat);
   }
}


void VectorCurlCurlIntegrator::AssembleElementMatrix(
   const FiniteElement &el, ElementTransformation &Trans, DenseMatrix &elmat)
{
   int dim = el.GetDim();
   int dof = el.GetDof();
   int cld = (dim*(dim-1))/2;

#ifdef MFEM_THREAD_SAFE
   DenseMatrix dshape_hat(dof, dim), dshape(dof, dim);
   DenseMatrix curlshape(dim*dof, cld), Jadj(dim);
#else
   dshape_hat.SetSize(dof, dim);
   dshape.SetSize(dof, dim);
   curlshape.SetSize(dim*dof, cld);
   Jadj.SetSize(dim);
#endif

   const IntegrationRule *ir = IntRule;
   if (ir == NULL)
   {
      // use the same integration rule as diffusion
      int order = 2 * Trans.OrderGrad(&el);
      ir = &IntRules.Get(el.GetGeomType(), order);
   }

   elmat = 0.0;
   for (int i = 0; i < ir->GetNPoints(); i++)
   {
      const IntegrationPoint &ip = ir->IntPoint(i);
      el.CalcDShape(ip, dshape_hat);

      Trans.SetIntPoint(&ip);
      CalcAdjugate(Trans.Jacobian(), Jadj);
      double w = ip.weight / Trans.Weight();

      Mult(dshape_hat, Jadj, dshape);
      dshape.GradToCurl(curlshape);

      if (Q)
         w *= Q->Eval(Trans, ip);

      AddMult_a_AAt(w, curlshape, elmat);
   }
}

double VectorCurlCurlIntegrator::GetElementEnergy(
   const FiniteElement &el, ElementTransformation &Tr, const Vector &elfun)
{
   int dim = el.GetDim();
   int dof = el.GetDof();

#ifdef MFEM_THREAD_SAFE
   DenseMatrix dshape_hat(dof, dim), Jadj(dim), grad_hat(dim), grad(dim);
#else
   dshape_hat.SetSize(dof, dim);
   Jadj.SetSize(dim);
   grad_hat.SetSize(dim);
   grad.SetSize(dim);
#endif
   DenseMatrix elfun_mat(elfun.GetData(), dof, dim);

   const IntegrationRule *ir = IntRule;
   if (ir == NULL)
   {
      // use the same integration rule as diffusion
      int order = 2 * Tr.OrderGrad(&el);
      ir = &IntRules.Get(el.GetGeomType(), order);
   }

   double energy = 0.;
   for (int i = 0; i < ir->GetNPoints(); i++)
   {
      const IntegrationPoint &ip = ir->IntPoint(i);
      el.CalcDShape(ip, dshape_hat);

      MultAtB(elfun_mat, dshape_hat, grad_hat);

      Tr.SetIntPoint(&ip);
      CalcAdjugate(Tr.Jacobian(), Jadj);
      double w = ip.weight / Tr.Weight();

      Mult(grad_hat, Jadj, grad);

      if (dim == 2)
      {
         double curl = grad(0,1) - grad(1,0);
         w *= curl * curl;
      }
      else
      {
         double curl_x = grad(2,1) - grad(1,2);
         double curl_y = grad(0,2) - grad(2,0);
         double curl_z = grad(1,0) - grad(0,1);
         w *= curl_x * curl_x + curl_y * curl_y + curl_z * curl_z;
      }

      if (Q)
         w *= Q->Eval(Tr, ip);

      energy += w;
   }

   elfun_mat.ClearExternalData();

   return 0.5 * energy;
}


void VectorFEMassIntegrator::AssembleElementMatrix(
   const FiniteElement &el,
   ElementTransformation &Trans,
   DenseMatrix &elmat)
{
   int dof  = el.GetDof();
   int dim  = el.GetDim();

   double w;

#ifdef MFEM_THREAD_SAFE
   Vector D(VQ ? VQ->GetVDim() : 0);
   DenseMatrix vshape(dof, dim);
   DenseMatrix K(MQ ? MQ->GetVDim() : 0, MQ ? MQ->GetVDim() : 0);
#else
   vshape.SetSize(dof,dim);
   D.SetSize(VQ ? VQ->GetVDim() : 0);
   K.SetSize(MQ ? MQ->GetVDim() : 0, MQ ? MQ->GetVDim() : 0);
#endif
   DenseMatrix tmp(vshape.Height(), K.Width());

   elmat.SetSize(dof);
   elmat = 0.0;

   const IntegrationRule *ir = IntRule;
   if (ir == NULL)
   {
      // int order = 2 * el.GetOrder();
      int order = Trans.OrderW() + 2 * el.GetOrder();
      ir = &IntRules.Get(el.GetGeomType(), order);
   }

   for (int i = 0; i < ir->GetNPoints(); i++)
   {
      const IntegrationPoint &ip = ir->IntPoint(i);

      Trans.SetIntPoint (&ip);

      el.CalcVShape(Trans, vshape);

      w = ip.weight * Trans.Weight();
      if (MQ)
      {
         MQ->Eval(K, Trans, ip);
         K *= w;
         Mult(vshape,K,tmp);
         AddMultABt(tmp,vshape,elmat);
      }
      else if (VQ)
      {
         VQ->Eval(D, Trans, ip);
         D *= w;
         AddMultADAt(vshape, D, elmat);
      }
      else
      {
         if (Q)
            w *= Q -> Eval (Trans, ip);
         AddMult_a_AAt (w, vshape, elmat);
      }
   }
}

void VectorFEMassIntegrator::AssembleElementMatrix2(
   const FiniteElement &trial_fe, const FiniteElement &test_fe,
   ElementTransformation &Trans, DenseMatrix &elmat)
{
   // assume test_fe is scalar FE and trial_fe is vector FE
   int dim  = test_fe.GetDim();
   int trial_dof = trial_fe.GetDof();
   int test_dof = test_fe.GetDof();
   double w;

   if (VQ || MQ)
      mfem_error("VectorFEMassIntegrator::AssembleElementMatrix2(...)\n"
                 "   is not implemented for vector/tensor permeability");

#ifdef MFEM_THREAD_SAFE
   DenseMatrix vshape(trial_dof, dim);
   Vector shape(test_dof);
#else
   vshape.SetSize(trial_dof, dim);
   shape.SetSize(test_dof);
#endif

   elmat.SetSize (dim*test_dof, trial_dof);

   const IntegrationRule *ir = IntRule;
   if (ir == NULL)
   {
      int order = (Trans.OrderW() + test_fe.GetOrder() + trial_fe.GetOrder());
      ir = &IntRules.Get(test_fe.GetGeomType(), order);
   }

   elmat = 0.0;
   for (int i = 0; i < ir->GetNPoints(); i++)
   {
      const IntegrationPoint &ip = ir->IntPoint(i);

      Trans.SetIntPoint (&ip);

      trial_fe.CalcVShape(Trans, vshape);
      test_fe.CalcShape(ip, shape);

      w = ip.weight * Trans.Weight();
      if (Q)
         w *= Q -> Eval (Trans, ip);

      for (int d = 0; d < dim; d++)
      {
         for (int j = 0; j < test_dof; j++)
         {
            for (int k = 0; k < trial_dof; k++)
            {
               elmat(d * test_dof + j, k) += w * shape(j) * vshape(k, d);
            }
         }
      }
   }
}


void VectorDivergenceIntegrator::AssembleElementMatrix2(
   const FiniteElement &trial_fe,
   const FiniteElement &test_fe,
   ElementTransformation &Trans,
   DenseMatrix &elmat)
{
   int dim  = trial_fe.GetDim();
   int trial_dof = trial_fe.GetDof();
   int test_dof = test_fe.GetDof();
   double c;

   dshape.SetSize (trial_dof, dim);
   gshape.SetSize (trial_dof, dim);
   Jadj.SetSize (dim);
   divshape.SetSize (dim*trial_dof);
   shape.SetSize (test_dof);

   elmat.SetSize (test_dof, dim*trial_dof);

   const IntegrationRule *ir = IntRule;
   if (ir == NULL)
   {
      int order = Trans.OrderGrad(&trial_fe) + test_fe.GetOrder();
      ir = &IntRules.Get(trial_fe.GetGeomType(), order);
   }

   elmat = 0.0;

   for (int i = 0; i < ir -> GetNPoints(); i++)
   {
      const IntegrationPoint &ip = ir->IntPoint(i);

      trial_fe.CalcDShape (ip, dshape);
      test_fe.CalcShape (ip, shape);

      Trans.SetIntPoint (&ip);
      CalcAdjugate(Trans.Jacobian(), Jadj);

      Mult (dshape, Jadj, gshape);

      gshape.GradToDiv (divshape);

      c = ip.weight;
      if (Q)
         c *= Q -> Eval (Trans, ip);

      // elmat += c * shape * divshape ^ t
      shape *= c;
      AddMultVWt (shape, divshape, elmat);
   }
}


void DivDivIntegrator::AssembleElementMatrix(
   const FiniteElement &el,
   ElementTransformation &Trans,
   DenseMatrix &elmat)
{
   int dof = el.GetDof();
   double c;

#ifdef MFEM_THREAD_SAFE
   Vector divshape(dof);
#else
   divshape.SetSize(dof);
#endif
   elmat.SetSize(dof);

   const IntegrationRule *ir = IntRule;
   if (ir == NULL)
   {
      int order = 2 * el.GetOrder() - 2; // <--- OK for RTk
      ir = &IntRules.Get(el.GetGeomType(), order);
   }

   elmat = 0.0;

   for (int i = 0; i < ir -> GetNPoints(); i++)
   {
      const IntegrationPoint &ip = ir->IntPoint(i);

      el.CalcDivShape (ip, divshape);

      Trans.SetIntPoint (&ip);
      c = ip.weight / Trans.Weight();

      if (Q)
         c *= Q -> Eval (Trans, ip);

      // elmat += c * divshape * divshape ^ t
      AddMult_a_VVt (c, divshape, elmat);
   }
}


void VectorDiffusionIntegrator::AssembleElementMatrix(
   const FiniteElement &el,
   ElementTransformation &Trans,
   DenseMatrix &elmat)
{
   int dim = el.GetDim();
   int dof = el.GetDof();

   double norm;

   elmat.SetSize (dim * dof);

   Jinv.  SetSize (dim);
   dshape.SetSize (dof, dim);
   gshape.SetSize (dof, dim);
   pelmat.SetSize (dof);

   const IntegrationRule *ir = IntRule;
   if (ir == NULL)
   {
      // integrant is rational function if det(J) is not constant
      int order = 2 * Trans.OrderGrad(&el); // order of the numerator
      if (el.Space() == FunctionSpace::rQk)
         ir = &RefinedIntRules.Get(el.GetGeomType(), order);
      else
         ir = &IntRules.Get(el.GetGeomType(), order);
   }

   elmat = 0.0;

   for (int i = 0; i < ir -> GetNPoints(); i++)
   {
      const IntegrationPoint &ip = ir->IntPoint(i);

      el.CalcDShape (ip, dshape);

      Trans.SetIntPoint (&ip);
      norm = ip.weight * Trans.Weight();
      CalcInverse (Trans.Jacobian(), Jinv);

      Mult (dshape, Jinv, gshape);

      MultAAt (gshape, pelmat);

      if (Q)
         norm *= Q -> Eval (Trans, ip);

      pelmat *= norm;

      for (int d = 0; d < dim; d++)
      {
         for (int k = 0; k < dof; k++)
            for (int l = 0; l < dof; l++)
               elmat (dof*d+k, dof*d+l) += pelmat (k, l);
      }
   }
}


void ElasticityIntegrator::AssembleElementMatrix(
   const FiniteElement &el, ElementTransformation &Trans, DenseMatrix &elmat)
{
   int dof  = el.GetDof();
   int dim = el.GetDim();
   double w, L, M;

#ifdef MFEM_THREAD_SAFE
   DenseMatrix dshape(dof, dim), Jinv(dim), gshape(dof, dim), pelmat(dof);
   Vector divshape(dim*dof);
#else
   Jinv.SetSize(dim);
   dshape.SetSize(dof, dim);
   gshape.SetSize(dof, dim);
   pelmat.SetSize(dof);
   divshape.SetSize(dim*dof);
#endif

   elmat.SetSize(dof * dim);

   const IntegrationRule *ir = IntRule;
   if (ir == NULL)
   {
      int order = 2 * Trans.OrderGrad(&el); // correct order?
      ir = &IntRules.Get(el.GetGeomType(), order);
   }

   elmat = 0.0;

   for (int i = 0; i < ir -> GetNPoints(); i++)
   {
      const IntegrationPoint &ip = ir->IntPoint(i);

      el.CalcDShape(ip, dshape);

      Trans.SetIntPoint(&ip);
      w = ip.weight * Trans.Weight();
      CalcInverse(Trans.Jacobian(), Jinv);
      Mult(dshape, Jinv, gshape);
      MultAAt(gshape, pelmat);
      gshape.GradToDiv (divshape);

      M = mu->Eval(Trans, ip);
      if (lambda)
         L = lambda->Eval(Trans, ip);
      else
      {
         L = q_lambda * M;
         M = q_mu * M;
      }

      if (L != 0.0)
         AddMult_a_VVt(L * w, divshape, elmat);

      if (M != 0.0)
      {
         for (int d = 0; d < dim; d++)
         {
            for (int k = 0; k < dof; k++)
               for (int l = 0; l < dof; l++)
                  elmat (dof*d+k, dof*d+l) += (M * w) * pelmat(k, l);
         }
         for (int i = 0; i < dim; i++)
            for (int j = 0; j < dim; j++)
            {
               for (int k = 0; k < dof; k++)
                  for (int l = 0; l < dof; l++)
                     elmat(dof*i+k, dof*j+l) +=
                        (M * w) * gshape(k, j) * gshape(l, i);
               // + (L * w) * gshape(k, i) * gshape(l, j)
            }
      }
   }
}

void DGTraceIntegrator::AssembleFaceMatrix(const FiniteElement &el1,
                                           const FiniteElement &el2,
                                           FaceElementTransformations &Trans,
                                           DenseMatrix &elmat)
{
   int dim, ndof1, ndof2;

   double un, a, b, w;

   dim = el1.GetDim();
   ndof1 = el1.GetDof();
   Vector vu(dim), nor(dim);

   if (Trans.Elem2No >= 0)
      ndof2 = el2.GetDof();
   else
      ndof2 = 0;

   shape1.SetSize(ndof1);
   shape2.SetSize(ndof2);
   elmat.SetSize(ndof1 + ndof2);
   elmat = 0.0;

   const IntegrationRule *ir = IntRule;
   if (ir == NULL)
   {
      int order;
      // Assuming order(u)==order(mesh)
      if (Trans.Elem2No >= 0)
         order = (min(Trans.Elem1->OrderW(), Trans.Elem2->OrderW()) +
                  2*max(el1.GetOrder(), el2.GetOrder()));
      else
         order = Trans.Elem1->OrderW() + 2*el1.GetOrder();
      if (el1.Space() == FunctionSpace::Pk)
         order++;
      ir = &IntRules.Get(Trans.FaceGeom, order);
   }

   for (int p = 0; p < ir->GetNPoints(); p++)
   {
      const IntegrationPoint &ip = ir->IntPoint(p);
      IntegrationPoint eip1, eip2;
      Trans.Loc1.Transform(ip, eip1);
      if (ndof2)
         Trans.Loc2.Transform(ip, eip2);
      el1.CalcShape(eip1, shape1);

      Trans.Face->SetIntPoint(&ip);
      Trans.Elem1->SetIntPoint(&eip1);

      u->Eval(vu, *Trans.Elem1, eip1);

      if (dim == 1)
         nor(0) = 2*eip1.x - 1.0;
      else
         CalcOrtho(Trans.Face->Jacobian(), nor);

      un = vu * nor;
      a = 0.5 * alpha * un;
      b = beta * fabs(un);
      // note: if |alpha/2|==|beta| then |a|==|b|, i.e. (a==b) or (a==-b)
      //       and therefore two blocks in the element matrix contribution
      //       (from the current quadrature point) are 0

      if (rho)
      {
         double rho_p;
         if (un >= 0.0 && ndof2)
         {
            Trans.Elem2->SetIntPoint(&eip2);
            rho_p = rho->Eval(*Trans.Elem2, eip2);
         }
         else
         {
            rho_p = rho->Eval(*Trans.Elem1, eip1);
         }
         a *= rho_p;
         b *= rho_p;
      }

      w = ip.weight * (a+b);
      if (w != 0.0)
      {
         for (int i = 0; i < ndof1; i++)
            for (int j = 0; j < ndof1; j++)
               elmat(i, j) += w * shape1(i) * shape1(j);
      }

      if (ndof2)
      {
         el2.CalcShape(eip2, shape2);

         if (w != 0.0)
            for (int i = 0; i < ndof2; i++)
               for (int j = 0; j < ndof1; j++)
                  elmat(ndof1+i, j) -= w * shape2(i) * shape1(j);

         w = ip.weight * (b-a);
         if (w != 0.0)
         {
            for (int i = 0; i < ndof2; i++)
               for (int j = 0; j < ndof2; j++)
                  elmat(ndof1+i, ndof1+j) += w * shape2(i) * shape2(j);

            for (int i = 0; i < ndof1; i++)
               for (int j = 0; j < ndof2; j++)
                  elmat(i, ndof1+j) -= w * shape1(i) * shape2(j);
         }
      }
   }
}

void DGDiffusionIntegrator::AssembleFaceMatrix(
   const FiniteElement &el1, const FiniteElement &el2,
   FaceElementTransformations &Trans, DenseMatrix &elmat)
{
   int dim, ndof1, ndof2, ndofs;
   bool kappa_is_nonzero = (kappa != 0.);
   double w, wq = 0.0;

   dim = el1.GetDim();
   ndof1 = el1.GetDof();

   nor.SetSize(dim);
   nh.SetSize(dim);
   ni.SetSize(dim);
   adjJ.SetSize(dim);
   if (MQ)
      mq.SetSize(dim);

   shape1.SetSize(ndof1);
   dshape1.SetSize(ndof1, dim);
   dshape1dn.SetSize(ndof1);
   if (Trans.Elem2No >= 0)
   {
      ndof2 = el2.GetDof();
      shape2.SetSize(ndof2);
      dshape2.SetSize(ndof2, dim);
      dshape2dn.SetSize(ndof2);
   }
   else
      ndof2 = 0;

   ndofs = ndof1 + ndof2;
   elmat.SetSize(ndofs);
   elmat = 0.0;
   if (kappa_is_nonzero)
   {
      jmat.SetSize(ndofs);
      jmat = 0.;
   }

   const IntegrationRule *ir = IntRule;
   if (ir == NULL)
   {
      // a simple choice for the integration order; is this OK?
      int order;
      if (ndof2)
         order = 2*max(el1.GetOrder(), el2.GetOrder());
      else
         order = 2*el1.GetOrder();
      ir = &IntRules.Get(Trans.FaceGeom, order);
   }

   // assmble: < {(Q \nabla u).n},[v] >      --> elmat
   //          kappa < {h^{-1} Q} [u],[v] >  --> jmat
   for (int p = 0; p < ir->GetNPoints(); p++)
   {
      const IntegrationPoint &ip = ir->IntPoint(p);
      IntegrationPoint eip1, eip2;

      Trans.Loc1.Transform(ip, eip1);
      Trans.Face->SetIntPoint(&ip);
      if (dim == 1)
         nor(0) = 2*eip1.x - 1.0;
      else
         CalcOrtho(Trans.Face->Jacobian(), nor);

      el1.CalcShape(eip1, shape1);
      el1.CalcDShape(eip1, dshape1);
      Trans.Elem1->SetIntPoint(&eip1);
      w = ip.weight/Trans.Elem1->Weight();
      if (ndof2)
         w /= 2;
      if (!MQ)
      {
         if (Q)
            w *= Q->Eval(*Trans.Elem1, eip1);
         ni.Set(w, nor);
      }
      else
      {
         nh.Set(w, nor);
         MQ->Eval(mq, *Trans.Elem1, eip1);
         mq.MultTranspose(nh, ni);
      }
      CalcAdjugate(Trans.Elem1->Jacobian(), adjJ);
      adjJ.Mult(ni, nh);
      if (kappa_is_nonzero)
         wq = ni * nor;
      // Note: in the jump term, we use 1/h1 = |nor|/det(J1) which is
      // independent of Loc1 and always gives the size of element 1 in
      // direction perpendicular to the face. Indeed, for linear transformation
      //     |nor|=measure(face)/measure(ref. face),
      //   det(J1)=measure(element)/measure(ref. element),
      // and the ratios measure(ref. element)/measure(ref. face) are
      // compatible for all element/face pairs.
      // For example: meas(ref. tetrahedron)/meas(ref. triangle) = 1/3, and
      // for any tetrahedron vol(tet)=(1/3)*height*area(base).
      // For interior faces: q_e/h_e=(q1/h1+q2/h2)/2.

      dshape1.Mult(nh, dshape1dn);
      for (int i = 0; i < ndof1; i++)
         for (int j = 0; j < ndof1; j++)
            elmat(i, j) += shape1(i) * dshape1dn(j);

      if (ndof2)
      {
         Trans.Loc2.Transform(ip, eip2);
         el2.CalcShape(eip2, shape2);
         el2.CalcDShape(eip2, dshape2);
         Trans.Elem2->SetIntPoint(&eip2);
         w = ip.weight/2/Trans.Elem2->Weight();
         if (!MQ)
         {
            if (Q)
               w *= Q->Eval(*Trans.Elem2, eip2);
            ni.Set(w, nor);
         }
         else
         {
            nh.Set(w, nor);
            MQ->Eval(mq, *Trans.Elem2, eip2);
            mq.MultTranspose(nh, ni);
         }
         CalcAdjugate(Trans.Elem2->Jacobian(), adjJ);
         adjJ.Mult(ni, nh);
         if (kappa_is_nonzero)
            wq += ni * nor;

         dshape2.Mult(nh, dshape2dn);

         for (int i = 0; i < ndof1; i++)
            for (int j = 0; j < ndof2; j++)
               elmat(i, ndof1 + j) += shape1(i) * dshape2dn(j);

         for (int i = 0; i < ndof2; i++)
            for (int j = 0; j < ndof1; j++)
               elmat(ndof1 + i, j) -= shape2(i) * dshape1dn(j);

         for (int i = 0; i < ndof2; i++)
            for (int j = 0; j < ndof2; j++)
               elmat(ndof1 + i, ndof1 + j) -= shape2(i) * dshape2dn(j);
      }

      if (kappa_is_nonzero)
      {
         // only assemble the lower triangular part of jmat
         wq *= kappa;
         for (int i = 0; i < ndof1; i++)
         {
            const double wsi = wq*shape1(i);
            for (int j = 0; j <= i; j++)
               jmat(i, j) += wsi * shape1(j);
         }
         if (ndof2)
         {
            for (int i = 0; i < ndof2; i++)
            {
               const int i2 = ndof1 + i;
               const double wsi = wq*shape2(i);
               for (int j = 0; j < ndof1; j++)
                  jmat(i2, j) -= wsi * shape1(j);
               for (int j = 0; j <= i; j++)
                  jmat(i2, ndof1 + j) += wsi * shape2(j);
            }
         }
      }
   }

   // elmat := -elmat + sigma*elmat^t + jmat
   if (kappa_is_nonzero)
   {
      for (int i = 0; i < ndofs; i++)
      {
         for (int j = 0; j < i; j++)
         {
            double aij = elmat(i,j), aji = elmat(j,i), mij = jmat(i,j);
            elmat(i,j) = sigma*aji - aij + mij;
            elmat(j,i) = sigma*aij - aji + mij;
         }
         elmat(i,i) = (sigma - 1.)*elmat(i,i) + jmat(i,i);
      }
   }
   else
   {
      for (int i = 0; i < ndofs; i++)
      {
         for (int j = 0; j < i; j++)
         {
            double aij = elmat(i,j), aji = elmat(j,i);
            elmat(i,j) = sigma*aji - aij;
            elmat(j,i) = sigma*aij - aji;
         }
         elmat(i,i) *= (sigma - 1.);
      }
   }
}

void TraceJumpIntegrator::AssembleFaceMatrix(
   const FiniteElement &trial_face_fe, const FiniteElement &test_fe1,
   const FiniteElement &test_fe2, FaceElementTransformations &Trans,
   DenseMatrix &elmat)
{
   int i, j, face_ndof, ndof1, ndof2;
   int order;

   double w;

   face_ndof = trial_face_fe.GetDof();
   ndof1 = test_fe1.GetDof();

   face_shape.SetSize(face_ndof);
   shape1.SetSize(ndof1);

   if (Trans.Elem2No >= 0)
   {
      ndof2 = test_fe2.GetDof();
      shape2.SetSize(ndof2);
   }
   else
      ndof2 = 0;

   elmat.SetSize(ndof1 + ndof2, face_ndof);
   elmat = 0.0;

   const IntegrationRule *ir = IntRule;
   if (ir == NULL)
   {
      if (Trans.Elem2No >= 0)
         order = max(test_fe1.GetOrder(), test_fe2.GetOrder());
      else
         order = test_fe1.GetOrder();
      order += trial_face_fe.GetOrder();
      if (trial_face_fe.GetMapType() == FiniteElement::VALUE)
         order += Trans.Face->OrderW();
      ir = &IntRules.Get(Trans.FaceGeom, order);
   }

   for (int p = 0; p < ir->GetNPoints(); p++)
   {
      const IntegrationPoint &ip = ir->IntPoint(p);
      IntegrationPoint eip1, eip2;
      // Trace finite element shape function
      Trans.Face->SetIntPoint(&ip);
      trial_face_fe.CalcShape(ip, face_shape);
      // Side 1 finite element shape function
      Trans.Loc1.Transform(ip, eip1);
      test_fe1.CalcShape(eip1, shape1);
      Trans.Elem1->SetIntPoint(&eip1);
      if (ndof2)
      {
         // Side 2 finite element shape function
         Trans.Loc2.Transform(ip, eip2);
         test_fe2.CalcShape(eip2, shape2);
         Trans.Elem2->SetIntPoint(&eip2);
      }
      w = ip.weight;
      if (trial_face_fe.GetMapType() == FiniteElement::VALUE)
         w *= Trans.Face->Weight();
      face_shape *= w;
      for (i = 0; i < ndof1; i++)
         for (j = 0; j < face_ndof; j++)
            elmat(i, j) += shape1(i) * face_shape(j);
      if (ndof2)
      {
         // Subtract contribution from side 2
         for (i = 0; i < ndof2; i++)
            for (j = 0; j < face_ndof; j++)
               elmat(ndof1+i, j) -= shape2(i) * face_shape(j);
      }
   }
}

}
