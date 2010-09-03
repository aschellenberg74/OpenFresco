filepath = '\\fender.ce.berkeley.edu\desktops\cmisra\OpenFresco GUI\Ground Motions\Old Database\STM090.AT2';
dt = ReadWriteTHFile('readDT',filepath);
[t ag] = ReadWriteTHFile('readTHF',filepath);
data = [];
dataT = [];
tic
figure
axis([0 40 -1 1]);
xlabel('Time (s)')
ylabel('ag')
hold on
for i = 1:250
    data = [data ag(i)];
    dataT = [dataT t(i)];
    plot(dataT, data)
    drawnow;
end
toc

%May be more efficient to use a callback whenever the array is updated with
%data from the test