function Links(action, varargin)
% Links stores the web addresses which the GUI links to

%  Initialization tasks
handles = guidata(gcbf);

switch action
    case 'User Tips'
        web('https://neesforge.nees.org/docman/index.php?group_id=36&selected_doc_group_id=36&language_id=1', '-browser');
%         web('http://openfresco.neesforge.nees.org/');
end


%Update handles structure
guidata(gcbf, handles);

%Note: guidata may not be necessary

end