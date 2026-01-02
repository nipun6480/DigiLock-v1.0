clc;
clear;

port = "COM4";  % Change as needed
baudRate = 9600;

arduino = serialport(port, baudRate);
disp("Connected to Arduino");

while true
    if arduino.NumBytesAvailable > 0
        ch = readline(arduino);
        fprintf("Input Key: %s\n", ch);
    end
end
