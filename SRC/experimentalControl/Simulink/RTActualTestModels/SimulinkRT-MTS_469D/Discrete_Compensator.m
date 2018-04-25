% clc
% clear all
% close all

%%
% samplePeriod = 1/1024;
% gamma_tf = 0.5;
% beta_tf = 0;
% length_delay = 5;
% tau_tf = length_delay*samplePeriod;
% 
% d_0_tf = 0;
% d_1_tf = 0;
% d_2_tf = (1-2*gamma_tf+2*beta_tf)*samplePeriod^3;
% d_3_tf = (1+2*gamma_tf-4*beta_tf)*samplePeriod^3;
% % d_4_tf = 2*beta_tf*controlPeriod^3;
% 
% n_0_tf = -2*beta_tf*samplePeriod^3 - 6*beta_tf*tau_tf*samplePeriod^2 - 6*beta_tf*tau_tf^2*samplePeriod  -2*beta_tf*tau_tf^3;
% n_1_tf = 8*beta_tf*samplePeriod^3 + 2*(gamma_tf+9*beta_tf)*tau_tf*samplePeriod^2 + (1+18*beta_tf)*tau_tf^2*samplePeriod  +6*beta_tf*tau_tf^3;
% n_2_tf = (1-2*gamma_tf-10*beta_tf)*samplePeriod^3 - 2*(1+9*beta_tf)*tau_tf^2*samplePeriod - 2*(1+2*gamma_tf+9*beta_tf)*tau_tf*samplePeriod^2  -6*beta_tf*tau_tf^3;
% n_3_tf = (1+2*gamma_tf+4*beta_tf)*samplePeriod^3 + (1+6*beta_tf)*tau_tf^2*samplePeriod + 2*(1+gamma_tf+3*beta_tf)*tau_tf*samplePeriod^2  +2*beta_tf*tau_tf^3;
% % n_4_tf = 0;
% num_tf = [n_3_tf n_2_tf n_1_tf n_0_tf]/d_3_tf;
% den_tf = [d_3_tf d_2_tf d_1_tf d_0_tf]/d_3_tf;

%% Adding zeros to GM

onesec = ceil(1/samplePeriod);

