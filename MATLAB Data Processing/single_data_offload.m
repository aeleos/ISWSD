clc;
clear workspace;

port = '/dev/ttyUSB0'; % serial port
path = '';    % path to where the data files should be saved
REFRESH = 5; % data points before updating plot


% SETUP

if ~isempty(instrfind)
    fclose(instrfind);
    delete(instrfind);
end

serial_device = serial(port, 'BAUD', 115200);

START_TIME = now;
START_TIME_STRING = datestr(datetime(START_TIME,'ConvertFrom','datenum'));
START_FILE_NAME = strrep(strrep([path, START_TIME_STRING, '.csv'],' ','_'),':','-');

data_file = fopen(START_FILE_NAME,'wt');
fopen(serial_device);
fprintf(data_file,'Point,Pressure(hPa),Temp(C)\n');

pressure = zeros(1,REFRESH);
temperature = zeros(1,REFRESH);
points = zeros(1,REFRESH);

fig = figure;
tiledlayout(2,1);
set(fig,'KeyPressFcn',@endloop);

p = nexttile;
hold(p,'on');
pp=plot(p,points,pressure,'r');
title('Pressure');
ylabel('hPa');
xlim([1 inf]);

t = nexttile;
hold(t,'on');
tt=plot(t,points,temperature,'b');
title('Temperature');
ylabel('C');
xlim([1 inf]);

global loop;
loop = 1;
loop_count = 0;
pause(5);

% LOOP

fprintf(serial_device,"1");

while(loop)
    
    for i=1:REFRESH
        pause(.5);
        temperature( i )=fscanf(serial_device,'%f');
        pause(.5);
        pressure( i ) = fscanf(serial_device,'%f');
        points( i ) = i+loop_count;
        fprintf(data_file,'%d,%f,%f\n', points(i),pressure(i),temperature(i));
    end
    loop_count = loop_count + REFRESH;
    
    xdata = get(pp,'XData');
    pdata = get(pp,'YData');
    tdata = get(tt,'YData');
    set(pp,'XData',[xdata points]);
    set(pp, 'YData',[pdata pressure]);
    set(tt,'XData',[xdata points]);
    set(tt, 'YData',[tdata temperature]);
end


% CLOSING

fprintf(serial_device,"0");

fclose(serial_device);
delete(serial_device);
fclose(data_file);

END_TIME = now;
END_TIME_STRING = datestr(datetime(END_TIME,'ConvertFrom','datenum'));
END_FILE_NAME = strrep(strrep([path, START_TIME_STRING, '_', END_TIME_STRING, '_S.csv'],' ','_'),':','-');
clear workspace;

movefile(START_FILE_NAME,END_FILE_NAME);

function endloop(~,event)
    if (event.Key == 'escape')
        global loop;
        loop = 0;
    end
end