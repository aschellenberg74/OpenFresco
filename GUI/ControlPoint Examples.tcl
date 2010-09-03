Model 1DOF:
===========
# expControlPoint tag nodeTag dir resp <-fact f> <-lim l u> ...
expControlPoint 1 1  ux disp -fact 1.0 -lim -7.5 7.5
expControlPoint 2 1  ux disp -fact 1.0 -lim -7.5 7.5  ux force -fact 1.0 -lim -12.0 12.0


Model 2DOFA:
============
# expControlPoint tag nodeTag dir resp <-fact f> <-lim l u> ...
expControlPoint 1 1  ux disp -fact 1.0 -lim -7.5 7.5
expControlPoint 2 1  ux disp -fact 1.0 -lim -7.5 7.5  ux force -fact 1.0 -lim -12.0 12.0
expControlPoint 3 2  ux disp -fact 1.0 -lim -7.5 7.5
expControlPoint 4 2  ux disp -fact 1.0 -lim -7.5 7.5  ux force -fact 1.0 -lim -12.0 12.0


Model 1DOF:
===========
# expControlPoint tag nodeTag dir resp <-fact f> <-lim l u> ...
expControlPoint 1 1  ux disp -fact 1.0 -lim -7.5 7.5  uy disp -fact 1.0 -lim -7.5 7.5
expControlPoint 2 1  ux disp -fact 1.0 -lim -7.5 7.5  uy disp -fact 1.0 -lim -7.5 7.5  ux force -fact 1.0 -lim -12.0 12.0  uy force -fact 1.0 -lim -12.0 12.0