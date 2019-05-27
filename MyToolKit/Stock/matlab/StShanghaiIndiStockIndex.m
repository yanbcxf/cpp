function handleNumber = StShanghaiIndiStockIndex( windowNumber, startDate, endDate, stockCode )
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
handle = figure(str2num(windowNumber));
handleNumber = handle.Number;

% 清空窗口
clf;

conn = database('stock', 'root', '','com.mysql.jdbc.Driver','jdbc:mysql://127.0.0.1:3366/stock');
strSql = 'select tradedate, close_index  from daily_hushen_stock where tradedate >=''';
strSql = strcat(strSql, startDate);
strSql = strcat(strSql, ''' and tradedate <= ''');
strSql = strcat(strSql, endDate);
strSql = strcat(strSql, ''' and code = ');

% 将数据读到 matlab 的空间中
% 获取上证指数
strSql_ = strcat(strSql, int2str(1));
curs = exec(conn, strSql_);
curs = fetch(curs);
szTradedate = cell2mat(curs.Data(:,1));
szTradedate = datenum(szTradedate); 
szIndex = cell2mat(curs.Data(:,2));
% 获取个股指数
strSql_ = strcat(strSql, int2str(stockCode));
curs = exec(conn, strSql_);
curs = fetch(curs);
indiTradedate = cell2mat(curs.Data(:,1));
indiTradedate = datenum(indiTradedate); 
indiIndex = cell2mat(curs.Data(:,3));

close(conn);

% 去掉图形周围的边框，让图形显示的更大
grid on;
set(gca,'looseInset',[10 10 10 10]);   
% 绘制双轴图
plotyy(szTradedate, szIndex, indiTradedate, indiIndex, 'plot', 'plot');

% 对 x 轴进行日期格式的显示, keepticks 选项可以增大 label 的密度
datetick('x', 'yyyy/mm/dd','keepticks');  
legend('上证指数', '个股指数');

strTitle = strcat('查询日期范围：' ,startDate, ' 到 ', endDate);
title(strTitle);
end

