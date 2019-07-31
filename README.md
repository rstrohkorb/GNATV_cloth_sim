# GNATV_cloth_sim
General Nonlinear Anisotropic Tensile Viscoelasticity Cloth Sim, implimented from Volino &amp; Magnenat-Thalmann 2009

Some updates:
Added RK4 integrator because I can't get conjugate gradient to work (it's supposed to minimize r but instead r explodes massively, no idea why).
In the Cloth::update() method, you can turn on/off rk4 v. conjugate gradient with comment blocks. Haven't added any fancy bools yet.

Rk4 works now! Provided the starting state gives an outlet for forces in all 3 directions. 
Given an XY-plane starting cloth, there's no reason for any masspoints to venture into z-space if there's no external force goading them into it. Requires an external force in the z-direction to work, or an initial configuration that doesn't align all the points to a single plane.

Boost makes my life frustrating, but whatever, their cubic b-spline interpolator is nice.
