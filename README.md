# GNATV_cloth_sim
General Nonlinear Anisotropic Tensile Viscoelasticity Cloth Sim, implimented from Volino &amp; Magnenat-Thalmann 2009

Some updates:
In the Cloth::update() method, you can turn on/off rk4 v. conjugate gradient with comment blocks. Haven't added any fancy bools yet.

Conjugate gradient works now! Sort of. It's still unstable and prone to exploding/long convergence times, especially when you fix some of the points, but hooray! It's sort of working! I have no idea why it works now but there you go.

Rk4 works now! Provided the starting state gives an outlet for forces in all 3 directions. 
Given an XY-plane starting cloth, there's no reason for any masspoints to venture into z-space if there's no external force goading them into it. Requires an external force in the z-direction to work, or an initial configuration that doesn't align all the points to a single plane.

Boost makes my life frustrating, but whatever, their cubic b-spline interpolator is nice.
