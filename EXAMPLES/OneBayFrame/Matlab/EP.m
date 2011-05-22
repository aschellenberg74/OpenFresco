function ElementPost = EP(u, ElementData)
%EP nonlinear one dof element
% ElementPost = EP(u, ElementData)
%
% This is an EP material using the return mapping algorithm with kinematic
% hardening, hk.  It takes the tip displacement and returns Pr, the
% resisting force.
%
%                           [+]-->1 DOF
%                            |
%                            |
%                            |
%                            |
%                         --[+]--
%                         ///////

%/* ****************************************************************** **
%**    OpenFRESCO - Open Framework                                     **
%**                 for Experimental Setup and Control                 **
%**                                                                    **
%**                                                                    **
%** Copyright (c) 2006, The Regents of the University of California    **
%** All Rights Reserved.                                               **
%**                                                                    **
%** Commercial use of this program without express permission of the   **
%** University of California, Berkeley, is strictly prohibited. See    **
%** file 'COPYRIGHT_UCB' in main directory for information on usage    **
%** and redistribution, and for a DISCLAIMER OF ALL WARRANTIES.        **
%**                                                                    **
%** Developed by:                                                      **
%**   Andreas Schellenberg (andreas.schellenberg@gmx.net)              **
%**   Yoshikazu Takahashi (yos@catfish.dpri.kyoto-u.ac.jp)             **
%**   Gregory L. Fenves (fenves@berkeley.edu)                          **
%**   Stephen A. Mahin (mahin@berkeley.edu)                            **
%**                                                                    **
%** ****************************************************************** */

% $Revision$
% $Date$
% $URL$

% Written: Hong Kim (hong_kim@berkeley.edu)
% Created: 10/06
% Revision: A


% Set Parameters
len  = ElementData.len;           % Element Length
k_el = ElementData.k_elem*len^2;  % Stiffness (kips*in)
Mp   = ElementData.My;            % Plastic Moment Capcity
hk   = ElementData.hk;            % Kinematic hardening ratio
qb   = ElementData.qb;            % Back Force
v_pl = ElementData.v_pl;          % Previous Plastic Deformation

% Get v from u
v = u/len;  %Element deformation (base rotation)

% Get q_tr (trial basic force)
q_tr = k_el * (v-v_pl);

% Determine Plastic flow parameter
n_tr   = sign(q_tr-qb);
d_beta = (n_tr*(q_tr-qb)-Mp)/(n_tr*(k_el*(1+hk))*n_tr);

if d_beta > 0
    dv_pl = n_tr*d_beta;
    q = q_tr - k_el*n_tr*d_beta;
    k = k_el * hk;
    ElementPost.v_pl = v_pl + dv_pl;
    ElementPost.qb = qb + k_el*hk*n_tr*d_beta;
else
    q = q_tr;
    k = k_el;
    ElementPost.v_pl = v_pl;
    ElementPost.qb = qb;
end

ElementPost.p_r = q/len;
