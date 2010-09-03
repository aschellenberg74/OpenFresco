filepath = '\\fender.ce.berkeley.edu\desktops\cmisra\OpenFresco GUI\Ground Motions\Old Database\STM090.AT2';
dt = ReadWriteTHFile('readDT',filepath);
[t ag] = ReadWriteTHFile('readTHF',filepath);
data = [];
dataT = [];
tic
f = figure;
set(f,'NextPlot','replacechildren');
xlabel('Time (s)')
ylabel('ag')
for i = 1:500
    data = [data ag(i)];
    dataT = [dataT t(i)];
    plot(dataT, data)
    pause(dt/2) %Insert a pause to simulate real time data stream
end
toc

%May be more efficient to use a callback whenever the array is updated with
%data from the test