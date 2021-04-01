clc;
clear workspace;

PORT1 = '/dev/ttyUSB0'; % serial port
PORT2 = '/dev/ttyUSB1'; % second serial port
PATH = '';    % path to where the data files should be saved
STEP = 30;    % seconds between measurements
REFRESH = 5; % data points before updating plot


% SETUP

if ~isempty(instrfind)
    fclose(instrfind);
    delete(instrfind);
end

serial_device1 = serial(PORT1, 'BAUD', 115200);
serial_device2 = serial(PORT2, 'BAUD', 115200);

fopen(serial_device1);
fopen(serial_device2);

start_time = now;
start_time_string = datestr(datetime(start_time,'ConvertFrom','datenum'));
start_file_name = strrep(strrep([PATH, start_time_string, '.csv'],' ','_'),':','-');

data_file = fopen(start_file_name,'wt');
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

fprintf(serial_device1,'%d',STEP);
fprintf(serial_device2,'%d',STEP);

while(loop)
    
    for i=1:REFRESH
        pause(STEP/4);
        temperature1( i ) = fscanf(serial_device1,'%f');
        pause(STEP/4);
        pressure1( i ) = fscanf(serial_device1,'%f');
        pause(STEP/4);
        temperature2( i ) = fscanf(serial_device2,'%f');
        pause(STEP/4);
        pressure2( i ) = fscanf(serial_device2,'%f');     
        points( i ) = STEP*(i+loop_count-1);
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

fprintf(serial_device1,0);
fprintf(serial_device2,0);

fclose(serial_device1);
fclose(serial_device2);
fclose(data_file);

end_time = now;
end_time_string = datestr(datetime(end_time,'ConvertFrom','datenum'));
end_file_name = strrep(strrep([PATH, start_time_string, '_', end_time_string, '_D.csv'],' ','_'),':','-');

movefile(start_file_name,end_file_name);
disp('Session closed.');

function endloop(~,event)
    if (length(event.Key) == 6)
    if (event.Key == 'escape')
        global loop;
        loop = 0;
        disp('Ending logging session...');
    end
    end
end