close all;


% f = figure;
% SS = get(0,'screensize');
% w = SS(3);
% h = SS(4);
% Cdata = imread('globe.jpg');
% % Cdata = imagesc(Cdata);
% Cdata = imresize(Cdata, [but_height but_width]);
% but = uicontrol('Style','pushbutton',...
%     'String','User Tips',...
%     'Units','normalized',...
%     'BackgroundColor',[0.3 0.5 0.7],...
%     'Callback','Links(''User Tips'')',...
%     'Position',[0.01 0.30 0.9 0.5],...
%     'CData',Cdata);
% 

% uicontrol('Style','pushbutton',...
%     'String','User Tips',...
%     'Units','normalized',...
%     'BackgroundColor',[0.3 0.5 0.7],...
%     'Callback','Links(''User Tips'')',...
%     'Position',[0.01 0.30 0.9 0.5]);
% 
% a = axes('Position',[0.01 0.30 0.9 0.5],...
%     'XTickLabel',[],'YTickLabel',[]);
% imagesc(Cdata);

%Notes:
%No map exists for .jpg
%Cannot layer axes on top of uicontrols (buttons) due to 'heavyweight'
%setting

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% axes('Parent',ph_Main,'Position',[0 0 1 1],...
%     'XTickLabel',[],'YTickLabel',[]);
% imagesc(imread('backgroundGradient.png'));


%%%Sidebar
%Calculate sidebar sizes
tabWidth = 0.8*SS(3)*0.1*0.84;
tabHeight = 0.8*SS(4)*0.5*0.16;
% structBut = imresize(imread('structure_button.png'), [tabHeight tabWidth]);
% structBut = imresize(imread('structure_rectangle.png'), [tabHeight tabWidth]);
% structBut = imresize(imread('struct2.png'), [tabHeight tabWidth]);
% structBut = imresize(imread('sav1.png'), [tabHeight tabWidth]);
% structBut = imresize(imread('sav2.png'), [tabHeight tabWidth]);
structBut = imresize(imread('gray2.png'), [(tabHeight*1.03) (tabWidth*1.03)]);
% structBut2 = imresize(imread('structOrange.png'), [tabHeight tabWidth]);
% structBut2 = imresize(imread('sav2in.png'), [tabHeight tabWidth]);
structBut2 = imresize(imread('gray3in.png'), [tabHeight tabWidth]);
GMBut = imresize(imread('GMBut.png'), [tabHeight tabWidth]);

tabWidth = 0.8*SS(3)*0.1*0.84;
tabHeight = 0.8*SS(4)*0.23*0.3;
startButton = imresize(imread('start.png'), [tabHeight tabWidth]);
pauseButton = imresize(imread('pause.png'), [tabHeight tabWidth]);
stopButton = imresize(imread('stop.png'), [tabHeight tabWidth]);



%%%Analysis
tabWidth = 0.8*SS(3)*0.81*0.5*0.3;
tabHeight = 0.8*SS(4)*0.8*0.3*0.7;
% startButton = imresize(imread('start.png'), [tabHeight tabWidth]);
% startButton = imresize(imread('molestart1.png'), [tabHeight tabWidth]);
startButton = imresize(imread('gray2.png'), [tabHeight*1.05 tabWidth*1.05]);
% pauseButton = imresize(imread('pause.png'), [tabHeight tabWidth]);
% pauseButton = imresize(imread('molepause1.png'), [tabHeight tabWidth]);
pauseButton = imresize(imread('gray2.png'), [tabHeight*1.05 tabWidth*1.05]);
% stopButton = imresize(imread('stop.png'), [tabHeight tabWidth]);
% stopButton = imresize(imread('molestop1.png'), [tabHeight tabWidth]);
stopButton = imresize(imread('gray2.png'), [tabHeight*1.05 tabWidth*1.05]);