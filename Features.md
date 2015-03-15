# MFEM Features #

The goal of MFEM is to enable research and development of scalable finite element discretization and solver algorithms through general finite element abstractions, accurate and flexible visualization, and tight integration with the _[hypre](http://www.llnl.gov/CASC/hypre)_ library.

Conceptually, MFEM can be viewed as a finite element toolbox that provides the building blocks for developing finite element algorithms in a manner similar to that of MATLAB for linear algebra methods.

## Higher-order Finite Element Spaces ##

<img src='http://images.mfem.googlecode.com/hg/ho-spaces-padding.png' align='right' alt='H(curl) and H(div) spaces'>

MFEM supports a wide variety of <a href='http://doxygen.mfem.googlecode.com/hg/html/inherit_graph_16.svg'>finite element</a> <a href='http://doxygen.mfem.googlecode.com/hg/html/inherit_graph_17.svg'>spaces</a> in 2D and 3D, including arbitrary high-order:<br>
<ul><li><a href='http://doxygen.mfem.googlecode.com/hg/examples/README_files/index.html?h1'>H1</a>-conforming, <a href='http://doxygen.mfem.googlecode.com/hg/examples/README_files/index.html?hdiv'>H(div)</a>-conforming, <a href='http://doxygen.mfem.googlecode.com/hg/examples/README_files/index.html?hcurl'>H(curl)</a>-conforming spaces,<br>
</li><li>discontinuous <a href='http://doxygen.mfem.googlecode.com/hg/examples/README_files/index.html?l2'>L2</a> spaces,<br>
</li><li>numerical trace (<a href='http://doxygen.mfem.googlecode.com/hg/examples/README_files/index.html?h12'>interfacial</a>) spaces,<br>
</li><li><a href='http://doxygen.mfem.googlecode.com/hg/examples/README_files/index.html?nurbs'>NURBS</a> spaces for isogeometric analysis.</li></ul>

Many <a href='http://doxygen.mfem.googlecode.com/hg/html/inherit_graph_67.svg'>bilinear</a> and <a href='http://doxygen.mfem.googlecode.com/hg/html/inherit_graph_40.svg'>linear</a> forms defined on these spaces, as well as linear operators such as gradient, curl and embedding between these spaces, are available in the code.<br>
<br>
<h2>Flexible Discretization</h2>

In addition to classical Galerkin methods, MFEM enables the quick prototyping of<br>
<ul><li><a href='http://doxygen.mfem.googlecode.com/hg/examples/README_files/index.html?mixed'>mixed</a> finite elements,<br>
</li><li>Discontinuous Galerkin (<a href='http://doxygen.mfem.googlecode.com/hg/examples/README_files/index.html?dg'>DG</a>) methods,<br>
</li><li><a href='http://doxygen.mfem.googlecode.com/hg/examples/README_files/index.html?nurbs'>isogeometric</a> analysis methods,<br>
</li><li>Discontinuous Petrov-Galerkin (<a href='http://doxygen.mfem.googlecode.com/hg/examples/README_files/index.html?dpg'>DPG</a>) approaches.</li></ul>

<a href='Hidden comment: 
http://doxygen.mfem.googlecode.com/hg/examples/README_files/ex9.png
'></a><br>
<br>
<img src='http://doxygen.mfem.googlecode.com/hg/examples/README_files/ex6.png' align='right' width='230'>

<h2>Wide Range of Mesh Types</h2>

MFEM supports arbitrary element <a href='http://doxygen.mfem.googlecode.com/hg/html/inherit_graph_14.svg'>transformations</a> and includes classes for dealing with:<br>
<ul><li>triangular, quadrilateral, tetrahedral and hexahedral <a href='http://doxygen.mfem.googlecode.com/hg/html/inherit_graph_13.svg'>elements</a>,<br>
</li><li>conforming local mesh refinement (triangular/tetrahedral meshes),<br>
</li><li>non-conforming mesh refinement (quadrilateral/hexahedral meshes), including anisotropic refinement,<br>
</li><li>higher-order elements with <a href='http://code.google.com/p/glvis/wiki/CurvilinearVTKMeshes'>curved</a> boundaries,<br>
</li><li><a href='https://code.google.com/p/mfem/source/browse/data/square-disc-surf.mesh'>surface</a> meshes embedded in 3D,<br>
</li><li>topologically <a href='https://code.google.com/p/mfem/source/browse/data/periodic-hexagon.mesh'>periodic</a> meshes,<br>
</li><li>1D meshes.</li></ul>

<h2>Parallel and Scalable</h2>

MFEM supports MPI-based parallelism throughout the library, and can readily be used as a scalable unstructured finite element problem generator.<br>
<ul><li>MFEM-based <a href='http://www.llnl.gov/casc/blast'>applications</a> have been <a href='http://www.llnl.gov/casc/blast/parallel.php'>scaled</a> to <a href='http://computation.llnl.gov/project/blast/images/blast_strong_vulcan_inline.png'>hundreds of thousands</a> of parallel cores.<br>
</li><li>An experimental support for OpenMP acceleration is also included.</li></ul>

<a href='Hidden comment: 
<img src="http://computation.llnl.gov/project/blast/images/blast_strong_vulcan_inline.png", width=400>
'></a><br>
<br>
A serial MFEM application typically requires <a href='http://doxygen.mfem.googlecode.com/hg/html/ex1_8cpp_source.html'>minimal</a> <a href='http://doxygen.mfem.googlecode.com/hg/html/ex1p_8cpp_source.html'>changes</a> to transition to a high-performing parallel version of the code, where it can take advantage of the integrated scalable linear solvers from the <i><a href='http://www.llnl.gov/CASC/hypre'>hypre</a></i> library.<br>
<br>
<h2>Built-in Solvers</h2>

MFEM is commonly used as a "finite element to linear algebra translator", since it can take a problem described in terms of finite element-type objects, and produce the corresponding linear algebra<br>
<a href='http://doxygen.mfem.googlecode.com/hg/html/inherit_graph_102.svg'>vectors</a> and <a href='http://doxygen.mfem.googlecode.com/hg/html/inherit_graph_72.svg'>sparse matrices</a>.<br>
<br>
Several matrix storage formats are available including dense, compressed sparse row (<a href='http://doxygen.mfem.googlecode.com/hg/html/classmfem_1_1SparseMatrix.html'>CSR</a>) and parallel compressed sparse row (<a href='http://doxygen.mfem.googlecode.com/hg/html/classmfem_1_1HypreParMatrix.html'>ParCSR</a>). Block vectors, operators and <a href='http://doxygen.mfem.googlecode.com/hg/html/classmfem_1_1BlockMatrix.html'>matrices</a> are also supported.<br>
<br>
A variety of solvers are available for the resulting linear algebra systems (or semi-discrete time-integration problems):<br>
<ul><li>point-wise and polynomial <a href='http://doxygen.mfem.googlecode.com/hg/html/classmfem_1_1SparseSmoother.html'>serial</a> and <a href='http://doxygen.mfem.googlecode.com/hg/html/classmfem_1_1HypreSmoother.html'>parallel</a> smoothers,<br>
<img src='https://computation.llnl.gov/project/linear_solvers/images/hypre_wiw.gif' align='right' width='250'>
</li><li><a href='http://doxygen.mfem.googlecode.com/hg/html/classmfem_1_1IterativeSolver.html'>Krylov solvers</a>, such as PCG, MINRES and GMRES applicable to general <a href='http://doxygen.mfem.googlecode.com/hg/html/inherit_graph_72.svg'>operators</a> in serial and in parallel,<br>
</li><li>high-performance preconditioners from the <i><a href='http://www.llnl.gov/CASC/hypre'>hypre</a></i> library including the <a href='http://doxygen.mfem.googlecode.com/hg/examples/README_files/index.html?amg'>BoomerAMG</a>, <a href='http://doxygen.mfem.googlecode.com/hg/examples/README_files/index.html?ams'>AMS</a> and <a href='http://doxygen.mfem.googlecode.com/hg/examples/README_files/index.html?ads'>ADS</a> solvers,<br>
</li><li>sequential sparse direct solvers from the <a href='http://www.cise.ufl.edu/research/sparse/SuiteSparse'>SuiteSparse</a> library,<br>
</li><li>explicit and implicit high-order Runge-Kutta <a href='http://doxygen.mfem.googlecode.com/hg/html/inherit_graph_71.svg'>time integrators</a>,<br>
</li><li>solvers for nonlinear problems (Newton) and for single linearly constrained <a href='http://doxygen.mfem.googlecode.com/hg/html/classmfem_1_1SLBQPOptimizer.html'>quadratic minimization</a> problems.</li></ul>

<h2>Extensive Examples</h2>

MFEM includes a number of well-documented <a href='http://doxygen.mfem.googlecode.com/hg/examples/README_files/index.html'>example codes</a> that can be used as tutorials, as well as simple starting points for user applications. Some of the included example codes are:<br>
<br>
<ul><li><a href='http://doxygen.mfem.googlecode.com/hg/html/ex1_8cpp_source.html'>Example 1</a>: nodal H1 FEM for the Laplace problem,<br>
</li><li><a href='http://doxygen.mfem.googlecode.com/hg/html/ex2_8cpp_source.html'>Example 2</a>: vector FEM for linear elasticity,<br>
</li><li><a href='http://doxygen.mfem.googlecode.com/hg/html/ex3_8cpp_source.html'>Example 3</a>: Nedelec H(curl) FEM for the definite Maxwell problem,<br>
</li><li><a href='http://doxygen.mfem.googlecode.com/hg/html/ex4_8cpp_source.html'>Example 4</a>: Raviart-Thomas H(div) FEM for the grad-div problem,<br>
</li><li><a href='http://doxygen.mfem.googlecode.com/hg/html/ex5_8cpp_source.html'>Example 5</a>: mixed pressure-velocity FEM for the Darcy problem,<br>
</li><li><a href='http://doxygen.mfem.googlecode.com/hg/html/ex6_8cpp_source.html'>Example 6</a>: non-conforming adaptive mesh refinement for the Laplace problem,<br>
</li><li><a href='http://doxygen.mfem.googlecode.com/hg/html/ex7_8cpp_source.html'>Example 7</a>: Laplace problem on a surface (the unit sphere),<br>
</li><li><a href='http://doxygen.mfem.googlecode.com/hg/html/ex8_8cpp_source.html'>Example 8</a>: Discontinuous Petrov-Galerkin (DPG) for the Laplace problem,<br>
</li><li><a href='http://doxygen.mfem.googlecode.com/hg/html/ex9_8cpp_source.html'>Example 9</a>: Discontinuous Galerkin (DG) time-dependent advection,<br>
</li><li><a href='http://doxygen.mfem.googlecode.com/hg/html/ex10_8cpp_source.html'>Example 10</a>: time-dependent implicit nonlinear elasticity.</li></ul>

Most of the examples have a serial and a parallel version, illustrating the ease of transition and the minimal code changes between the two.<br>
<br>
<a href='Hidden comment: 
<img src="http://images.glvis.googlecode.com/hg/logo-130.png" align="right" alt="GLVis logo">
'></a><br>
<br>
<h2>Accurate and Flexible Visualization</h2>
The general (high-order) meshes and finite element functions in MFEM can be visualized accurately using the companion OpenGL visualization tool <a href='http://glvis.googlecode.com'>GLVis</a>, which is built on top of MFEM.<br>
<br>
<a href='Hidden comment: 
http://glvis.googlecode.com/files/glvis-screenshot2.png
'></a><br>
<br>
The <a href='http://visit.llnl.gov'>VisIt</a> visualization and analysis tool also natively supports MFEM formats.<br>
<br>
<h2>Lightweight, Portable and Easily Extendable</h2>

The MFEM code base is <a href='Releases.md'>relatively small</a> and is written in highly portable C++ (e.g. with very limited use of templates and the STL).<br>
<br>
<ul><li>The serial version of MFEM has no external dependencies and is <a href='Building.md'>straightforward to build</a> on Linux, Mac and Windows machines.<br>
</li><li>The MPI-parallel version uses two third-party libraries (<i>hypre</i> and METIS), and is also easy to build with an MPI compiler.<br>
</li><li>On most machines, both versions can be built in under a minute by typing: "<code>make serial -j</code>" and "<code>make parallel -j</code>" respectively.</li></ul>

The object-oriented design of MFEM <a href='http://doxygen.mfem.googlecode.com/hg/html/index.html'>separates</a> the mesh, finite element and linear algebra abstractions, making it easy to extend the library and adapt it to a variety of <a href='Publications.md'>applications</a>.<br>
<br>
<h2>Open Source</h2>

MFEM is an open-source software, and can be freely used under the terms of the <a href='https://www.gnu.org/licenses/lgpl-2.1.html'>LGPL 2.1</a> license.