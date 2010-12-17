function Links(action, varargin)
% Links stores the web addresses which the GUI links to

%  Initialization tasks
handles = guidata(gcbf);

switch action
    case 'User Tips'
        web('http://peer.berkeley.edu/publications/peer_reports/reports_2009/web_PEER9104_Schellenberg_etal.pdf', '-browser');
%         open('Schellenberg_etal.pdf');
    case 'Sim Material'
        objTag = get(gcbo, 'Tag');
        switch objTag
            case 'SimMatHelp'
                web('http://opensees.berkeley.edu/wiki/index.php/UniaxialMaterial_Command', '-browser');
            case 'ElasticHelp'
                web('http://opensees.berkeley.edu/wiki/index.php/Elastic_Uniaxial_Material', '-browser');
            case 'EPPHelp'
                web('http://opensees.berkeley.edu/wiki/index.php/Elastic-Perfectly_Plastic_Material', '-browser');
            case 'Steel01Help'
                web('http://opensees.berkeley.edu/wiki/index.php/Steel01_Material', '-browser');
            case 'Steel02Help'
                web('http://opensees.berkeley.edu/wiki/index.php/Steel02_Material_--_Giuffr%C3%A9-Menegotto-Pinto_Model_with_Isotropic_Strain_Hardening', '-browser');
        end
    case 'GM'
        GMchoice = questdlg(sprintf('You can find ground motions from PEER or\nCESMD by following the links below or you\ncan upload your own file.\nNote that unrecognized formats require the\ntime step to be input manually.'),...
            'Ground Motions','PEER','CESMD','OK','OK');
        switch GMchoice
            case 'PEER'
                web('http://peer.berkeley.edu/nga/search.html', '-browser');
            case 'CESMD'
                web('http://www.strongmotioncenter.org/', '-browser');
            case 'OK'
        end
    case 'MTS'
        web('http://www.mts.com/en/civil/index.asp', '-browser');
    case 'PEER'
        web('http://peer.berkeley.edu/', '-browser');
end


%Update handles structure
guidata(gcbf, handles);

%Note: guidata may not be necessary

end