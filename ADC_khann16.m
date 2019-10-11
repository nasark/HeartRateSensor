delete(instrfindall);   %remove any other devices
comm = serial('COM3');comm.BaudRate = 57600;comm.Terminator='CR';  %initialize COM port 3, baud rate of 57600, and 'CR' as terminator character

fopen(comm);        %starts serial communication
x=1;
while (1>0)     %infinite loop 
    f(x) = str2double(fgetl(comm))*5/((2^12)-1);    %convert digital value to voltage
    plot(f);                                        %plot graph
    xlabel('Time(ms)');              %set axis titles
    ylabel('Voltage(V)');
    grid
    drawnow;
    x=x+1;                      %increment x in order to allow for continuous communication
end
fclose(comm);               %stops serial communication
delete(comm);
clear comm;
