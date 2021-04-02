clc;
clear workspace;

data = readmatrix(uigetfile('*_D.csv'),'RANGE',[2 1]);

points = data(:,1);
pressure1 = data(:,2);
pressure1 = pressure1 - pressure1(1);
temperature1 = data(:,3);
pressure2 = data(:,4);
pressure2 = pressure2 - pressure2(1);
temperature2 = data(:,5);
height1 = 44330 * (1-(pressure1/1013.25).^.1903);
height1 = height1 - height1(1);
height2 = 44330 * (1-(pressure2/1013.25).^.1903);
height2 = height2 -height2(2);

fig = figure;
tiledlayout(4,1);

p = nexttile;
plot(p,points,pressure1,'r');
plot(p,points,pressure2,'b');
title('Pressure');
ylabel('hPa');

h = nexttile;
plot(h,points,height1,'r');
plot(h,points,height2,'b');
title('Altitude');
ylabel('m');

t = nexttile;
plot(points,temperature1,'r');
plot(points,temperature2,'b');
title('Temperature');
ylabel('C');
