function varargout = Etiqbutton(varargin)
% ETIQBUTTON M-file for Etiqbutton.fig
%      ETIQBUTTON, by itself, creates a new ETIQBUTTON or raises the existing
%      singleton*.
%
%      H = ETIQBUTTON returns the handle to a new ETIQBUTTON or the handle to
%      the existing singleton*.
%
%      ETIQBUTTON('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in ETIQBUTTON.M with the given input arguments.
%
%      ETIQBUTTON('Property','Value',...) creates a new ETIQBUTTON or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before Etiqbutton_OpeningFunction gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to Etiqbutton_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Copyright 2002-2003 The MathWorks, Inc.

% Edit the above text to modify the response to help Etiqbutton

% Last Modified by GUIDE v2.5 20-Aug-2006 22:12:31

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @Etiqbutton_OpeningFcn, ...
                   'gui_OutputFcn',  @Etiqbutton_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Executes just before Etiqbutton is made visible.
function Etiqbutton_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to Etiqbutton (see VARARGIN)

%Carga la imagen de fondo (opcional)
[x,map]=imread('hammerfall.jpg','jpg');
image(x),colormap(map),axis off,hold on

%Coloca una imagen en cada botón
[a,map]=imread('vol.jpg');
[r,c,d]=size(a); 
x=ceil(r/30); 
y=ceil(c/30); 
g=a(1:x:end,1:y:end,:);
g(g==255)=5.5*255;
set(handles.pushbutton1,'CData',g);

[a,map]=imread('stop.jpg');
[r,c,d]=size(a); 
x=ceil(r/30); 
y=ceil(c/30); 
g=a(1:x:end,1:y:end,:);
g(g==255)=5.5*255;
set(handles.pushbutton2,'CData',g);

[a,map]=imread('play.jpg');
[r,c,d]=size(a); 
x=ceil(r/30); 
y=ceil(c/30); 
g=a(1:x:end,1:y:end,:);
g(g==255)=5.5*255;
set(handles.pushbutton3,'CData',g);

[a,map]=imread('open_files.jpg');
[r,c,d]=size(a); 
x=ceil(r/30); 
y=ceil(c/30); 
g=a(1:x:end,1:y:end,:);
g(g==255)=5.5*255;
set(handles.pushbutton4,'CData',g);

[a,map]=imread('cd_eject.jpg');
[r,c,d]=size(a); 
x=ceil(r/35); 
y=ceil(c/35); 
g=a(1:x:end,1:y:end,:);
g(g==255)=5.5*255;
set(handles.pushbutton5,'CData',g);

[a,map]=imread('pause.jpg');
[r,c,d]=size(a); 
x=ceil(r/100); 
y=ceil(c/80); 
g=a(1:x:end,1:y:end,:);
g(g==255)=5.5*255;
set(handles.pushbutton6,'CData',g);

[a,map]=imread('mute2.jpg');
[r,c,d]=size(a); 
x=ceil(r/30); 
y=ceil(c/30); 
g=a(1:x:end,1:y:end,:);
g(g==255)=5.5*255;
set(handles.pushbutton7,'CData',g);
% Choose default command line output for Etiqbutton
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes Etiqbutton wait for user response (see UIRESUME)
% uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = Etiqbutton_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;


% --- Executes on button press in pushbutton1.
function pushbutton1_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)




% --- Executes on button press in pushbutton2.
function pushbutton2_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)




% --- Executes on button press in pushbutton3.
function pushbutton3_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)




% --- Executes on button press in pushbutton4.
function pushbutton4_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton4 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)




% --- Executes on button press in pushbutton5.
function pushbutton5_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton5 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)




% --- Executes on button press in pushbutton6.
function pushbutton6_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton6 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --- Executes on button press in pushbutton7.
function pushbutton7_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton7 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


