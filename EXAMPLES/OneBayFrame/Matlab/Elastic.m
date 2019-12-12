function ElementPost = Elastic(u, ElementData)
%ELASTIC linear elastic one dof element
% ElementPost = Elastic(u, ElementData)
%
% This is a linearly elastic material. It takes the tip displacement
% and returns Pr, the resisting force.
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
%**   Andreas Schellenberg (andreas.schellenberg@gmail.com)            **
%**   Yoshikazu Takahashi (yos@catfish.dpri.kyoto-u.ac.jp)             **
%**   Gregory L. Fenves (fenves@berkeley.edu)                          **
%**   Stephen A. Mahin (mahin@berkeley.edu)                            **
%**                                                                    **
%** ****************************************************************** */

% Written: Hong Kim (hong_kim@berkeley.edu)
% Created: 10/06
% Revision: A

% Set Parameters
k_el = ElementData.k_elem;  % Element Stiffness

% Set resisting force
ElementPost.p_r = k_el * u;

ElementPost.v_pl = 0;
ElementPost.qb   = 0;
