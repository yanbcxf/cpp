function handleNumber = StStockChangeInPlate( windowNumber, startDate, endDate, plateCode )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
handle = figure(str2num(windowNumber));
handleNumber = handle.Number;

% 本函数为了计算某个板块指数的某个暴跌交易日，哪些股票的点数下跌最大
% 清空窗口
clf;

% 将数据读入 matlab 空间中
% 首先读入属于该板块的所有 股票
conn = database('stock', 'root', '','com.mysql.jdbc.Driver','jdbc:mysql://127.0.0.1:3366/stock');

strSql = 'select code, abbreviation from stockinfo where ';
if plateCode == 399101 
    strSql = strcat(strSql, ' code >=2000 and code <=2999 ');
elseif plateCode == 399102
    strSql = strcat(strSql, ' code >=300000 and code <=399999 ');
elseif plateCode == 000001
    strSql = strcat(strSql, ' code >=600000  ');
end;

curs = exec(conn, strSql);
curs = fetch(curs);
if ~strcmp(curs.Data,'No Data')
    % stockCodes 和 stockNames 这两个是 Cell 型数组
    stockCodes = curs.Data(:, 1);
    stockNames = curs.Data(:, 2);
end
close(curs);

% 再逐个股票读入期末及期初的 流通市值, 及计算指数的 除数
strSql = 'select  tradedate, circulating_value ,';
strSql = strcat(strSql,  '(select b.division from daily_hushen_plate b where a.tradedate = b.tradedate and b.code = ');
strSql = strcat(strSql, num2str(plateCode) );
strSql = strcat(strSql, ' ) as division ');
strSql = strcat(strSql, ' from daily_hushen_stock a where tradedate >=''');
strSql = strcat(strSql, startDate, ''' and tradedate <= ''', endDate, ''' and  code = ');
        
hwait = waitbar(0,'请等待......','WindowStyle','modal');
totalNum = length(stockCodes);
j = 1;
for i = 1: totalNum
    strSql_ = strcat(strSql, int2str(stockCodes{i, 1}));
    
    curs = exec(conn, strSql_);
    curs = fetch(curs);
    
    if ~strcmp(curs.Data,'No Data')
        areaIndex(j).code = stockCodes{i,1};
        abbreviation = stockNames{i, 1};
        if length(abbreviation) > 0
            areaIndex(j).name = abbreviation(1,:);
        else
            areaIndex(j).name = '(无)';
        end;
        areaIndex(j).tradedate = cell2mat(curs.Data(:, 1));
        areaIndex(j).circulating_value = cell2mat(curs.Data(:, 2));
        areaIndex(j).division = cell2mat(curs.Data(:, 3));
        j = j + 1;
    end;
  
    str=['正在运行中',num2str(i/totalNum * 100),'%'];
    waitbar(i/totalNum, hwait, str);
    close(curs);
end;
close(hwait);
close(conn);

strTitle = strcat('查询日期范围：' ,startDate, ' 到 ', endDate);
title(strTitle);

 %   以表格形式展示
    columnname =   {'代码', '名称', '期初日期', '期末日期', '期初点位', '期末点位', '涨跌率(%)', '涨跌幅' };
    columnformat = {'numeric', 'char', 'char', 'char', 'bank', 'bank', 'bank', 'bank'};
    ColumnWidth = {'auto', 200, 'auto', 'auto', 'auto', 'auto', 'auto', 'auto'};
    for i = 1: length(areaIndex)
        [len, cols] = size(areaIndex(i).tradedate);
        % dat 中的元素是一个 cell
        dat(i, 1) = { areaIndex(i).code };
        dat(i, 2) = { areaIndex(i).name };
        dat(i, 3) = { areaIndex(i).tradedate(1, :) };
        dat(i, 4) = { areaIndex(i).tradedate(len, :) };
        dat(i, 5) = { areaIndex(i).circulating_value(1)/areaIndex(i).division(1) };
        dat(i, 6) = { areaIndex(i).circulating_value(len)/areaIndex(i).division(len) };
        dat(i, 7) = { (dat{i, 6} - dat{i, 5})/ dat{i, 5} * 100  };
        dat(i, 8) = { dat{i, 6} - dat{i, 5}  };
    end;
    
    %   当 Units = normalized 时， The lower-left corner of the container maps to (0,0) and the upper-right corner maps to (1,1).
    t = uitable(handle, 'Units','normalized',...
        'Data', dat,...
        'Position', [0.05 0.05 0.9 0.88],...
        'FontSize',10,...
        'ColumnName', columnname,...
        'ColumnFormat', columnformat,...
        'ColumnWidth', ColumnWidth,...
        'RearrangeableColumns', 'on');
    
    c = uicontextmenu;
    t.UIContextMenu = c;
    
    % Create child menu items for the uicontextmenu
    m1 = uimenu(c,'Label','期末点位-递增','Callback',@setlinestyle);
    m2 = uimenu(c,'Label','期末点位-递减','Callback',@setlinestyle);
    m3 = uimenu(c,'Label','涨跌率-递增','Callback',@setlinestyle);
    m4 = uimenu(c,'Label','涨跌率-递减','Callback',@setlinestyle);
    m5 = uimenu(c,'Label','涨跌幅-递增','Callback',@setlinestyle);
    m6 = uimenu(c,'Label','涨跌幅-递减','Callback',@setlinestyle);

    function lincbf(src,eventdata)
        % 用右键点击要删除的曲线
        strLegend = get(src, 'Tag');
        strTitle_ = strcat(strTitle, '：' ,strLegend);
        title(strTitle_);
        if strcmp(get(gcf,'SelectionType'),'alt')
            delete(src) % 删除曲线
        end
    end

    function setlinestyle(source,callbackdata)
        switch source.Label
            case '期末点位-递增'
                dat = sortrows(dat, 6);
                set(t,'data',dat);
            case '涨跌率-递增'
                dat = sortrows(dat, 7);
                set(t,'data',dat);
            case '涨跌幅-递增'
                dat = sortrows(dat, 8);
                set(t,'data',dat);
            case '期末点位-递减'
                dat = sortrows(dat, -6);
                set(t,'data',dat);
            case '涨跌率-递减'
                dat = sortrows(dat, -7);
                set(t,'data',dat);
            case '涨跌幅-递减'
                dat = sortrows(dat, -8);
                set(t,'data',dat);
        end
    end
end

