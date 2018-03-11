function [x,deltaT,desc,units,fileInfo,fileDate,header,isText] = loadbin(fileName,silent)
% Load MTS time series data file.
% [x,deltaT,desc,units,fileInfo,fileDate,header,isText] = loadbin(fileName,silent)
%
%	fileName :  File name text.
%	silent	 :  Silent error messages flag (optional) (if nonzero, an error
%	            returns error message in "fileInfo" with all other fields empty,
%	            otherwise error message is returned to command window.
%	x        :  Time series data (one column per channel).
%	deltaT   :  Sample interval.
%	desc     :  Channel descriptors text array (one row per channel).
%	units    :  Channel units text array (one row per channel).
%	fileInfo :  File information text.
%	fileDate :  File creation date text.
%	header   :  Header text (returned as string matrix:
%	            ['key1'; 'value1'; 'key2'; 'value2'; ...]).
%	isText   :  File was written as text instead of binary.
%
% 	size(x, 1) returns the number of data points per channel.
% 	size(x, 2) returns the number of channels.
%
%	See also "savebin".
%
% Written: Brad Thoen 28-Jun-00.
% Copyright (c) 2000 by MTS Systems Corporation.
%
% $Revision$
% $Date$
% $URL$

x 	 = [];
deltaT	 = [];
desc	 = [];
units	 = [];
fileInfo = [];
fileDate = [];
header   = [];
message	 = [];
isText	 = [];
if ~exist('silent')
    silent = [];
end
if ~exist('fileName')
    fileName = [];
end
if isempty(fileName)
    silent = 0;
end
if isempty(fileName)
    message = 'File name is a mandatory input to loadbin.';
    if silent
        fileInfo = message;
    else
        error(message);
    end
    return
end

% open file
[f, message] = fopen(fileName, 'r');
if f < 3
    if silent
        fileInfo = message;
    else
        error(message);
    end
    return
end

% read and interpret fixed header record #1 to determine
% if file is valid MTS series file
hdr = fread(f, 129, 'char');
key = 'FILE_TYPE';
val = 'TIME_SERIES';
if ~strcmp(char(hdr(1:1+length(key)-1)'), key)
    message = 'File is not an MTS series file.';
else
    val1 = 'TIME_SERIES_WITH_EXTENDED_HEADER';
    val2 = 'TIME_SERIES';
    if strcmp(char(hdr(33:33+length(val1)-1)'), val1)
        extHdr = 1;
    elseif strcmp(char(hdr(33:33+length(val2)-1)'), val2)
        extHdr = 0;
    else
        message = 'File is not an MTS series file.';
    end
end
if ~isempty(message)
    if silent
        fileInfo = message;
    else
        error(message);
    end
    return
end

% determine whether file is text or binary format by looking for
% carriage return/line feed character
crlf = 10;
if isempty(find(hdr == crlf))
    isText = 0;	% binary
else
    isText = 1;	% text
end

% start over, now that we know whether to read the file as binary or as text
frewind(f);			% go back to beginning of file
hdr = freadHdr(f, isText);	% advance over fixed header record #1
header = char(char(hdr(1:32))', char(hdr(33:128))');

% read and interpret fixed header records #2 - #5
channels = 0;
key1 = 'FILE_DATE';
key2 = 'FILE_INFO';
key3 = 'DELTA_T';
key4 = 'CHANNELS';
for i = 2:5
    hdr = freadHdr(f, isText);
    header = char(header, char(hdr(1:32))', char(hdr(33:128))');
    if strcmp(char(hdr(1:1+length(key1)-1)'), key1)
        fileDate = char(hdr(33:128))';
    elseif strcmp(char(hdr(1:1+length(key2)-1)'), key2)
        fileInfo = char(hdr(33:128))';
    elseif strcmp(char(hdr(1:1+length(key3)-1)'), key3)
        deltaT = str2num(char(hdr(33:128))');
    elseif strcmp(char(hdr(1:1+length(key4)-1)'), key4)
        channels = str2num(char(hdr(33:128))');
    end
end

% read and interpret channel descriptor and units header records
% (must follow fixed header section; can be in any order within)
desc  = zeros(channels, 96);
units = zeros(channels, 96);
for i = 1:2*channels
    hdr  = freadHdr(f, isText);
    header = char(header, char(hdr(1:32))', char(hdr(33:128))');
    key1 = 'DESC.CHAN_';
    len1 = length(key1);
    key2 = 'UNITS.CHAN_';
    len2 = length(key2);
    if strcmp(char(hdr(1:1+len1-1)'), key1)
        chanNum = str2num(char(hdr((len1+1):32)'));
        if ~isempty(chanNum)
            desc(chanNum,:) = hdr(33:128)';
        end
    elseif strcmp(char(hdr(1:1+len2-1)'), key2)
        chanNum = str2num(char(hdr((len2+1):32)'));
        if ~isempty(chanNum)
            units(chanNum,:) = hdr(33:128)';
        end
    end
end
desc  = char(desc);
units = char(units);

% read extended header records
% (must follow channel descriptor and units header section)
if (extHdr)
    
    % read number of extended header records
    hdr = freadHdr(f, isText);
    header = char(header, char(hdr(1:32))', char(hdr(33:128))');
    key = 'EXTENDED_HEADER_RECORDS';
    if strcmp(char(hdr(1:1+length(key)-1)'), key)
        extHdrRecs = str2num(char(hdr(33:128))');
    else
        extHdrRecs = 0;
    end
    
    % read extended header records
    for i = 1:extHdrRecs
        hdr = freadHdr(f, isText);
        header = char(header, char(hdr(1:32))', char(hdr(33:128))');
    end
end

% read multiplexed data
[data, filePoints] = freadData(f, isText);

% demultiplex channel data into columns
for i = 1:channels
    x(:,i) = data(i:channels:filePoints);
end

% close file
fclose(f);

function hdr = freadHdr(f, isText)
if isText
    hdr = fgetl(f);
    len = length(hdr);
    if len < 128
        hdr = [char(hdr), blanks(128-len)];	% pad to 128 chars
    else
        hdr = hdr(1:128);			% clip to 128 chars
    end
    hdr = hdr(:);
else
    hdr = fread(f, 128, 'char');
end

function [data, npts] = freadData(f, isText)
if isText
    [data, npts] = fscanf(f, '%f', inf);
else
    [data, npts] = fread(f, inf, 'float32');
end
