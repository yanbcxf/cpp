
drop procedure if exists `add_table`;

###########@

CREATE  PROCEDURE `add_table`(IN p_tablename varchar(50), IN p_sql varchar(5000))
begin
	DECLARE sql1 VARCHAR(5000);
	select count(*) into @cnt from information_schema.tables where table_name = p_tablename;
	if @cnt = 0 then
		set @sql1=p_sql;
		prepare stmt FROM @sql1;
		execute stmt;
	end if;
end

###########@


###########@

CREATE  PROCEDURE `CalculateAllStockPe`(`fromDate` date)
BEGIN
	#Routine body goes here...
	DECLARE nCode int;
	DECLARE	Done int DEFAULT 0;
	declare rs CURSOR for select code from stockinfo ORDER BY code;
	DECLARE CONTINUE HANDLER FOR NOT FOUND SET Done = 1;
	
	open rs;
	fetch next from rs into nCode;
label:
	REPEAT
		if Done THEN
			leave label;
		ELSE
			call CalculatePe(nCode, fromDate);
		end if;
		fetch next from rs into nCode;
	until Done end repeat;
	close rs;
END

###########@


###########@

CREATE  PROCEDURE `CalculateAllStockRecentMACD`()
BEGIN
	#Routine body goes here...
	DECLARE nCode int;
	DECLARE	Done int DEFAULT 0;
	Declare lastDate varchar(32);
	DECLARE existTable int;
	declare rs CURSOR for select code from stockinfo ORDER BY code;
	DECLARE CONTINUE HANDLER FOR NOT FOUND SET Done = 1;
	
	open rs;
	fetch next from rs into nCode;
label:
	REPEAT
		if Done THEN
			leave label;
		ELSE
			select count(*) into existTable from information_schema.tables where table_name = 'MACD';
			if existTable = 0 THEN
				call CalculateMACD(nCode, '2012-1-1');
			ELSE
				set lastDate = MaxDateForMACD(nCode);
				if  lastDate = ''  or lastDate is null THEN
					call CalculateMACD(nCode, '2012-1-1');
				else
					call CalculateMACD(nCode, lastDate);
				end if;
			end if;
		end if;
		fetch next from rs into nCode;
	until Done end repeat;
	close rs;
END

#############@@@

#############@@@

CREATE  PROCEDURE `CalculateAllStockRecentPe`()
BEGIN
	#Routine body goes here...
	DECLARE nCode int;
	DECLARE	Done int DEFAULT 0;
	Declare lastDate varchar(32);
	DECLARE existTable int;
	declare rs CURSOR for select code from stockinfo ORDER BY code;
	DECLARE CONTINUE HANDLER FOR NOT FOUND SET Done = 1;
	
	open rs;
	fetch next from rs into nCode;
label:
	REPEAT
		if Done THEN
			leave label;
		ELSE
			select count(*) into existTable from information_schema.tables where table_name = 'PeDaily';
			if existTable = 0 THEN
				call CalculatePe(nCode, '2012-1-1');
			ELSE
				select max(trade_date) into lastDate from pedaily where code = nCode ;
				if  lastDate = ''  or lastDate is null THEN
					call CalculatePe(nCode, '2012-1-1');
				else
					call CalculatePe(nCode, lastDate);
				end if;
			end if;
		end if;
		fetch next from rs into nCode;
	until Done end repeat;
	close rs;
END

#############@@@


#############@@@

CREATE PROCEDURE `CalculateMACD`(`nCode` int,`fromDate` date)
BEGIN
	#Routine body goes here...
	DECLARE odt_cur date;
	DECLARE price double;
	declare total_value double;
	DECLARE ema12 double;
	declare ema26 double;
	declare dif double;
	declare dea double;
	declare a_total double;
	declare first_date date;
	declare isFirst int;
	DECLARE isExist int;

	DECLARE	Done int DEFAULT 0;
	
	declare rs CURSOR for select TradeDate, Close  from souhutradingdaily where code = nCode and TradeDate >= fromDate ORDER BY TradeDate;
	DECLARE CONTINUE HANDLER FOR NOT FOUND SET Done = 1;

	set isFirst = 1;

	create table if not exists MACD ( code int , trade_date date,
		price DECIMAL(10,2),  a_total DECIMAL(15,2), total_value DECIMAL(15,2),  
		ema12 DECIMAL(15,2) , ema26 DECIMAL(15,2), dif DECIMAL(15,2), 
		dea  DECIMAL(15,2) ,  PRIMARY KEY ( code, trade_date ));

	open rs;
	fetch next from rs into odt_cur, price;
label:
	REPEAT
		if Done THEN
			leave label;
		ELSE
			set a_total = CurrentATotalShares(nCode, odt_cur);
			#股本结构还未下载
			if a_total <=0 THEN
				leave label;
			end if;

			set total_value = price * a_total * 10000;
			/*计算上一个交易日的 MACD 值 */
			if isFirst =1 THEN
				select max(trade_date) into first_date from MACD where  code = nCode and  trade_date < odt_cur;
				if first_date is not null THEN
					select  macd.ema12, macd.ema26, macd.dif, macd.dea into ema12, ema26, dif, dea  
					from MACD  where  code = nCode and  trade_date = first_date; 
				else 
					set ema12 = total_value;
					set ema26 = total_value;
					set dif = 0;
					set dea = 0;
				end if;
				set isFirst = 0;
			end if;
			
			set ema12 = ema12 * 11 /13 + total_value * 2 /13;
			set ema26 = ema26 * 25/ 27 + total_value * 2 /27;
			set dif = ema12 - ema26;
			set dea = dea * 8 / 10 + dif * 2 /10;

			select count(*) into isExist from MACD where code = nCode and trade_date = odt_cur;
			if isExist = 0 THEN
				insert into MACD select nCode, odt_cur, price, a_total, total_value, ema12, ema26, dif, dea from dual ; 
			ELSE
				update MACD set code = nCode, trade_date = odt_cur, price = price, 
				a_total = a_total, total_value = total_value, ema12 = ema12, ema26 = ema26, dif = dif, dea = dea
				where code = nCode and trade_date = odt_cur;
			end if;
						
		end if;
		fetch next from rs into odt_cur, price;
	until Done end repeat;
	close rs;
	
END

#############@@@



#############@@@


#############@@@

###################@

create table if not exists hexun_research_paper (Code int , report_date date, title varchar(256),
author varchar(32), source varchar(32), grade varchar(32), abstract varchar(2000), 
url varchar(256), PRIMARY KEY ( Code, report_date, title));

###################@

drop FUNCTION if exists `EstimateNetProfits`;

###################@

CREATE  FUNCTION `EstimateNetProfits`(`nCode` int,`curDate` date) RETURNS double
    READS SQL DATA
BEGIN
	#Routine body goes here...
	declare baseDate date;
	declare isLast int;
	declare net_profit double;
	declare curMonth int;
	declare curYear int;
	
	call NetProfitBelongingToParentCompany(nCode, curDate, baseDate, isLast, net_profit);

	#估算本年的利润
	set curMonth = MONTH(baseDate);
	if curMonth =3 THEN
			set net_profit = 4 * net_profit;
	end if;
	if curMonth =6 THEN
			set net_profit = 2 * net_profit;
	end if;
	if curMonth =9 THEN
			set net_profit = 4 * net_profit / 3;
	end if;
	if curMonth =12 THEN
			set net_profit = net_profit;
	end if;
	
	RETURN net_profit;
END

###################@

drop FUNCTION if exists `RecentFinanceDate`;

###################@

CREATE  FUNCTION `RecentFinanceDate`(`nCode` int,`curDate` date) RETURNS date
    READS SQL DATA
BEGIN
	#Routine body goes here...
	declare baseDate date;
	declare isLast int;
	declare net_profit double;
	
	call NetProfitBelongingToParentCompany(nCode, curDate, baseDate, isLast, net_profit);

	RETURN baseDate;
END

###################@


###################@