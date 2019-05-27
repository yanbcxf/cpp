
conn = database('stock', 'root', '','com.mysql.jdbc.Driver','jdbc:mysql://127.0.0.1:3366/stock')
curs = exec(conn,'select * from shanghai_index');
curs = fetch(curs); % 将数据读到 matlab 的空间中
close(conn);
timeAxis = curs.Data(:,1);
timeA = cell2mat(timeAxis);
szindex = cell2mat(curs.Data(:,2));
self_index = cell2mat(curs.Data(:,6));
timeB = datenum(timeA); % 将字符串型的日期 转换成 数值型
plot(timeB, szindex, 'LineWidth',1, 'Color', 'r');
hold on;
plot(timeB, self_index, 'LineWidth',1, 'Color', 'g');
% 对 x 轴进行日期格式的显示, keepticks 选项可以增大 label 的密度
datetick('x', 'yyyy/mm/dd','keepticks');   
