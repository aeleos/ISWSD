clear all;
clc;
clear workspace;

data = readmatrix(uigetfile('*_S.csv'),'RANGE',[2 1]);

points = data(2:end,1);
pressure = data(2:end,2);
temperature = data(2:end,3);

fig = figure;
tiledlayout(2,1);

p = nexttile;
plot(p,points,pressure,'r');
title('Pressure');
ylabel('hPa');

t = nexttile;
plot(points,temperature,'b');
title('Temperature');
ylabel('C');