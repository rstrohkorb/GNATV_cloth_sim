# GNATV_cloth_sim
General Nonlinear Anisotropic Tensile Viscoelasticity Cloth Sim, implimented from Volino &amp; Magnenat-Thalmann 2009

Some updates:
Added RK4 integrator because I can't get conjugate gradient to work (it's supposed to minimize r but instead r explodes massively, no idea why).
In the Cloth::update() method, you can turn on/off rk4 v. conjugate gradient with comment blocks. Haven't added any fancy bools yet.

Also, occasionally stuff just decides to stop working on my home machine, so RK4 hasn't been fully tested yet. On last iteration it imploded, but slowly and in a pretty manner. I'll know more once I can test the new version on a school machine.

Boost makes my life frustrating, but whatever, their cubic b-spline interpolator is nice.
