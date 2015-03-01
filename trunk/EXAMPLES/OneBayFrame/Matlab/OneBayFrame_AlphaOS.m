%ONEBAYFRAME_ALPHAOS to perform a hybrid simulation of a one-bay-frame
%
% This program interfaces with OpenFresco. The structure is a two
% degrees of freedom system as seen in the diagram below.  It uses
% the alpha operator splitting method.
%
%                        DoF 1     Element 3     DoF 2
%                          [+]o-->--\/\/\/\----o[+]-->
%                           |                    |
%                 Element 1 |                    |  Element 2
%                           |                    |
%                           |                    |
%                        --[+]--              --[+]--
%                        ///////              ///////

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


close all;
clear all;
tic;

%%%%%%%%%%%%%%%%%%%% Load Earthquake Data - "El Centro" %%%%%%%%%%%%%%%%%%%

dt =.020;                        % Set time step for analysis, dt (sec)
g = 386.4;                       % Gravity (in/sec^2)
accel = load ('elcentro.txt');   % Load Elcentro Data
a_g = g * accel;
b = [1; 1];                      % Load application vector
nsteps = length (accel)-1;       % number of steps


%%%%%%%%%%%%%%%%%%%%%%%%% Setup Connection %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

socketID = UDPSocket('openConnection','127.0.0.1',8090);
if (socketID<0)
   errordlg('Unable to setup connection.');
   return;
end

% set the data size for the experimental site
dataSize = 2;
sData = zeros(1,dataSize);
dataSizes = int32([1 0 0 0 0, 0 0 0 1 0, dataSize]);
UDPSocket('sendData',socketID,dataSizes,11);


%%%%%%%%%%%%%%%%%%%%%%% Define Parameters/Variables %%%%%%%%%%%%%%%%%%%%%%%

% Number of Degrees of Freedom in the system
ndof = 2;

% Define Mass at each DoF and Damping Ratio
mass(1) = 0.04;     % Mass for DoF 1 (kips/g)
mass(2) = 0.02;     % Mass for DoF 2 (kips/g)
zeta    = 0.05;     % Damping Ratio for both modes

% Element 1 Parameters - Column Element
ElementData(1).type     = 3;          %Type of Element: 0=turn off,1=analytical Elastic,2=analytical-EP with Kinematic Hard,3=Experimental
ElementData(1).len      = 54;         %Element Length (in.)
ElementData(1).k_elem   = 2.8;        %Elastic Stiffness (kips/in)
ElementData(1).My       = 81;         %Plastic Capacity (kip*in)
ElementData(1).hk       = 0.01;       %Kinematic Hardening Ratio
ElementData(1).qb       = 0;          %back force
ElementData(1).v_pl     = 0;          %Element Plastic Deformation
ElementData(1).socketID = socketID;   %Socket ID
ElementData(1).dataSize = dataSize;   %size of send and receive vectors

% Element 2 Parameters - Column Element
ElementData(2).type     = 1;          %Type of Element: 0=turn off,1=analytical Elastic,2=analytical-EP with Kinematic Hard,3=Experimental
ElementData(2).len      = 54;         %Element Length (in.)
ElementData(2).k_elem   = 5.6;        %Elastic Stiffness (kips/in)
ElementData(2).My       = 162;        %Plastic Capacity (kip*in)
ElementData(2).hk       = 0.01;       %Kinematic Hardening Ratio
ElementData(2).qb       = 0;          %back force
ElementData(2).v_pl     = 0;          %Element Plastic Deformation
%ElementData(2).socketID = socketID;   %Socket ID
%ElementData(2).dataSize = dataSize;   %size of send and receive vectors

% Element 3 Parameters - Spring Element
ElementData(3).type     = 1;          %Type of Element: 0=turn off,1=analytical Elastic,2=analytical-EP with Kinematic Hard,3=Experimental
ElementData(3).k_elem   = 2.0;        %Elastic Stiffness (kips/in)
ElementData(3).Fy       = 25;         %Yield Strength (ksi)
ElementData(3).hk       = 0.01;       %Kinematic Hardening Ratio
ElementData(3).qb       = 0;          %back force
ElementData(3).v_pl     = 0;          %Element Plastic Deformation
%ElementData(3).socketID = socketID;   %Socket ID
%ElementData(3).dataSize = dataSize;   %size of send and receive vectors

% Determine Each Element Type
MatType = cell(3,1);
for e=1:3
   if ElementData(e).type == 0;           %Element Turned Off
      MatType{e} = 'zForce';
      ElementData(e).k_el=0;

   elseif ElementData(e).type == 1;       %Analytical Elastic Element
      MatType{e} = 'Elastic';

   elseif ElementData(e).type == 2;
      if e == 3
         MatType{e} = 'EP_spring';    %Analytical EP spring element
      else
         MatType{e} = 'EP';           %Analytical EP Element
      end

   else
      MatType{e} = 'Experimental';   %Experimental Element

   end
end

% Derived Parameters

% Mass Matrix, M
M = [mass(1)  0;  0  mass(2)];

% Initial Elastic Stiffness Matrix, K_el
K_el = [ElementData(1).k_elem+ElementData(3).k_elem  -ElementData(3).k_elem;
       -ElementData(3).k_elem  ElementData(2).k_elem+ElementData(3).k_elem];

% Calculate natural frequencies and periods
w2 = eig (K_el,M);
w  = sort(sqrt(w2));     %frequencies sorted
T  = w.\(2*pi)

% Mass Proportional Damping Matrix, C
a_o = zeta(1) * 2 * w(1);
C = a_o*M;

% Rayleigh Damping Matrix, C
% a_o = zeta(1) * 2 * w(1) * w(2) / (w(1)+w(2));
% a_1 = zeta(1) * 2 / (w(1)+w(2));
% C = a_o*M + a_1*K_el;

% Time vector
t = dt*(0:nsteps);

% Initialize Time Integration variables
U(1:2,nsteps)          = zeros;     %Displacement
U_pred(1:2,nsteps)     = zeros;     %Displ predictor

U_dot(1:2,nsteps)      = zeros;     %Velocity
U_dot_pred(1:2,nsteps) = zeros;     %Vel. predictor

P_r(1:2,nsteps)        = zeros;     %Resisting force
P_r_pred(1:2,nsteps)   = zeros;     %Resisting force predictor
P_hat(1:2,nsteps)      = zeros;     %p_hat according to Eq. 9
Pu(1:2,nsteps)         = zeros;     %Initial unbalance force Pu = P-Pr
p_r(1:3,nsteps)        = zeros;     %Resisting force for each element

% Alpha OS Parameters - alpha, beta and gamma
alpha = -1/6;           %alpha [-1/3,0]
beta  = (1-alpha)^2/4;
gamma = (1-2*alpha)/2;


%%%%%%%%%%%%%%%%% Alpha OS Method (Combesure&Pegon-p.429) %%%%%%%%%%%%%%%%%

%Calculate initial acceleration
P(:,1) = -M*b*a_g(1);   %applied force
U_dotdot(:,1) = M \ (P(:,1)-C*U_dot(:,1)-P_r(:,1));

% Calculate m_hat (Eq. 8)
M_hat = M + gamma*dt*(1+alpha)*C + beta*dt^2*(1+alpha)*K_el;

% Calculations for each time steps
for i = 1:nsteps;

   % Input Excitation p(i+1)
   P(:,i+1) = -M*b*a_g(i+1);

   % Compute u_pred(i+1) and u_dot_pred(i+1) (Eq. 2)
   U_pred(:,i+1) = U(:,i) + dt*U_dot(:,i) +dt^2/2*(1-2*beta)*U_dotdot(:,i);
   U_dot_pred(:,i+1) = U_dot(:,i) + dt*(1-gamma)*U_dotdot(:,i);

   % Input displace, U, and get resisting force, P_r for each element
   for e = 1:3

      if e == 3;  % Determine if Element is a Spring Element
         ElementPost = feval(MatType{e}, (U_pred(1,i+1)-U_pred(2,i+1)), ElementData(e));
      else
         ElementPost = feval(MatType{e}, U_pred(e,i+1), ElementData(e));
      end

      p_r(e,i+1) = ElementPost.p_r;
      ElementData(e).v_pl = ElementPost.v_pl;
      ElementData(e).qb = ElementPost.qb;

   end

   % Calculate P_r for each DoF
   P_r_pred(1,i+1) = p_r(1,i+1) + p_r(3,i+1);
   P_r_pred(2,i+1) = p_r(2,i+1) - p_r(3,i+1);

   % Compute p_hat(i+1) according to Eq.9
   P_hat(:,i+1) = (1+alpha)*P(:,i+1) - alpha*P(:,i) + alpha*P_r_pred(:,i) - (1+alpha)*P_r_pred(:,i+1) + alpha*C*U_dot_pred(:,i) - (1+alpha)*C*U_dot_pred(:,i+1) + alpha*(gamma*dt*C+beta*dt^2*K_el)*U_dotdot(:,i);

   % Solve for u_dotdot(i+1) according to Eq. 7
   U_dotdot(:,i+1) = M_hat \ P_hat(:,i+1);

   % Compute u(i+1) and u_dot(i+1) according to Eq. 3
   U(:,i+1) = U_pred(:,i+1) + (dt^2)*beta*U_dotdot(:,i+1);
   U_dot(:,i+1) = U_dot_pred(:,i+1) + dt*gamma*U_dotdot(:,i+1);

   % Calculate p_r and and pu
   P_r(:,i+1) = K_el*U(:,i+1) + (P_r_pred(:,i+1) - K_el*U_pred(:,i+1));
   %pu(i+1) = (1+alpha)*p(i+1) - alpha*p(i) - (1+alpha)*p_r(i+1) + alpha*p_r(i) - (1+alpha)*c*u_dot(i+1) + alpha*c*u_dot(i) - m*u_dotdot(i+1);

end


%%%%%%%%%%%%%%%% Disconnect from Experimental Site %%%%%%%%%%%%%%%%%%%%%%%%

sData(1) = 99;
UDPSocket('sendData',socketID,sData,dataSize);
UDPSocket('closeConnection',socketID);
toc;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Post-Processing %%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% p_r_max(1) = max(abs(p_r(1,:)));
% p_r_max(2) = max(abs(p_r(2,:)));
% p_r_max(3) = max(abs(p_r(3,:)))
%
% U_max(1) = max(abs(U(1,:)));
% U_max(2) = max(abs(U(2,:)));

% Plot element force vs. element deformation
figure(1) ;
plot(U_pred(1,:),p_r(1,:),'r','LineWidth',1.2)
hold on
plot(U_pred(2,:),p_r(2,:),'b','LineWidth',1.2)
ylabel('p_elem [kip]')
xlabel('Displacement [in.]')
legend('Element 1','Element 2',2)
title('Alpha OS')
grid on
hold off

% Plot displacement vs. time at each DoF
figure(2) ;
plot(t,U(1,:),'r','LineWidth',1.2)
hold on
plot(t,U(2,:),'b','LineWidth',1.2)
ylabel('Displacement [in.]')
xlabel('Time [sec]')
legend('DOF 1','DOF 2',1)
title('Alpha OS')
grid on
