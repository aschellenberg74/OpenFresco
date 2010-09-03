filepath = '\\fender.ce.berkeley.edu\desktops\cmisra\OpenFresco GUI\Ground Motions\Old Database\STM090.AT2';
dt = ReadWriteTHFile('readDT',filepath);
[t ag] = ReadWriteTHFile('readTHF',filepath);

tic
figure
axis([0 40 -1 1]);
for i = 1:600
    clf
    plot(t, ag)
    hold on
    plot(t(i), ag(i),'ro')
    drawnow;
    pause(dt/4)
end
toc

%May be more efficient to use a callback whenever the array is updated with
%data from the test