clear all;
clc;

handles = guidata(gcf);

handles.Model.Element{1}.tag = 1;
handles.Model.Element{1}.type = 'Element_ExpGeneric';
handles.Model.Element{1}.kInit = handles.Model.K;
handles.Model.Element{1}.ipAddr = '127.0.0.1';
handles.Model.Element{1}.ipPort = 8090;
%handles.Model.Element{1}.id = 1;
handles.Model.Element{1}.id = [1 2];