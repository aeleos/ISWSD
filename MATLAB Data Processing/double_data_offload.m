clc;
clear workspace;

port1 = '/dev/ttyUSB0'; % serial port
port2 = '/dev/ttyUSB1'; % second serial port
path = '';    % path to where the data files should be saved
REFRESH = 5; % data points before updating plot


% SETUP

if ~isempty(instrfind)
    fclose(instrfind);
    delete(instrfind);
end

serial_device1 = serial(port1, 'BAUD', 115200);
serial_device2 = serial(port2, 'BAUD', 115200);

fopen(serial_device1);
fopen(serial_device2);

START_TIME = now;
START_TIME_STRING = datestr(datetime(START_TIME,'ConvertFrom','datenum'));
START_FILE_NAME = [path, START_TIME_STRING, '.csv'];

data_file = fopen(START_FILE_NAME,'wt');
fprintf(data_file,'Point,Pressure(hPa),Temp(C),Pressure(hPa),Temp(C)\n');

pressure1 = zeros(1,REFRESH);
temperature1 = zeros(1,REFRESH);
pressure2 = zeros(1,REFRESH);
temperature2 = zeros(1,REFRESH);
points = zeros(1,REFRESH);

fig = figure;
tiledlayout(4,1);
set(fig,'KeyPressFcn',@endloop);

p1 = nexttile;
hold(p1,'on');
pp1 = plot(p1,points,pressure1);
title('Pressure');
ylabel('hPa');
xlim([1 inf]);

t1 = nexttile;
hold(t1,'on');
tt1 = plot(t1,points,temperature1);
title('Temperature');
ylabel('C');
xlim([1 inf]);

p2 = nexttile;
hold(p2,'on');
pp2 = plot(p2,points,pressure2);
title('Pressure');
ylabel('hPa');
xlim([1 inf]);

t2 = nexttile;
hold(t2,'on');
tt2 = plot(t2,points,temperature2);
title('Temperature');
ylabel('C');
xlim([1 inf]);

global loop;
loop = 1;
loop_count = 0;
pause(5);

% LOOP

fprintf(serial_device1,"1");
fprintf(serial_device2,"1");

while(loop)
    
    for i=1:REFRESH
        pause(.25);
        temperature1( i ) = fscanf(serial_device1,'%f');
        pause(.25);
        pressure1( i ) = fscanf(serial_device1,'%f');
        pause(.25);
        temperature2( i ) = fscanf(serial_device2,'%f');
        pause(.25);
        pressure2( i ) = fscanf(serial_device2,'%f');     
        points( i ) = i+loop_count;
        fprintf(data_file,'%d,%f,%f,%f,%f\n', points(i),pressure1(i),temperature1(i),pressure2(i),temperature2(i));
    end
    loop_count = loop_count + REFRESH;
    
    xdata = get(pp1,'XData');
    p1data = get(pp1,'YData');
    t1data = get(tt1,'YData');
    p2data = get(pp2,'YData');
    t2data = get(tt2,'YData');
    set(pp1,'XData',[xdata points]);
    set(pp1, 'YData',[p1data pressure1]);
    set(tt1,'XData',[xdata points]);
    set(tt1, 'YData',[t1data temperature1]);
    set(pp2,'XData',[xdata points]);
    set(pp2, 'YData',[p2data pressure2]);
    set(tt2,'XData',[xdata points]);
    set(tt2, 'YData',[t2data temperature2]);
    
end


% CLOSING

fprintf(serial_device1,"0");
fprintf(serial_device2,"0");

fclose(serial_device1);
fclose(serial_device2);
fclose(data_file);

END_TIME = now;
END_TIME_STRING = datestr(datetime(END_TIME,'ConvertFrom','datenum'));
END_FILE_NAME = [path, START_TIME_STRING, ' - ', END_TIME_STRING, '_D.csv'];


movefile(START_FILE_NAME,END_FILE_NAME);

function endloop(~,event)
    if (event.Key == 'escape')
        global loop;
        loop = 0;
    end
end