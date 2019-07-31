# GNATV_cloth_sim
General Nonlinear Anisotropic Tensile Viscoelasticity Cloth Sim, implimented from Volino &amp; Magnenat-Thalmann 2009

Some updates:
Added RK4 integrator because I can't get conjugate gradient to work (it's supposed to minimize r but instead r explodes massively, no idea why).
In the Cloth::update() method, you can turn on/off rk4 v. conjugate gradient with comment blocks. Haven't added any fancy bools yet.

Rk4 still doesn't fully work. 
  No gravity: fine. At least, as far as I've watched.
  Gravity on, no corners fixed: Falls, very very very slowly, and the bottom edge folds in on itself.
  Gravity on, top two corners fixed: Exceedingly slowly, starts to droop naturally, then begins to fold in on itself. Explodes after about two minutes.

Boost makes my life frustrating, but whatever, their cubic b-spline interpolator is nice.
