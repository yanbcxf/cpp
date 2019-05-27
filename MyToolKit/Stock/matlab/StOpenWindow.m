function StOpenWindow( windowNumber )
%UNTITLED4 Summary of this function goes here
%   Detailed explanation goes here
handle = figure(str2num(windowNumber));
set(handle, 'NumberTitle','off');
set(handle, 'MenuBar','None');
set(handle, 'ToolBar','None');
winName = strcat('Stock', windowNumber);
set(handle, 'Name', winName);
number = handle.Number;
end

