function uitableDemo

f = figure('Position',[100 100 400 150], 'MenuBar','None','ToolBar','None');
dat =  {6.125, 456.3457, 'true',  'Fixed';...
    6.75,  658.2 , 'false', 'Adjustable';...
    7,     510.2342,    'false', 'Fixed';};
columnname =   {'Rate', 'Amount', 'Available', 'Fixed/Adj'};
columnformat = {'numeric', 'bank', 'char', {'Fixed' 'Adjustable'}};
columneditable =  [false false true true];

t = uitable('Units','normalized','Position',...
    [0.1 0.1 0.9 0.9], 'Data', dat,...
    'ColumnName', columnname,...
    'ColumnFormat', columnformat,...
    'RearrangeableColumns', 'on',...
    'ColumnEditable', columneditable);

c = uicontextmenu;
t.UIContextMenu = c;

% Create child menu items for the uicontextmenu
m1 = uimenu(c,'Label','dashed','Callback',@setlinestyle);
m2 = uimenu(c,'Label','dotted','Callback',@setlinestyle);

    function setlinestyle(source,callbackdata)
        switch source.Label
            case 'dashed'
                dat = sortrows(dat, 1);
                set(t,'data',dat); 
            case 'dotted'
                dat = sortrows(dat, 2);
                set(t,'data',dat);
        end
    end

end

