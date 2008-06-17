function [Sys, MDL, AUX] = SimConfig
MDL = MDL_RF; AUX = MDL_AUX;        % Type definition. Do not delete this line. 
% =================================================================================================
% Configuration parameters for One Bay Frame SimCor example
%
% Unit: in, kip, sec
%
% by Hong Kim, hongkim@ce.berkeley.edu
% Univ. of California at Berkeley
% 
% Last updated on 2008-01-18
% =================================================================================================

% _________________________________________________________________________________________________
%
% Common parameters
% _________________________________________________________________________________________________

% Ground acceleration file name with extension. The file should contains two
% columns for time and acceleration. The unit of acceleration should be
% consistent with the mass, time, and force. (i.e. mass*acc = force)
Sys.GM_Input = 'elcentro.dat';

% Ground acceleration scale factor. This factor will be multiplied to
% acceleration before starting simulation.
Sys.GM_SC = 386.1;

% Direction of ground acceleration. (x, y, or z)
Sys.GM_direction = 'x';

% Integration parameter related to the alpha-OS method.
% Alpha = (0 ~ 1/3). In most cases, SC.Alph = 0.05 worked.
Sys.Alph = 0.05;
Sys.Beta = 1/4*(1+Sys.Alph)^2;
Sys.Gamm = 1/2 + Sys.Alph;

% Evaluate Stiffness?
% Yes (1) to run stiffness evaluation test,
% No  (0) to read stiffness matrix from file. In this case, there should exist
%         stiffness matrices of individual module in the files MDL01_K.txt,
%         MDL02_K.txt, etc.
Sys.Eval_Stiffness = 1;

% Number of initial static loading steps. When there exist static constant
% loading,i.e. gravity forces, apply then in Zeus-NL or OpenSees as a
% incremental loading with 'n' steps. In this file, SimConfig.m, specify the
% number of static steps in the following variable.
Sys.Num_Static_Step = 0;

% Number of dynamic analysis steps
Sys.Num_Dynamic_Step = 500;

% Dynamic analysis time steps
Sys.dt = 0.02;

% Rayleigh damping, xi_1 and xi_2: Damping ratio, Tn_1, Tn_2: Target period
Sys.xi_1 = 0.00;
Sys.Tn_1 = 0.00;
Sys.xi_2 = 0.00;
Sys.Tn_2 = 0.00;

% Number of Stiffness test
% If stiffness is evaluated through experiment, the evaluation need to be done
% several times and the average of the results are used as the initial
% stiffness. This parameter is used when Sys.Eval_Stiffness = 1
Sys.Num_Test_Stiffness  = 1;

% Enable GUI for SimCor?
% Yes (1) enable the GUI for SimCor
% No  (0) disable the GUI for SimCor
%         Hybrid simulation will be run automatically.
%         Not recommended for the experiment.
Sys.EnableGUI       = 1;         % Use GUI for SimCor

% Number of restoring force modules.
Sys.Num_RF_Module     = 3;

% Number of auxilary modules.
Sys.Num_AUX_Module    = 0;

% Total number of effective nodes. Effective nodes are interface nodes between
% modules and nodes where lumped masses are defined.
Sys.Num_Node        = 2;

% Lumped mass assigned for each DOF for each node.
% Node number = x, y, z, rx, ry, rz directional mass
Sys.Node_Mass{1} = [0.04, 0.04, 0, 0, 0, 0];
Sys.Node_Mass{2} = [0.02, 0.02, 0, 0, 0, 0];

% _________________________________________________________________________________________________
%
% Restoring force module configuration
% _________________________________________________________________________________________________

% Create objects of MDL_RF
MDL(1) = MDL_RF;
MDL(2) = MDL_RF;
MDL(3) = MDL_RF;

% Name of each module.
MDL(1).name = 'ColumnLeft'; % Module ID of this module is 1
MDL(2).name = 'Truss'; % Module ID of this module is 1
MDL(3).name = 'ColumnRight'; % Module ID of this module is 2

% URL of each module
% Format - IP address:port number
% ex) 'http://cee-nsp4.cee.uiuc.edu:11997'
%     for local machine - '127.0.0.1:11997'
MDL(1).URL  = '127.0.0.1:8090';
MDL(2).URL  = '127.0.0.1:8091';
MDL(3).URL  = '127.0.0.1:8092';

% Communication protocol for each module. 
%       NTCP         : communicate through NEESPOP server
%       TCPIP        : binary communication using TCPIP
%       LabView1     : ASCII communication with LabView plugin format (Propose-Query-Execute-Query)
%       LabView2     : same as LabView1 but Propose-Query
%       OpenFresco1D : OpenFresco, only 1 DOF is implemented now.
%       NHCP         : NHCP, linear 1 DOF simulation mode, Mini MOST 1 and 2 at UIUC or SDSC
MDL(1).protocol = 'OpenFresco1D';
MDL(2).protocol = 'LabView1';
MDL(3).protocol = 'OpenFresco1D';

% Module 1: ColumnLeft ---------------------------------------------------------------------------------
MDL(1).node    = [1];           % Control point node number
MDL(1).EFF_DOF = [1 0 0 0 0 0];   % Effective DOF for CP 1

% Module 2: Truss ---------------------------------------------------------------------------------
MDL(2).node    = [1 2];             % Control point node number
MDL(2).EFF_DOF = [1 0 0 0 0 0     % Effective DOF for CP 1
                  1 0 0 0 0 0];   % Effective DOF for CP 2

% Module 3: ColumnRight ---------------------------------------------------------------------------------
MDL(3).node    = [2];         % Control point node number
MDL(3).EFF_DOF = [1 0 0 0 0 0];   % Effective DOF for CP 1


% Dismplacement for preliminary test for each module
% Del_t: Translation, Del_r: Rotation in radian
MDL(1).DEL_t = 1e-5;
MDL(2).DEL_t = 1e-5;
MDL(3).DEL_t = 1e-5;

MDL(1).DEL_r = 1e-5;
MDL(2).DEL_r = 1e-5;
MDL(3).DEL_t = 1e-5;

% Enable GUI for each module?
% GUI for each module can only display the data.
% GUI for each module can not control the hybrid simulation.
% Yes (1) enable the GUI for each module
% No  (0) disable the GUI for each module
MDL(1).EnableGUI = 1;
MDL(2).EnableGUI = 1;
MDL(2).EnableGUI = 1;

% _____________________________________________________________________________
%
% Advanced modular parameters
% _____________________________________________________________________________
% These parameters need to be redefined for following situations.
%     (1) Different coordinate system between UI-SIMCOR and static module
%     (2) When scale factor needs to be applied either in experiment or 
%         simulation
%     (3) To define force and displacement criteria (for tolerance and safety)
%     (4) To trigger camera modules or DAQ system
%     (5) When LBCB at UIUC is used for experiment
%     (6) When NHCP protocol is used
%
% URL of remote site and NHCP mode for NHCP
for i=1:Sys.Num_RF_Module
  if strcmp(lower(MDL(i).protocol), 'nhcp')
    MDL(i).remote_URL = '127.0.0.1:99999';
    MDL(i).NHCPMode = 'sim1d';
  end
end

% Stiffness for NHCP (Only valid if NHCPMode = 'Sim1D')
for i=1:Sys.Num_RF_Module
  if strcmp(lower(MDL(i).NHCPMode), 'sim1d')
    MDL(i).NHCPSimK = '6.2344023e+003';
  end
end

% Coordinate transformation. If it needs, the transformation matrix also
% needs to be provided. 
for i=1:Sys.Num_RF_Module 
  MDL(i).TransM = [];
end

% Scale factor for displacement, rotation, force, moment
% Experimental specimens are not always in full scale. Use this factors to 
% apply scale factors. 
% The displacement scale factors are multiplied before they are 
% sent to module. Measured force and moments are divided with scale factors 
% before used in the PSD algorithm.
for i=1:Sys.Num_RF_Module 
  MDL(i).ScaleF = [1 1 1 1];  % Module i
end

% Relaxation check
% If this parameter is 1, UI_SimCor send commend to retrieve data and check 
% relaxation just before the execution of proposed command. If it's 1, the 
% checking criteria needs to be provided.
for i=1:Sys.Num_RF_Module 
  MDL(i).CheckRelax  = 0;   % Module i
  % if MDL(i).CheckLimit=1, define following variables. 
  % Variable size should be (number of control nodes)* 6 array
  %
  % Displacement variation ratio (not increment)
  % MDL(i).MES_D_inc = [ a b c d e f    
    %                     ...        ];
  % Force variaiton ratio (not increment)
  % MDL(i).MES_F_inc = [ a b c d e f    
    %                     ...        ];
end

% Check displacement and force limit
% At every steps, check if the displacement or force are approaching to the 
% limitation of the equipments stroke or force capacity.
for i=1:Sys.Num_RF_Module 
  MDL(i).CheckLimit  = 0;   % Module i
  % if MDL(i).CheckLimit=1, define following variables. 
  % Variable size should be (number of control nodes)* 6 array
  %
  % Displacement increment limit(not ratio)
  % MDL(i).TGT_D_inc = [ a b c d e f    
    %                     ...        ];
    % Displacement limit
  % MDL(i).CAP_D_tot = [ a b c d e f    
    %                     ...        ];
    % Force limit
  % MDL(i).CAP_F_tot = [ a b c d e f    
    %                     ...        ];
    % Displacement tolerance (ratio)
  % MDL(i).TOL_D_inc = [ a b c d e f    
    %                     ...        ];
end

% Loading and Boundary Condition Box (LBCB) case. If it's 1, the 
% coordinate transformation matrix needs to be provided.
% This can be also used for any other actuator which has diffrence number of
% DOF coordinate with those of UI-SIMCOR

for i=1:Sys.Num_RF_Module 
  MDL(i).LBCB = 0;
end

for i=1:Sys.Num_RF_Module 
  MDL(i).LBCB_TransM = [];
end

% _________________________________________________________________________________________________
%
% Auxiliary module configuration
% _________________________________________________________________________________________________

% AUX(1)                = MDL_AUX;
% AUX(1).URL            = '127.0.0.1:12000';
% AUX(1).protocol       = 'labview1';
% AUX(1).name           = 'Camera';     % Module ID of this mdoule is 1
% AUX(1).Command        = {'displacement' 'z' 3500};