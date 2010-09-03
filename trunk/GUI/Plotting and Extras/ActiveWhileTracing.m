filepath = '\\fender.ce.berkeley.edu\desktops\cmisra\OpenFresco GUI\Ground Motions\Old Database\STM090.AT2';
dt = ReadWriteTHFile('readDT',filepath);
[t ag] = ReadWriteTHFile('readTHF',filepath);
t_data = [];
ag_data = [];

figure
axis([0 40 -1 1]);
for i = 1:500
    t_data = [t_data t(i)];
    ag_data = [ag_data ag(i)];
    clf
    plot(t_data, ag_data)
    hold on
    plot(t(i), ag(i),'ro')
    %Must rescale axis after plotting since plotting the data arrays
    %resizes the window
    axis([0 40 -1 1]);
    drawnow;
end
