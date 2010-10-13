function Links(action, varargin)
% Links stores the web addresses which the GUI links to

%  Initialization tasks
handles = guidata(gcbf);

switch action
    case 'User Tips'
        web('http://peer.berkeley.edu/publications/peer_reports/reports_2009/web_PEER9104_Schellenberg_etal.pdf', '-browser');
%         open('Schellenberg_etal.pdf');
end


%Update handles structure
guidata(gcbf, handles);

%Note: guidata may not be necessary

end