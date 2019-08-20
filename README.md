# GNATV_cloth_sim
General Nonlinear Anisotropic Tensile Viscoelasticity Cloth Sim, implimented from Volino &amp; Magnenat-Thalmann 2009
Author: Rachel Strohkorb

This project contains a cloth simulator based on force-displacement data taken from actual cloth samples. All the underlying math
for the force calculations can be found in [3]. I implimented the implicit integration found in [1] and used the damping model from [2].
This sim does not include collisions.

To run, you will need Qt to run qmake. You will also need NGL (found here: https://github.com/NCCA/NGL), a graphics library
written by Jon Macey for the NCCA at Bournemouth University, as well as boost for both its string parser and b-spline interpolator.

A NOTE:

I've organized this project into a 'dirty' branch and the 'clean' master branch. I tried a lot of different things with this project, many
of which did not work, and the methods from those attempts stuck around. The 'clean' master branch does not contain any of these residual methods,
but the 'dirty' branch does, just as a way for me to keep the old attempts as an example.

CITATIONS:

[1] D. Baraff, A.Witkin, 1998, Large Steps in Cloth Simulation. Computer Graphics (SIGGRAPH’98 proceedings), ACM Press, pp 43-54.

[2] Kang, Y. M., and Cho, H. G., 2002, Complex deformable objects in virtual reality. VRST '02 Proceedings of the ACM symposium on Virtual reality software and technology, ACM, 49-56.

[3] P. Volino, N. Magnenat-Thalmann, F. Faure, 2009, A Simple Approach to Nonlinear Tensile Stiffness for Accurate Cloth Simulation. ACM Transactions on Graphics, Association for Computing Machinery, 28(4).
