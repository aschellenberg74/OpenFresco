function newimg = ResizeImage(oldimg,newsize)
%RESIZEIMAGE to resize an image using bicubic interpolation
% newimg = ResizeImage(oldimg,newsize)
%
% newimg  : new resized image
% oldimg  : image to be resized
% newsize : new size of image [height,width]
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%                          OpenFresco Express                          %%
%%    GUI for the Open Framework for Experimental Setup and Control     %%
%%                                                                      %%
%%   (C) Copyright 2011, The Pacific Earthquake Engineering Research    %%
%%            Center (PEER) & MTS Systems Corporation (MTS)             %%
%%                         All Rights Reserved.                         %%
%%                                                                      %%
%%     Commercial use of this program without express permission of     %%
%%                 PEER and MTS is strictly prohibited.                 %%
%%     See Help -> OpenFresco Express Disclaimer for information on     %%
%%   usage and redistribution, and for a DISCLAIMER OF ALL WARRANTIES.  %%
%%                                                                      %%
%%   Developed by:                                                      %%
%%     Andreas Schellenberg (andreas.schellenberg@gmail.com)            %%
%%     Carl Misra (carl.misra@gmail.com)                                %%
%%     Stephen A. Mahin (mahin@berkeley.edu)                            %%
%%                                                                      %%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% $Revision$
% $Date$
% $URL$

oldsize = size(oldimg);
newsize = [ceil(newsize),oldsize(3)];

xi = linspace(1,oldsize(2),newsize(2));
yi = linspace(1,oldsize(1),newsize(1))';

newimg = zeros(newsize,'uint8');
for i=1:newsize(3)
    newimg(:,:,i) = interp2(oldimg(:,:,i),xi,yi,'*cubic');
end
