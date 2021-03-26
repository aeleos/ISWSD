clc;
clear workspace;

data = readmatrix(uigetfile('*_D.csv'),'RANGE',[2 1]);

points = data(:,1);
pressure1 = data(:,2);
temperature1 = data(:,3);
pressure2 = data(:,4);
temperature2 = data(:,5);

fig = figure;
tiledlayout(4,1);

p1 = nexttile;
plot(p1,points,pressure1,'r');
title('Pressure');
ylabel('hPa');

t1 = nexttile;
plot(points,temperature1,'b');
title('Temperature');
ylabel('C');

p2 = nexttile;
plot(p2,points,pressure2,'r');
title('Pressure');
ylabel('hPa');

t2 = nexttile;
plot(points,temperature2,'b');
title('Temperature');
ylabel('C');