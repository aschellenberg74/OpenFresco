function ElementPost = EP_spring(u, ElementData)
%EP_spring nonlinear one dof element
% ElementPost = EP_spring(u, ElementData)
%
% This is an EP material using the return mapping algorithm with kinematic
% hardening, hk.  It takes the tip displacement and returns Pr, the
% resisting force.
%
%                          [+]o-----\/\/\/\----o[+]

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
k_el = ElementData.k_elem;    % Stiffness (kips/in)
Fy   = ElementData.Fy;        % Yield Strength of spring
hk   = ElementData.hk;        % Kinematic Hardening Ratio
qb   = ElementData.qb;        % Back Force
v_pl = ElementData.v_pl;      % Previous Plastic Deformation

% Get v from u
v = u;

% Get q_tr
q_tr = k_el * (v-v_pl);

% Determine Plastic flow parameter
n_tr = sign(q_tr-qb);
d_beta = (n_tr*(q_tr-qb)-Fy)/(n_tr*(k_el*(1+hk))*n_tr);

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

ElementPost.p_r = q;
