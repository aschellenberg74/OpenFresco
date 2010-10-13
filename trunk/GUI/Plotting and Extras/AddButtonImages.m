data = guidata(gcf);

%Access children
kids = get(data.Sidebar(1),'Children');


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%Structure/JPG
kids(5);
struct_pic = imread('Hamburger','jpg');
set(kids(5),'CData',struct_pic); %note: does not scale image

%GM/BMP
kids(4);
[GM_pic, GM_map] = imread('Mandrill','bmp');
set(kids(4),'CData',ind2rgb(GM_pic, GM_map)); %note: does not scale image

%ExpSetup/GIF
kids(3);
[ES_pic, ES_map] = imread('Hugo','gif');
set(kids(3),'CData',ind2rgb(ES_pic, ES_map)); %note: does not scale image

%ExpControl/PNG
kids(2);
EC_pic = imread('AlphaEdge','png');
set(kids(2),'CData',EC_pic); %note: does not scale image
%Does not seem to display correctly

%Analysis
kids(1);


%No eps loading!
%Supports: bmp, cur, gif, hdf4, ico, jpeg, pbm, pcx, pgm, png, ppm, ras,
%tiff, xwd