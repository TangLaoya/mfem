[![](http://images.mfem.googlecode.com/hg/ex1p-np100-logo.png)](http://code.google.com/p/mfem/wiki/Gallery)
<a href='Hidden comment: 
http://mfem.googlecode.com/files/ex1p-np100.png
'></a>

## Introduction ##
MFEM is a _lightweight_, _general_, _scalable_ C++ library for finite element methods.

Its goal is to enable the research and development of scalable finite element discretization
and solver algorithms through general finite element abstractions, accurate and
flexible visualization, and tight integration with the _[hypre](http://www.llnl.gov/CASC/hypre)_ library.

## Latest release ##
### [mfem-3.0.1.tgz](http://goo.gl/Is2XJq) | [Example codes](http://doxygen.mfem.googlecode.com/hg/examples/README_files/index.html) | [Code documentation](http://doxygen.mfem.googlecode.com/hg/html/index.html) | [New features](http://mfem.googlecode.com/hg/CHANGELOG) ###

The best starting point for new users interested in MFEM's features is the
[interactive documentation](http://doxygen.mfem.googlecode.com/hg/examples/README_files/index.html) of the example codes.

_While we recommend that users download the latest release above, all MFEM releases are available [here](Releases.md).
Please use the [issue tracker](http://code.google.com/p/mfem/issues) to report [bugs](http://code.google.com/p/mfem/issues/entry?template=Defect%20report%20from%20user) or post [questions or comments](http://code.google.com/p/mfem/issues/entry?template=Question%20or%20Comment)._

## Overview ##
Conceptually, MFEM can be viewed as a finite element toolbox, that provides the
building blocks for developing finite element algorithms in a manner similar to
that of MATLAB for linear algebra methods. In particular, MFEM supports a wide variety of [finite element spaces](http://doxygen.mfem.googlecode.com/hg/html/classmfem_1_1FiniteElement.html) in 2D and 3D, including arbitrary high-order H1-conforming, discontinuous (L2), H(div)-conforming, H(curl)-conforming and NURBS elements, as well as many [bilinear](http://doxygen.mfem.googlecode.com/hg/html/classmfem_1_1BilinearFormIntegrator.html) and
[linear forms](http://doxygen.mfem.googlecode.com/hg/html/classmfem_1_1LinearFormIntegrator.html) defined on them.  It enables the quick
prototyping of various finite element discretizations, including Galerkin
methods, mixed finite elements, Discontinuous Galerkin (DG), isogeometric
analysis and Discontinuous Petrov-Galerkin (DPG) approaches.

MFEM includes [classes](http://doxygen.mfem.googlecode.com/hg/html/classmfem_1_1Element.html) for dealing with a wide range of mesh types: triangular,
quadrilateral, tetrahedral and hexahedral, as well as surface and topologically
periodical meshes. It has general support for mesh refinement, including local
conforming and [non-conforming (AMR)](http://doxygen.mfem.googlecode.com/hg/html/classmfem_1_1NCMesh.html) adaptive refinement. Arbitrary element
transformations, allowing for [high-order mesh elements with curved boundaries](http://code.google.com/p/glvis/wiki/CurvilinearVTKMeshes), are also supported.

MFEM is commonly used as a "finite element to linear algebra translator", since
it can take a problem described in terms of finite element-type objects, and
produce the corresponding linear algebra vectors and sparse matrices. In order
to facilitate this, MFEM uses compressed sparse row (CSR) sparse matrix storage
and includes simple smoothers and Krylov solvers, such as PCG, MINRES and GMRES,
as well as support for sequential sparse direct solvers from the [SuiteSparse](http://www.cise.ufl.edu/research/sparse/SuiteSparse)
library. Nonlinear solvers (the Newton method), and several explicit and
implicit Runge-Kutta time integrators are also available.

MFEM supports MPI-based parallelism throughout the library, and can readily be
used as a scalable unstructured finite element problem generator. MFEM-based
applications require minimal changes to transition from a serial to a
high-performing parallel version of the code, where they can take advantage of
the integrated [scalable linear solvers](http://doxygen.mfem.googlecode.com/hg/html/classmfem_1_1HypreSolver.html) from the _[hypre](http://www.llnl.gov/CASC/hypre)_ library. An experimental support for OpenMP acceleration is also included.

MFEM originates from the previous research effort in the (unreleased) AggieFEM/aFEM project.  Some examples of its use can be found in the [examples/](http://doxygen.mfem.googlecode.com/hg/examples/README_files/index.html) directory, the [Gallery](http://code.google.com/p/mfem/wiki/Gallery) and [Publications](http://code.google.com/p/mfem/wiki/Publications) pages. We recommend using it together with [GLVis](http://glvis.googlecode.com), which is an OpenGL visualization tool build on top of MFEM.

## Tutorials ##
  * [Building](http://code.google.com/p/mfem/wiki/Building)
  * [Serial tutorial](http://code.google.com/p/mfem/wiki/SerialTutorial)
  * [Parallel tutorial](http://code.google.com/p/mfem/wiki/ParallelTutorial)
  * [Mesh Formats](http://code.google.com/p/glvis/wiki/MeshFormats)
  * [GLVis tutorials](http://code.google.com/p/glvis/wiki)