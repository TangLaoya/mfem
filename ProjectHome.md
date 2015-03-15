MFEM is a _lightweight_, _general_, _scalable_ C++ library for finite element methods. Its features include:
  * 2D and 3D, arbitrary high-order H1, [H(curl)](http://doxygen.mfem.googlecode.com/hg/examples/README_files/index.html?hcurl), [H(div)](http://doxygen.mfem.googlecode.com/hg/examples/README_files/index.html?hdiv), L2 and NURBS elements.
  * Parallel version scalable to [hundreds of thousands](http://www.llnl.gov/casc/blast/parallel.php) of MPI cores, tightly integrated with the _[hypre](http://www.llnl.gov/CASC/hypre)_ linear solvers library.
  * Conforming or nonconforming adaptive mesh refinement ([AMR](http://doxygen.mfem.googlecode.com/hg/examples/README_files/index.html?amr)), including anisotropic refinement.
  * Galerkin, [mixed](http://doxygen.mfem.googlecode.com/hg/examples/README_files/index.html?mixed), isogeometric, [DG](http://doxygen.mfem.googlecode.com/hg/examples/README_files/index.html?dg) and [DPG](http://doxygen.mfem.googlecode.com/hg/examples/README_files/index.html?dpg) discretizations.
  * Support for triangular, quadrilateral, tetrahedral and hexahedral elements with [curved](https://code.google.com/p/glvis/wiki/MeshFormats#Curvilinear_and_more_general_meshes) boundaries.
  * ... and [many more](Features.md).

MFEM is currently used in the [BLAST](http://www.llnl.gov/casc/blast), [GLVis](http://glvis.googlecode.com) and [XBraid](http://www.llnl.gov/casc/xbraid) projects. See also our [Gallery](Gallery.md).

## Latest Release ##

### [mfem-3.0.1.tgz](http://goo.gl/Is2XJq) | [Example codes](http://doxygen.mfem.googlecode.com/hg/examples/README_files/index.html) | [Code documentation](http://doxygen.mfem.googlecode.com/hg/html/index.html) | [New features](http://mfem.googlecode.com/hg/CHANGELOG) ###

_Please use the [issue tracker](http://code.google.com/p/mfem/issues) to report [bugs](http://code.google.com/p/mfem/issues/entry?template=Defect%20report%20from%20user) or post [questions or comments](http://code.google.com/p/mfem/issues/entry?template=Question%20or%20Comment)_. For older releases see [all releases](https://code.google.com/p/mfem/wiki/Releases).


## Documentation ##

The best starting point for new users is the interactive documentation of the [example codes](http://doxygen.mfem.googlecode.com/hg/examples/README_files/index.html).

[Building MFEM](http://code.google.com/p/mfem/wiki/Building) | [Serial Tutorial](http://code.google.com/p/mfem/wiki/SerialTutorial) | [Parallel Tutorial](http://code.google.com/p/mfem/wiki/ParallelTutorial) | [Mesh Formats](http://code.google.com/p/glvis/wiki/MeshFormats) | [GLVis Tutorials](http://code.google.com/p/glvis/wiki)

MFEM is being developed at [CASC](http://computation.llnl.gov/casc/), [LLNL](https://www.llnl.gov/). It can be cited with
<pre>
@misc{mfem-library,<br>
title = {{MFEM}: Modular finite element methods},<br>
howpublished = {\url{mfem.googlecode.com}}<br>
}</pre>
See also our list of [Publications](Publications.md).