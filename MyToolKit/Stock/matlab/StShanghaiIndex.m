function handleNumber = StShanghaiIndex( windowNumber, startDate, endDate )
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
handle = figure(str2num(windowNumber));
handleNumber = handle.Number;

% 清空窗口
clf;

 % 将数据读到 matlab 的空间中
conn = database('stock', 'root', '','com.mysql.jdbc.Driver','jdbc:mysql://127.0.0.1:3366/stock');
strSql = 'select * from daily_hushen_plate where tradedate >''';
strSql = strcat(strSql, startDate, ''' and tradedate <= ''',endDate, ''' and code = 1  ');

curs = exec(conn, strSql);
curs = fetch(curs);
timeA = cell2mat(curs.Data(:,4));
timeA = datenum(timeA);
point = cell2mat(curs.Data(:,8));

strSql = 'select tradedate, close from sohuindexdaily where tradedate >''';
strSql = strcat(strSql, startDate, ''' and tradedate <= ''',endDate, ''' and code = 1 ');
curs = exec(conn, strSql);
curs = fetch(curs);
timeB = cell2mat(curs.Data(:,1));
timeB = datenum(timeB);         % 将字符串型的日期 转换成 数值型
szindex = cell2mat(curs.Data(:,2));
close(conn);

plot(timeA, point, 'LineWidth',1, 'Color', 'r');
hold on;
plot(timeB, szindex, 'LineWidth',1, 'Color', 'g');
% 对 x 轴进行日期格式的显示, keepticks 选项可以增大 label 的密度
datetick('x', 'yyyy/mm/dd','keepticks');  
legend('上证指数', '模拟上证指数');
grid on;

strTitle = strcat('查询日期范围：' ,startDate, ' 到 ', endDate);
title(strTitle);
end

