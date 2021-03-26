clc;
clear workspace;

data = readmatrix(uigetfile('*_S.csv'),'RANGE',[2 1]);

points = data(:,1);
pressure = data(:,2);
temperature = data(:,3);
height = 44330 * (1-(pressure/1013.25).^.1903);

fig = figure;
tiledlayout(3,1);

p = nexttile;
plot(p,points,pressure,'r');
title('Pressure');
ylabel('hPa');

h = nexttile;
plot(points,height,'g');
title('Altitude');
ylabel('m');

t = nexttile;
plot(points,temperature,'b');
title('Temperature');
ylabel('C');
