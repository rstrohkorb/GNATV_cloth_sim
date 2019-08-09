# GNATV_cloth_sim
General Nonlinear Anisotropic Tensile Viscoelasticity Cloth Sim, implimented from Volino &amp; Magnenat-Thalmann 2009

Some updates:
UI is here! Unfortunately, sim is slow on the hires objects, so I'm going to add options to bake out sequences. Also going to try to go back to my old Mass-Spring build to grab some comparison renders/playbacks.

Conjugate gradient works now! It explodes without damping, but with damping converges very slowly. This would be better if I had data relating change in strain to stress, but I don't have that, so I can settle for slow.

Rk4 works now! Provided the starting state gives an outlet for forces in all 3 directions. 
Given an XY-plane starting cloth, there's no reason for any masspoints to venture into z-space if there's no external force goading them into it. Requires an external force in the z-direction to work, or an initial configuration that doesn't align all the points to a single plane.

Boost makes my life frustrating, but whatever, their cubic b-spline interpolator is nice.
