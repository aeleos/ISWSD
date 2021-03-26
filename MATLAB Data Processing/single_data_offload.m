clc;
clear workspace;

port = '/dev/ttyUSB1'; % serial port
path = '';    % path to where the data files should be saved
REFRESH = 2; % data points before updating plot


% SETUP

if ~isempty(instrfind)
    fclose(instrfind);
    delete(instrfind);
end

serial_device = serial(port, 'BAUD', 115200);

START_TIME = now;
START_TIME_STRING = datestr(datetime(START_TIME,'ConvertFrom','datenum'));
START_FILE_NAME = [path, START_TIME_STRING, '.csv'];

data_file = fopen(START_FILE_NAME,'wt');
fopen(serial_device);
fprintf(data_file,'Point,Pressure(hPa),Temp(C)\n');

pressure = zeros(REFRESH,1);
temperature = zeros(REFRESH,1);
points = zeros(REFRESH,1);

fig = figure;
tiledlayout(2,1);
set(fig,'KeyPressFcn',@endloop);

p = nexttile;
hold(p,'on');
plot(p,points,pressure);
title('Pressure');
ylabel('hPa');

t = nexttile;
hold(t,'on');
plot(points,temperature);
title('Temperature');
ylabel('C');

global loop;
loop = 1;
loop_count = 0;
pause(5);

% LOOP

fprintf(serial_device,"1");

line_start = zeros(3,1);

while(loop)

    temperature(1) = line_start(1);
    pressure(1) = line_start(2);
    points(1) = line_start(3);
    
    for i=2:REFRESH+1
        pause(.5);
        temperature( i )=fscanf(serial_device,'%f');
        pause(.5);
        pressure( i ) = fscanf(serial_device,'%f');
        points( i ) = i+loop_count-1;
        fprintf(data_file,'%d,%f,%f\n', points(i),pressure(i),temperature(i));
    end
    loop_count = loop_count + REFRESH;
    
    line_start(1) = temperature(REFRESH+1);
    line_start(2) = pressure(REFRESH+1);
    line_start(3) = points(REFRESH+1);
    
    plot(p,points,pressure,'r');
    plot(t,points,temperature,'b');
end


% CLOSING

fprintf(serial_device,"0");

fclose(serial_device);
delete(serial_device);
fclose(data_file);

END_TIME = now;
END_TIME_STRING = datestr(datetime(END_TIME,'ConvertFrom','datenum'));
END_FILE_NAME = [path, START_TIME_STRING, ' - ', END_TIME_STRING, '_S.csv'];


movefile(START_FILE_NAME,END_FILE_NAME);

function endloop(~,event)
    if (event.Key == 'escape')
        global loop;
        loop = 0;
    end
end