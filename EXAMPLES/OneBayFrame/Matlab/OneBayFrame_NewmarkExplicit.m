%ONEBAYFRAME_NEWMARK to perform a hybrid simulation of a one-bay-frame
%
% This program interfaces with OpenFresco. The structure is a two
% degrees of freedom system as seen in the diagram below.  It uses
% the explicit Newmark time integration scheme.
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

% $Revision: $
% $Date: $
% $URL: $

% Written: Hong Kim (hong_kim@berkeley.edu)
% Created: 10/06
% Revision: A


close all;
clear all;

%%%%%%%%%%%%%%%%%%%% Load Earthquake Data - "El Centro" %%%%%%%%%%%%%%%%%%%

dt =.020;                        % Set time step for analysis, dt (sec)
g = 386.4;                       % Gravity (in/sec^2)
accel = load ('elcentro.txt');   % Load Elcentro Data
a_g = g * accel';
b = [1; 1];                      % Load application vector
nsteps = length (accel)-1;       % number of steps


%%%%%%%%%%%%%%%%%%%%%%%%% Setup Connection %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

socketID = TCPSocket('openConnection','127.0.0.1',8090);
if (socketID<0)
   errordlg('Unable to setup connection.');
   return;
end

% set the data size for the experimental site
dataSize = 2;
sData = zeros(1,dataSize);
dataSizes = int32([1 0 0 0 0, 0 0 0 1 0, dataSize]);
TCPSocket('sendData',socketID,dataSizes,11);


%%%%%%%%%%%%%%%%%%%%%%% Define Parameters/Variables %%%%%%%%%%%%%%%%%%%%%%%

% Number of Degrees of Freedom in the system
ndof = 2;

% Define Mass at each DoF and Damping Ratio
mass(1) = 0.04;   %Mass for DoF 1 (kips/g)
mass(2) = 0.02;   %Mass for DoF 2 (kips/g)
zeta    = 0.05;   %Damping Ratio for both modes

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
for e=1:3
   if ElementData(e).type == 0;           %Element Turned Off
      MatType(e,1:6) = 'zForce';
      ElementData(e).k_el=0;

   elseif ElementData(e).type == 1;       %Analytical Elastic Element
      MatType(e,1:7) = 'Elastic';

   elseif ElementData(e).type == 2;
      if e == 3
         MatType(e,1:9) = 'EP_spring';    %Analytical EP spring element
      else
         MatType(e,1:2) = 'EP';           %Analytical EP Element
      end

   else
      MatType(e,1:12) = 'Experimental';   %Experimental Element

   end
end

% Derived Parameters

% Mass Matrix, M
M = [mass(1)  0;  0  mass(2)];

% Initial Elastic Stiffness Matrix, K_el
K_el = [ElementData(1).k_elem + ElementData(3).k_elem   -ElementData(3).k_elem;
       -ElementData(3).k_elem    ElementData(2).k_elem + ElementData(3).k_elem];

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

% Initialize resisting force vectors
p_elem(1:3,nsteps) = zeros;         %Resisting force of each element
P_r(1:ndof,nsteps) = zeros;         %Resisting force of each DoF

% Time vector
t = dt*(0:nsteps);

% Initialize Time Integration Variables
U         (1:ndof,nsteps) = zeros;     %Displacement
U_dot     (1:ndof,nsteps) = zeros;     %Velocity
U_dot_pred(1:ndof,nsteps) = zeros;     %Velocity predictor
u_elem    (1:3,nsteps)    = zeros;

% Newmark Parameters Beta and Gamma
beta  = 0;     %Set to be explicit
gamma = 1/2;


%%%%%%%%%%%%%%%%%%%%%%%%%%%% Newmark's Method %%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Calculate initial acceleration
P(:,1) = -M * b * a_g(1);       %Applied force
U_dotdot(:,1) = M \ (P(:,1) - C*U_dot(:,1) - P_r(:,1));

% Calculate m_effective
M_hat = M + gamma*dt*C + beta*dt^2*K_el;

% Calculations for each time steps
for i = 1:nsteps;

   % Calculate U(:,i+1) using explicit NM method
   U(:,i+1) = U(:,i) + dt*U_dot(:,i) + (0.5-beta)*dt^2*U_dotdot(:,i);

   % Calculate U_dot_pred(:,i+1)
   U_dot_pred(:,i+1) = U_dot(:,i) + (1-gamma)*dt*U_dotdot(:,i);

   % Transform Global DoF to Element DoF
   A = [ 1 0; 0 1; 1 -1];   %Transformation Matrix, A
   u_elem(:,i+1) = A * U(:,i+1);

   % Input displace, U, and get resisting force, P_r for each element
   for e = 1:3

      ElementPost = feval(MatType(e,:), u_elem(e,i+1), ElementData(e));
      p_elem(e,i+1) = ElementPost.p_r;
      ElementData(e).v_pl = ElementPost.v_pl;
      ElementData(e).qb = ElementPost.qb;

   end

   % Calculate P_r for each DoF
   P_r(:,i+1) = A' * p_elem(:,i+1);

   % Calculate P_hat-rhs of equation
   P(:,i+1)   = -M*b*a_g(:,i+1);
   P_hat(:,i) = -C*U_dot_pred(:,i+1) - P_r(:,i+1) + P(:,i+1);

   % Calculate U_dotdot
   U_dotdot(:,i+1) = M_hat \ P_hat(:,i);

   % Calculate U_dot = U_dot_pred + U_dot_corrector
   U_dot(:,i+1) = U_dot_pred(:,i+1) + (gamma*dt)*U_dotdot(:,i+1);

end


%%%%%%%%%%%%%%%% Disconnect from Experimental Site %%%%%%%%%%%%%%%%%%%%%%%%

sData(1) = 99;
TCPSocket('sendData',socketID,sData,dataSize);
TCPSocket('closeConnection',socketID);


%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Post-Processing %%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Calculate maxium forces in each element
p_r_max(1) = max(abs(p_elem(1,:)));
p_r_max(2) = max(abs(p_elem(2,:)));
p_r_max(3) = max(abs(p_elem(3,:)));
% Calculate maxium displacements at each DoF
U_max(1) = max(abs(U(1,:)));
U_max(2) = max(abs(U(2,:)));

% Plot element force vs. element deformation
figure(1) ;
plot(u_elem(1,:),p_elem(1,:),'r','LineWidth',1.2)
hold on
plot(u_elem(2,:),p_elem(2,:),'b','LineWidth',1.2)
ylabel('p_e_l_e_m [kip]')
xlabel('Displacement [in.]')
legend('Element 1','Element 2',2)
title(['Explicit Newmark: Hysteresis beta = ', num2str(beta), ' and gamma = ', num2str(gamma)])
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
title(['Explicit Newmark: Displacement vs. Time beta = ', num2str(beta), ' and gamma = ', num2str(gamma)])
grid on
