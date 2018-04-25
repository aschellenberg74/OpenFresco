% clc
% clear all
% close all
%% Number of signals to read from LHPOST (DoF for offset correction)
HybridCtrlParameters.nDOF = 1;  % Added on 10/19/2017
%% scale GM
scale_GM = 1.50; % change as needed
forc_fbk_gain = 1.00; % change as needed
check_exist = exist('i_MV');

if check_exist == 0
   i_MV = 0; 
end
%% NL Parameters
K2 = 0.5*4; %update from MathCAD
K1 = 4.6686*K2; % change if needed
a_ratio = K2/K1; % ratio of K2/K1 in the isolator(post yielding/initial stiffnesses)             
%% Units: kip, in
DOF_sup = 1;
DOF_sub = 1;
DOF = DOF_sup + DOF_sub;
a_vector = ones(DOF,1);
% influence vector
a_sub = ones(DOF_sub,1);
a_sup = ones(DOF_sup,1);
a_recorder_sub_sup = zeros(DOF_sub,1);
a_recorder_sub_sup(DOF_sub,1) = 1;
a_recorder_sub_sup = a_recorder_sub_sup.';
g = 386.089; %gravity - in/s^2
%% Load Ground Motion
% El_Centro = importdata('ecns (1d1024).txt');
dt = samplePeriod;
% time = 0:dt:(length(El_Centro)-1)*dt;
% El_Centro = El_Centro';
%El_Centro = [time;El_Centro];
% save('El_Centro_array','El_Centro','-v7.3')

%% Mass Matrix (Lumped mass)
% Mstory = 59.625/g;
Mstory = 56/g;
Mstr = [0.88585/(56/450) 0;
        0                1]*Mstory;
%% Material Properties
ECol = 29000;
LCol = 41*12; % height of substructure on berry st (3 stories)
% Cross section
T_target = 0.25; % Substructure before retrofit
bCol = (LCol^3*(Mstr(1,1))*(2*pi/T_target)^2/(2*ECol))^0.25;
hCol = (LCol^3*(Mstr(1,1))*(2*pi/T_target)^2/(2*ECol))^0.25;
ICol = (bCol*hCol^3)/12;
%% Stiffness of a 3 story shear frame building
Kstory = 24*ECol*ICol/LCol^3;
uy = 0.85; %update from real data
Kstr = [Kstory+K1 -K1;
        -K1       K1]; 
%% EigenAnalysis and Damping Matrix 
% [~,lamda] = eig(Mstr\Kstr);
% w1 = sqrt(lamda(2,2));
% w2 = sqrt(lamda(1,1));
% To = 2*pi/w1;
damp = 0.05;
% alphaM = 2*damp*w1*w2/(w1+w2);
w1 = 2*pi/T_target;
betaKinit = 2*damp/w1;
Cstr = betaKinit*Kstr;
%% Substructure Setup  
Msub = (0.88585/(56/450))*Mstory; 
Ksub = Kstory;
Csub = betaKinit*Ksub;
%% Initial calculations for Substructure Setup
rho_inf_sub = 1; % high-frequency spectral radius (1 = CR, 0 = maximum numerical energy dissip.
alpha_f_sub = rho_inf_sub/(rho_inf_sub+1); % Eq. 9.b
alpha_m_sub = (2*rho_inf_sub-1)/(rho_inf_sub+1); % Eq. 9.a
gamma_sub = 0.5-alpha_m_sub+alpha_f_sub; % Eq. 8.a
beta_sub = 0.25*(1-alpha_m_sub+alpha_f_sub)^2; % Eq. 8.b
alpha_sub = (Msub + gamma_sub*dt*Csub + beta_sub*(dt^2)*Ksub);
alpha1_sub = alpha_sub\Msub; % Eq. 7.a
alpha3_sub = alpha_sub\(alpha_m_sub*Msub + alpha_f_sub*gamma_sub*dt*Csub + alpha_f_sub*beta_sub*(dt^2)*Ksub); % Eq. 7.c
A_sub = (dt*alpha1_sub)/(Msub-Msub*alpha3_sub); % Eq. 19.a
B_sub = ((1/dt)*Msub*alpha3_sub)/alpha1_sub; % Eq. 19.b
D_sub = inv(alpha1_sub)/dt; % Eq. 19.c
%% Superstructure Setup  
Msup = Mstory; 
Ksup = K1;
% Csup = betaKinit*Ksup;
% Csup = 0; %No extra damping in the experimental
Csup = (2*0.01/w1)*Ksup; % 1% damping to experimental
%% Initial calculations for Superstructure Setup
rho_inf_sup = 1; % high-frequency spectral radius (1 = CR, 0 = maximum numerical energy dissip.
alpha_f_sup = rho_inf_sup/(rho_inf_sup+1); % Eq. 9.b
alpha_m_sup = (2*rho_inf_sup-1)/(rho_inf_sup+1); % Eq. 9.a
gamma_sup = 0.5-alpha_m_sup+alpha_f_sup; % Eq. 8.a
beta_sup = 0.25*(1-alpha_m_sup+alpha_f_sup)^2; % Eq. 8.b
alpha_sup = (Msup + gamma_sup*dt*Csup + beta_sup*(dt^2)*Ksup);
alpha1_sup = alpha_sup\Msup; % Eq. 7.a
alpha3_sup = alpha_sup\(alpha_m_sup*Msup + alpha_f_sup*gamma_sup*dt*Csup + alpha_f_sup*beta_sup*(dt^2)*Ksup); % Eq. 7.c
A_sup = (dt*alpha1_sup)/(Msup-Msup*alpha3_sup); % Eq. 19.a
B_sup = ((1/dt)*Msup*alpha3_sup)/alpha1_sup; % Eq. 19.b
D_sup = inv(alpha1_sup)/dt; % Eq. 19.c

%%
% open_system('KR_algorithm.mdl')
% open_system('KR_algorithm_sts.mdl')
