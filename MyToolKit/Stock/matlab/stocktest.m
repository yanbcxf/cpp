function  stocktest(wndname, surface)
%UNTITLED3 此处显示有关此函数的摘要
%   此处显示详细说明
figure('NumberTitle','off','MenuBar','None','ToolBar','None','Name',wndname);
switch surface
    case 'sphere'
        sphere
    case 'cylinder'
        cylinder
end
shading interp
axis equal
end

