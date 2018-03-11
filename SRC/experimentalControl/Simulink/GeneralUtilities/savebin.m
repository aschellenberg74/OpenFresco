function savebin(fileName, x, deltaT, desc, units, fileInfo, header, asText)
% Save data to MTS binary time series data file.
%	savebin(fileName, x, deltaT, desc, units, fileInfo, header, asText)
%
%	fileName :  File name text.
%	x        :  N x M time series data matrix (M channels of N data points)
%	            (optional; header is written if x is an empty matrix).
%	deltaT   :  Sample interval (optional; default = 1.0).
%	desc     :  Channel descriptors text array (one row per channel) (optional)
%	            (assemble with char():  char('desc1', 'desc2', ...)).
%	units    :  Channel units text array (one row per channel) (optional).
%	            (assemble with char():  char('units1', 'units2', ...)).
%	fileInfo :  File information text (optional).
%	header   :  Header text (optional).
%	            (assemble keyword/value pairs with char():
%	            char('key1', 'value1', 'key2', 'value2', ...)).
%	asText   :  Write the file as text instead of binary (optional).
%
%	See also "loadbin".
%
% Written: Brad Thoen 10-Jul-00.
% Copyright (c) 2000 by MTS Systems Corporation.
%
% $Revision$
% $Date$
% $URL$

% input argument checking
if ~exist('fileName')
    error('File name is a mandatory input to savebin.')
end
if ~exist('x')
    error('Data is a mandatory input to savebin.')
end
if ~exist('deltaT')
    deltaT = [];
end
if ~exist('desc')
    desc = [];
end
if ~exist('units')
    units = [];
end
if ~exist('fileInfo')
    fileInfo = [];
end
if ~exist('header')
    header = [];
end
if ~exist('asText')
    asText = [];
end
if isempty(deltaT)
    deltaT = 1.0;
end
if isempty(desc)
    desc = '';
end
if isempty(units)
    units = '';
end
if isempty(desc)
    fileInfo = ' ';
end
if isempty(asText)
    asText = 0;
end
channelPoints = size(x, 1);
channels = size(x, 2);

% open file
[f, message] = fopen(fileName, 'w');
if f < 3
    error(message)
end
newhdr = [];

% assemble file type header record
key    = 'FILE_TYPE';
header = removeHeaderRecord(header, key);
newhdr = addHeaderRecord(newhdr, key, 'TIME_SERIES');

% assemble file date header record
key    = 'FILE_DATE';
time   = clock;
if time(4) < 10
    str = [date, ' ', '0', int2str(time(4)), ':'];
else
    str = [date, ' ', int2str(time(4)), ':'];
end
if time(5) < 10
    str = [str, '0', int2str(time(5)), ':'];
else
    str = [str, int2str(time(5)), ':'];
end
if time(6) < 10
    str = [str, '0', int2str(time(6))];
else
    str = [str, int2str(time(6))];
end
header = removeHeaderRecord(header, key);
newhdr = addHeaderRecord(newhdr, key, str);

% assemble file info header record
key    = 'FILE_INFO';
header = removeHeaderRecord(header, key);
newhdr = addHeaderRecord(newhdr, key, fileInfo);

% assemble sample interval header record
key    = 'DELTA_T';
header = removeHeaderRecord(header, key);
newhdr = addHeaderRecord(newhdr, key, num2str(deltaT, 12));

% assemble channel count header record
key    = 'CHANNELS';
header = removeHeaderRecord(header, key);
newhdr = addHeaderRecord(newhdr, key, int2str(channels));

% assemble channel descriptor and units header records
for i = 1:channels
    
    % write channel descriptor header record
    key = ['DESC.CHAN_', int2str(i)];
    if size(desc, 1) >= i
        str = desc(i, :);
    else
        str = ' ';
    end
    header = removeHeaderRecord(header, key);
    newhdr = addHeaderRecord(newhdr, key, str);
    
    % write channel units header record
    key = ['UNITS.CHAN_', int2str(i)];
    if size(units, 1) >= i
        str = units(i, :);
    else
        str = ' ';
    end
    header = removeHeaderRecord(header, key);
    newhdr = addHeaderRecord(newhdr, key, str);
end

% assemble extended header
key    = 'EXTENDED_HEADER_RECORDS';
header = removeHeaderRecord(header, key);
nxrecs = size(header, 1) / 2;
if (nxrecs)
    % modify file type header record
    value  = 'TIME_SERIES_WITH_EXTENDED_HEADER';
    newhdr(1,33:33+length(value)-1) = value;
    
    % assemble number of extended header records
    newhdr = addHeaderRecord(newhdr, key, int2str(nxrecs));
    
    % assemble extended header records
    for i = 1:nxrecs
        key    = header(2*i-1,:);
        value  = header(2*i,:);
        newhdr = addHeaderRecord(newhdr, key, value);
    end
end

% write header
if asText
    for i = 1:size(newhdr, 1)
        fprintf(f, '%s\n', deblank(newhdr(i,:)));
    end
else
    fprintf(f, '%s', newhdr');
end

% write data
if channelPoints
    if asText
        formatStr = '';
        for i = 1:channels-1
            formatStr = [formatStr, '%f\t'];
        end
        formatStr = [formatStr, '%f\n'];
        fprintf(f, formatStr, x(1:channelPoints, :)');
    else
        % multiplex channel data
        filePoints = channels * channelPoints;
        for i = 1:channels
            data(i:channels:filePoints) = x(:,i);
        end
        fwrite(f, data, 'float32');
    end
end

% close file
fclose(f);


function newHeader = addHeaderRecord(header, key, value)
value  = deblank(value);
record = blanks(128);
record(1:1+length(key)-1) = key;
record(33:33+length(value)-1) = value;
newHeader = [header; record];


function newHeader = removeHeaderRecord(header, key)
newHeader = header;
rows  = size(header, 1);
chars = size(header, 2);
len   = min(length(key), chars);
for i = 1:2:rows-1
    if strcmp(char(header(i,1:len)), key)
        newHeader = header([1:i-1,i+2:rows],:);
        break;
    end
end
