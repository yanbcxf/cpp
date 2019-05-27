function handleNumber = StShanghaiStockIndex( windowNumber, startDate, endDate, graphOrTable, stockCodes )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
%   StShanghaiStockIndex('1', '2017-1-1', '2017-10-1', '1',[600116, 600115])
handle = figure(str2num(windowNumber));
handleNumber = handle.Number;

% 清空窗口
clf;

% 将数据读入 matlab 空间中
conn = database('stock', 'root', '','com.mysql.jdbc.Driver','jdbc:mysql://127.0.0.1:3366/stock');

strSql = 'select abbreviation, tradedate, close_index from daily_hushen_stock where tradedate >=''';
strSql = strcat(strSql, startDate, ''' and tradedate <= ''', endDate, ''' and code = ');

hwait = waitbar(0,'读入数据中，请等待......','WindowStyle','modal');
totalNum = length(stockCodes);
j = 1;
for i = 1: totalNum
    strSql_ = strcat(strSql, int2str(stockCodes(1, i)));
    
    curs = exec(conn, strSql_);
    curs = fetch(curs);
    
    if ~strcmp(curs.Data,'No Data')
        areaIndex(j).code = stockCodes(1, i);
        abbreviation = cell2mat(curs.Data(1, 1));
        if length(abbreviation) > 0
            areaIndex(j).name = abbreviation(1,:);
        else
            areaIndex(j).name = '(无)';
        end;
        areaIndex(j).tradedate = cell2mat(curs.Data(:, 2));
        areaIndex(j).close_index = cell2mat(curs.Data(:, 3));
        j = j + 1;
    end;
    
    str=['读入数据中，请等待......',num2str(i/totalNum * 100),'%， 股票代码 = ',int2str(stockCodes(1, i))];
    waitbar(i/totalNum,hwait,str);
    close(curs);
end;
close(hwait);

% 对所有的股票进行合计
strSql = 'select tradedate, sum(close_index) as close_index from daily_hushen_stock where tradedate >''';
strSql = strcat(strSql, startDate, ''' and tradedate <= ''', endDate, ''' and code in  ( ');
for i = 1: length(stockCodes)
    strSql = strcat(strSql, int2str(stockCodes(1, i)));
    if i< length(stockCodes)
        strSql = strcat(strSql, ',');
    else
        strSql = strcat(strSql, ')');
    end;
end;
strSql = strcat(strSql, ' GROUP BY tradedate');
curs = exec(conn, strSql);
curs = fetch(curs);
if ~strcmp(curs.Data,'No Data')
%     j = length(areaIndex) + 1;
%     areaIndex(j).code = 0;
%     areaIndex(j).name = '合计';
%     areaIndex(j).tradedate = cell2mat(curs.Data(:, 1));
%     areaIndex(j).close_index = cell2mat(curs.Data(:, 2));
end;
close(curs);
close(conn);

strTitle = strcat('查询日期范围：' ,startDate, ' 到 ', endDate);
title(strTitle);

if strcmp(graphOrTable, 'Graph')
    % 进行曲线的绘制
    hwait = waitbar(0,'绘图中，请等待......','WindowStyle','modal');
    totalNum = length(areaIndex);
    
    hold on;
    grid on;
    set(gca,'looseInset',[10 10 10 10]);   % 去掉图形周围的边框，让图形显示的更大
    
    for i = 1: totalNum
        timeB = datenum(areaIndex(i).tradedate); % 将字符串型的日期 转换成 数值型
        ph(i) = plot(timeB, areaIndex(i).close_index, 'LineWidth',1, 'ButtonDownFcn', @lincbf);
        label(i) = {areaIndex(i).name};
        %  计算期末相对于期初的变化率
        len = length(areaIndex(i).close_index);
        if len > 0
            change_rate = areaIndex(i).close_index(len) -  areaIndex(i).close_index(1);
            change_rate = change_rate/ areaIndex(i).close_index(1) * 100;
            label(i) = strcat(label(i), '：', int2str(change_rate),  '%');
            
            set(ph(i), 'Tag', label{i});
        end;
        
        str=['绘图中，请等待......',num2str(i/totalNum * 100),'%'];
        waitbar(i/totalNum,hwait,str);
    end;
    close(hwait);
    legend(ph, label, 'Location','EastOutside');
    datetick('x', 'yyyy/mm/dd','keepticks');
else
    %   以表格形式展示
    columnname =   {'代码', '名称', '期初日期', '期末日期', '期初复权价', '期末复权价', '涨跌率(%)' };
    columnformat = {'numeric', 'char', 'char', 'char', 'bank', 'bank', 'bank'};
    ColumnWidth = {'auto', 200, 'auto', 'auto', 'auto', 'auto', 'auto'};
    for i = 1: length(areaIndex)
        [len, cols] = size(areaIndex(i).tradedate);
        dat(i, 1) = { areaIndex(i).code };
        dat(i, 2) = { areaIndex(i).name };
        dat(i, 3) = { areaIndex(i).tradedate(1, :) };
        dat(i, 4) = { areaIndex(i).tradedate(len, :) };
        dat(i, 5) = { areaIndex(i).close_index(1) };
        dat(i, 6) = { areaIndex(i).close_index(len) };
        dat(i, 7) = { (areaIndex(i).close_index(len) - areaIndex(i).close_index(1))/ areaIndex(i).close_index(1) * 100  };
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
    
end;

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
            case '期末点位-递减'
                dat = sortrows(dat, -6);
                set(t,'data',dat);
            case '涨跌率-递减'
                dat = sortrows(dat, -7);
                set(t,'data',dat);
                
        end
    end
end

