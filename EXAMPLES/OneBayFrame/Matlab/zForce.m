function ElementPost = zForce(u, ElementData)
%ZFORCE zero force one dof element
% ElementPost = zForce(u, ElementData)
%
% This is a zero force element.  No force is returned for displacement.
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

% Return Zero values
ElementPost.p_r  = 0;
ElementPost.v_pl = 0;
ElementPost.qb   = 0;
