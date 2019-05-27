/*
Navicat MySQL Data Transfer

Source Server         : localhost
Source Server Version : 50160
Source Host           : 127.0.0.1:3366
Source Database       : stock

Target Server Type    : MYSQL
Target Server Version : 50160
File Encoding         : 65001

Date: 2017-04-09 20:19:49
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for 10jqka_bonus
-- ----------------------------
DROP TABLE IF EXISTS `10jqka_bonus`;
CREATE TABLE `10jqka_bonus` (
  `Code` int(11) NOT NULL DEFAULT '0',
  `bonus_year` date NOT NULL DEFAULT '0000-00-00',
  `bonus_share` decimal(10,2) DEFAULT NULL,
  `reserve_to_common_share` decimal(10,2) DEFAULT NULL,
  `bonus_cash` decimal(10,2) DEFAULT NULL,
  `plan_announcement_date` date DEFAULT NULL,
  `right_record_date` date DEFAULT NULL,
  `exit_right_date` date DEFAULT NULL,
  `event_state` varchar(64) DEFAULT NULL,
  PRIMARY KEY (`Code`,`bonus_year`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for balance
-- ----------------------------
DROP TABLE IF EXISTS `balance`;
CREATE TABLE `balance` (
  `code` int(11) NOT NULL DEFAULT '0',
  `report_date` date NOT NULL DEFAULT '0000-00-00',
  `colume_1` decimal(18,2) DEFAULT NULL,
  `colume_2` decimal(18,2) DEFAULT NULL,
  `colume_3` decimal(18,2) DEFAULT NULL,
  `colume_4` decimal(18,2) DEFAULT NULL,
  `colume_5` decimal(18,2) DEFAULT NULL,
  `colume_6` decimal(18,2) DEFAULT NULL,
  `colume_7` decimal(18,2) DEFAULT NULL,
  `colume_8` decimal(18,2) DEFAULT NULL,
  `colume_9` decimal(18,2) DEFAULT NULL,
  `colume_10` decimal(18,2) DEFAULT NULL,
  `colume_11` decimal(18,2) DEFAULT NULL,
  `colume_12` decimal(18,2) DEFAULT NULL,
  `colume_13` decimal(18,2) DEFAULT NULL,
  `colume_14` decimal(18,2) DEFAULT NULL,
  `colume_15` decimal(18,2) DEFAULT NULL,
  `colume_16` decimal(18,2) DEFAULT NULL,
  `colume_17` decimal(18,2) DEFAULT NULL,
  `colume_18` decimal(18,2) DEFAULT NULL,
  `colume_19` decimal(18,2) DEFAULT NULL,
  `colume_20` decimal(18,2) DEFAULT NULL,
  `colume_21` decimal(18,2) DEFAULT NULL,
  `colume_22` decimal(18,2) DEFAULT NULL,
  `colume_23` decimal(18,2) DEFAULT NULL,
  `colume_24` decimal(18,2) DEFAULT NULL,
  `colume_25` decimal(18,2) DEFAULT NULL,
  `colume_26` decimal(18,2) DEFAULT NULL,
  `colume_27` decimal(18,2) DEFAULT NULL,
  `colume_28` decimal(18,2) DEFAULT NULL,
  `colume_29` decimal(18,2) DEFAULT NULL,
  `colume_30` decimal(18,2) DEFAULT NULL,
  `colume_31` decimal(18,2) DEFAULT NULL,
  `colume_32` decimal(18,2) DEFAULT NULL,
  `colume_33` decimal(18,2) DEFAULT NULL,
  `colume_34` decimal(18,2) DEFAULT NULL,
  `colume_35` decimal(18,2) DEFAULT NULL,
  `colume_36` decimal(18,2) DEFAULT NULL,
  `colume_37` decimal(18,2) DEFAULT NULL,
  `colume_38` decimal(18,2) DEFAULT NULL,
  `colume_39` decimal(18,2) DEFAULT NULL,
  `colume_40` decimal(18,2) DEFAULT NULL,
  `colume_41` decimal(18,2) DEFAULT NULL,
  `colume_42` decimal(18,2) DEFAULT NULL,
  `colume_43` decimal(18,2) DEFAULT NULL,
  `colume_44` decimal(18,2) DEFAULT NULL,
  `colume_45` decimal(18,2) DEFAULT NULL,
  `colume_46` decimal(18,2) DEFAULT NULL,
  `colume_47` decimal(18,2) DEFAULT NULL,
  `colume_48` decimal(18,2) DEFAULT NULL,
  `colume_49` decimal(18,2) DEFAULT NULL,
  `colume_50` decimal(18,2) DEFAULT NULL,
  `colume_51` decimal(18,2) DEFAULT NULL,
  `colume_52` decimal(18,2) DEFAULT NULL,
  `colume_53` decimal(18,2) DEFAULT NULL,
  `colume_54` decimal(18,2) DEFAULT NULL,
  `colume_55` decimal(18,2) DEFAULT NULL,
  `colume_56` decimal(18,2) DEFAULT NULL,
  `colume_57` decimal(18,2) DEFAULT NULL,
  `colume_58` decimal(18,2) DEFAULT NULL,
  `colume_59` decimal(18,2) DEFAULT NULL,
  `colume_60` decimal(18,2) DEFAULT NULL,
  `colume_61` decimal(18,2) DEFAULT NULL,
  `colume_62` decimal(18,2) DEFAULT NULL,
  `colume_63` decimal(18,2) DEFAULT NULL,
  `colume_64` decimal(18,2) DEFAULT NULL,
  `colume_65` decimal(18,2) DEFAULT NULL,
  `colume_66` decimal(18,2) DEFAULT NULL,
  `colume_67` decimal(18,2) DEFAULT NULL,
  `colume_68` decimal(18,2) DEFAULT NULL,
  `colume_69` decimal(18,2) DEFAULT NULL,
  `colume_70` decimal(18,2) DEFAULT NULL,
  `colume_71` decimal(18,2) DEFAULT NULL,
  `colume_72` decimal(18,2) DEFAULT NULL,
  `colume_73` decimal(18,2) DEFAULT NULL,
  `colume_74` decimal(18,2) DEFAULT NULL,
  `colume_75` decimal(18,2) DEFAULT NULL,
  `colume_76` decimal(18,2) DEFAULT NULL,
  `colume_77` decimal(18,2) DEFAULT NULL,
  `colume_78` decimal(18,2) DEFAULT NULL,
  `colume_79` decimal(18,2) DEFAULT NULL,
  `colume_80` decimal(18,2) DEFAULT NULL,
  `colume_81` decimal(18,2) DEFAULT NULL,
  `colume_82` decimal(18,2) DEFAULT NULL,
  `colume_83` decimal(18,2) DEFAULT NULL,
  `colume_84` decimal(18,2) DEFAULT NULL,
  `colume_85` decimal(18,2) DEFAULT NULL,
  `colume_86` decimal(18,2) DEFAULT NULL,
  `colume_87` decimal(18,2) DEFAULT NULL,
  `colume_88` decimal(18,2) DEFAULT NULL,
  `colume_89` decimal(18,2) DEFAULT NULL,
  `colume_90` decimal(18,2) DEFAULT NULL,
  `colume_91` decimal(18,2) DEFAULT NULL,
  `colume_92` decimal(18,2) DEFAULT NULL,
  `colume_93` decimal(18,2) DEFAULT NULL,
  `colume_94` decimal(18,2) DEFAULT NULL,
  `colume_95` decimal(18,2) DEFAULT NULL,
  `colume_96` decimal(18,2) DEFAULT NULL,
  `colume_97` decimal(18,2) DEFAULT NULL,
  `colume_98` decimal(18,2) DEFAULT NULL,
  `colume_99` decimal(18,2) DEFAULT NULL,
  `colume_100` decimal(18,2) DEFAULT NULL,
  `colume_101` decimal(18,2) DEFAULT NULL,
  `colume_102` decimal(18,2) DEFAULT NULL,
  `colume_103` decimal(18,2) DEFAULT NULL,
  `colume_104` decimal(18,2) DEFAULT NULL,
  `colume_105` decimal(18,2) DEFAULT NULL,
  `colume_106` decimal(18,2) DEFAULT NULL,
  `colume_107` decimal(18,2) DEFAULT NULL,
  `colume_108` decimal(18,2) DEFAULT NULL,
  `colume_109` decimal(18,2) DEFAULT NULL,
  `colume_110` decimal(18,2) DEFAULT NULL,
  `colume_111` decimal(18,2) DEFAULT NULL,
  `colume_112` decimal(18,2) DEFAULT NULL,
  PRIMARY KEY (`code`,`report_date`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for capitalflows163
-- ----------------------------
DROP TABLE IF EXISTS `capitalflows163`;
CREATE TABLE `capitalflows163` (
  `Code` int(11) NOT NULL DEFAULT '0',
  `TradeDate` date NOT NULL DEFAULT '0000-00-00',
  `Close` decimal(10,2) DEFAULT NULL,
  `Change_Rate` decimal(10,2) DEFAULT NULL,
  `Turnover_Rate` decimal(10,2) DEFAULT NULL,
  `FlowIn` decimal(15,4) DEFAULT NULL,
  `FlowOut` decimal(15,4) DEFAULT NULL,
  `NetFlow` decimal(15,4) DEFAULT NULL,
  `MainFlowIn` decimal(15,4) DEFAULT NULL,
  `MainFlowOut` decimal(15,4) DEFAULT NULL,
  `NetMainFlow` decimal(15,4) DEFAULT NULL,
  PRIMARY KEY (`Code`,`TradeDate`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for capitalstructure
-- ----------------------------
DROP TABLE IF EXISTS `capitalstructure`;
CREATE TABLE `capitalstructure` (
  `Code` int(11) NOT NULL DEFAULT '0',
  `real_date` date NOT NULL DEFAULT '0000-00-00',
  `AShares` decimal(15,4) DEFAULT NULL,
  `BShares` decimal(15,4) DEFAULT NULL,
  `change_reason` int(11) DEFAULT NULL,
  `change_reason_desc` varchar(256) DEFAULT NULL,
  `seq` int(11) DEFAULT NULL,
  `total_shares` decimal(15,4) DEFAULT NULL,
  `A_total_shares` decimal(15,4) DEFAULT NULL,
  `download_date` date DEFAULT NULL,
  PRIMARY KEY (`Code`,`real_date`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for cashflows
-- ----------------------------
DROP TABLE IF EXISTS `cashflows`;
CREATE TABLE `cashflows` (
  `code` int(11) NOT NULL DEFAULT '0',
  `report_date` date NOT NULL DEFAULT '0000-00-00',
  `colume_1` decimal(18,2) DEFAULT NULL,
  `colume_2` decimal(18,2) DEFAULT NULL,
  `colume_3` decimal(18,2) DEFAULT NULL,
  `colume_4` decimal(18,2) DEFAULT NULL,
  `colume_5` decimal(18,2) DEFAULT NULL,
  `colume_6` decimal(18,2) DEFAULT NULL,
  `colume_7` decimal(18,2) DEFAULT NULL,
  `colume_8` decimal(18,2) DEFAULT NULL,
  `colume_9` decimal(18,2) DEFAULT NULL,
  `colume_10` decimal(18,2) DEFAULT NULL,
  `colume_11` decimal(18,2) DEFAULT NULL,
  `colume_12` decimal(18,2) DEFAULT NULL,
  `colume_13` decimal(18,2) DEFAULT NULL,
  `colume_14` decimal(18,2) DEFAULT NULL,
  `colume_15` decimal(18,2) DEFAULT NULL,
  `colume_16` decimal(18,2) DEFAULT NULL,
  `colume_17` decimal(18,2) DEFAULT NULL,
  `colume_18` decimal(18,2) DEFAULT NULL,
  `colume_19` decimal(18,2) DEFAULT NULL,
  `colume_20` decimal(18,2) DEFAULT NULL,
  `colume_21` decimal(18,2) DEFAULT NULL,
  `colume_22` decimal(18,2) DEFAULT NULL,
  `colume_23` decimal(18,2) DEFAULT NULL,
  `colume_24` decimal(18,2) DEFAULT NULL,
  `colume_25` decimal(18,2) DEFAULT NULL,
  `colume_26` decimal(18,2) DEFAULT NULL,
  `colume_27` decimal(18,2) DEFAULT NULL,
  `colume_28` decimal(18,2) DEFAULT NULL,
  `colume_29` decimal(18,2) DEFAULT NULL,
  `colume_30` decimal(18,2) DEFAULT NULL,
  `colume_31` decimal(18,2) DEFAULT NULL,
  `colume_32` decimal(18,2) DEFAULT NULL,
  `colume_33` decimal(18,2) DEFAULT NULL,
  `colume_34` decimal(18,2) DEFAULT NULL,
  `colume_35` decimal(18,2) DEFAULT NULL,
  `colume_36` decimal(18,2) DEFAULT NULL,
  `colume_37` decimal(18,2) DEFAULT NULL,
  `colume_38` decimal(18,2) DEFAULT NULL,
  `colume_39` decimal(18,2) DEFAULT NULL,
  `colume_40` decimal(18,2) DEFAULT NULL,
  `colume_41` decimal(18,2) DEFAULT NULL,
  `colume_42` decimal(18,2) DEFAULT NULL,
  `colume_43` decimal(18,2) DEFAULT NULL,
  `colume_44` decimal(18,2) DEFAULT NULL,
  `colume_45` decimal(18,2) DEFAULT NULL,
  `colume_46` decimal(18,2) DEFAULT NULL,
  `colume_47` decimal(18,2) DEFAULT NULL,
  `colume_48` decimal(18,2) DEFAULT NULL,
  `colume_49` decimal(18,2) DEFAULT NULL,
  `colume_50` decimal(18,2) DEFAULT NULL,
  `colume_51` decimal(18,2) DEFAULT NULL,
  `colume_52` decimal(18,2) DEFAULT NULL,
  `colume_53` decimal(18,2) DEFAULT NULL,
  `colume_54` decimal(18,2) DEFAULT NULL,
  `colume_55` decimal(18,2) DEFAULT NULL,
  `colume_56` decimal(18,2) DEFAULT NULL,
  `colume_57` decimal(18,2) DEFAULT NULL,
  `colume_58` decimal(18,2) DEFAULT NULL,
  `colume_59` decimal(18,2) DEFAULT NULL,
  `colume_60` decimal(18,2) DEFAULT NULL,
  `colume_61` decimal(18,2) DEFAULT NULL,
  `colume_62` decimal(18,2) DEFAULT NULL,
  `colume_63` decimal(18,2) DEFAULT NULL,
  `colume_64` decimal(18,2) DEFAULT NULL,
  `colume_65` decimal(18,2) DEFAULT NULL,
  `colume_66` decimal(18,2) DEFAULT NULL,
  `colume_67` decimal(18,2) DEFAULT NULL,
  `colume_68` decimal(18,2) DEFAULT NULL,
  `colume_69` decimal(18,2) DEFAULT NULL,
  `colume_70` decimal(18,2) DEFAULT NULL,
  `colume_71` decimal(18,2) DEFAULT NULL,
  `colume_72` decimal(18,2) DEFAULT NULL,
  `colume_73` decimal(18,2) DEFAULT NULL,
  `colume_74` decimal(18,2) DEFAULT NULL,
  `colume_75` decimal(18,2) DEFAULT NULL,
  `colume_76` decimal(18,2) DEFAULT NULL,
  `colume_77` decimal(18,2) DEFAULT NULL,
  `colume_78` decimal(18,2) DEFAULT NULL,
  `colume_79` decimal(18,2) DEFAULT NULL,
  `colume_80` decimal(18,2) DEFAULT NULL,
  `colume_81` decimal(18,2) DEFAULT NULL,
  `colume_82` decimal(18,2) DEFAULT NULL,
  `colume_83` decimal(18,2) DEFAULT NULL,
  `colume_84` decimal(18,2) DEFAULT NULL,
  `colume_85` decimal(18,2) DEFAULT NULL,
  `colume_86` decimal(18,2) DEFAULT NULL,
  `colume_87` decimal(18,2) DEFAULT NULL,
  `colume_88` decimal(18,2) DEFAULT NULL,
  `colume_89` decimal(18,2) DEFAULT NULL,
  `colume_90` decimal(18,2) DEFAULT NULL,
  `colume_91` decimal(18,2) DEFAULT NULL,
  `colume_92` decimal(18,2) DEFAULT NULL,
  PRIMARY KEY (`code`,`report_date`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for ccccc
-- ----------------------------
DROP TABLE IF EXISTS `ccccc`;
CREATE TABLE `ccccc` (
  `announcementId` varchar(32) NOT NULL DEFAULT '',
  `secCode` varchar(32) DEFAULT NULL,
  `secName` varchar(32) DEFAULT NULL,
  `announcementTime` date DEFAULT NULL,
  `announcementTitle` varchar(128) DEFAULT NULL,
  `adjunctUrl` varchar(128) DEFAULT NULL,
  `holder_num` int(11) NOT NULL DEFAULT '0',
  `share_holder` varchar(64) DEFAULT NULL,
  `number_of_holding` int(11) DEFAULT NULL,
  `share_percent` decimal(5,2) DEFAULT NULL,
  `number_of_restrict` int(11) DEFAULT NULL,
  `holder_character` varchar(32) DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for cffexfuturedaily
-- ----------------------------
DROP TABLE IF EXISTS `cffexfuturedaily`;
CREATE TABLE `cffexfuturedaily` (
  `trade_date` date NOT NULL DEFAULT '0000-00-00',
  `agreement_code` varchar(32) NOT NULL DEFAULT '',
  `open` decimal(10,2) DEFAULT NULL,
  `high` decimal(10,0) DEFAULT NULL,
  `low` decimal(10,2) DEFAULT NULL,
  `volume` int(11) DEFAULT NULL,
  `turnover` decimal(15,2) DEFAULT NULL,
  `holding` int(11) DEFAULT NULL,
  `close` decimal(10,2) DEFAULT NULL,
  `settlement` decimal(10,2) DEFAULT NULL,
  `change1` decimal(10,2) DEFAULT NULL,
  `change2` decimal(10,2) DEFAULT NULL,
  PRIMARY KEY (`trade_date`,`agreement_code`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for cfi_add_issuance
-- ----------------------------
DROP TABLE IF EXISTS `cfi_add_issuance`;
CREATE TABLE `cfi_add_issuance` (
  `Code` int(11) NOT NULL DEFAULT '0',
  `placement_year` date NOT NULL DEFAULT '0000-00-00',
  `placement_ratio` decimal(10,2) DEFAULT NULL,
  `placement_price` decimal(10,2) DEFAULT NULL,
  `plan_announcement_date` date DEFAULT NULL,
  `right_record_date` date DEFAULT NULL,
  `exit_right_date` date DEFAULT NULL,
  `placement_object` varchar(256) DEFAULT NULL,
  `market_date` date DEFAULT NULL,
  PRIMARY KEY (`Code`,`placement_year`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for cfi_bonus
-- ----------------------------
DROP TABLE IF EXISTS `cfi_bonus`;
CREATE TABLE `cfi_bonus` (
  `Code` int(11) NOT NULL DEFAULT '0',
  `bonus_year` date NOT NULL DEFAULT '0000-00-00',
  `bonus_share` decimal(10,2) DEFAULT NULL,
  `reserve_to_common_share` decimal(10,2) DEFAULT NULL,
  `bonus_cash` decimal(10,2) DEFAULT NULL,
  `plan_announcement_date` date DEFAULT NULL,
  `right_record_date` date DEFAULT NULL,
  `exit_right_date` date DEFAULT NULL,
  `event_state` varchar(64) DEFAULT NULL,
  PRIMARY KEY (`Code`,`bonus_year`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for cfi_placement_of_share
-- ----------------------------
DROP TABLE IF EXISTS `cfi_placement_of_share`;
CREATE TABLE `cfi_placement_of_share` (
  `Code` int(11) NOT NULL DEFAULT '0',
  `placement_year` date NOT NULL DEFAULT '0000-00-00',
  `placement_ratio` decimal(10,2) DEFAULT NULL,
  `placement_price` decimal(10,2) DEFAULT NULL,
  `plan_announcement_date` date DEFAULT NULL,
  `right_record_date` date DEFAULT NULL,
  `exit_right_date` date DEFAULT NULL,
  `placement_object` varchar(256) DEFAULT NULL,
  `market_date` date DEFAULT NULL,
  PRIMARY KEY (`Code`,`placement_year`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for cninfoannouncement
-- ----------------------------
DROP TABLE IF EXISTS `cninfoannouncement`;
CREATE TABLE `cninfoannouncement` (
  `secCode` varchar(16) DEFAULT NULL,
  `secName` varchar(32) DEFAULT NULL,
  `orgId` varchar(32) DEFAULT NULL,
  `announcementId` varchar(32) NOT NULL DEFAULT '',
  `announcementTitle` varchar(128) DEFAULT NULL,
  `announcementTime` date DEFAULT NULL,
  `adjunctUrl` varchar(128) DEFAULT NULL,
  `adjunctSize` int(11) DEFAULT NULL,
  `adjunctType` varchar(32) DEFAULT NULL,
  `storageTime` date DEFAULT NULL,
  `columnId` varchar(256) DEFAULT NULL,
  `pageColumn` varchar(256) DEFAULT NULL,
  `announcementType` varchar(32) DEFAULT NULL,
  `announcementTypeName` varchar(128) DEFAULT NULL,
  `pdf_already` int(11) DEFAULT NULL COMMENT '0',
  PRIMARY KEY (`announcementId`),
  KEY `idx_secCode` (`secCode`),
  KEY `idx_announcementTime` (`announcementTime`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for cninfoannouncepdf
-- ----------------------------
DROP TABLE IF EXISTS `cninfoannouncepdf`;
CREATE TABLE `cninfoannouncepdf` (
  `announcementId` varchar(32) NOT NULL DEFAULT '',
  `secCode` varchar(16) DEFAULT NULL,
  `secName` varchar(32) DEFAULT NULL,
  `announcementTime` date DEFAULT NULL,
  `announcementTitle` varchar(128) DEFAULT NULL,
  `adjunctUrl` varchar(128) DEFAULT NULL,
  `catalogNo` int(11) NOT NULL DEFAULT '0',
  `catalogName` varchar(128) DEFAULT NULL,
  `isDisplay` int(11) DEFAULT NULL,
  `catalogContent` varchar(4000) DEFAULT NULL,
  PRIMARY KEY (`announcementId`,`catalogNo`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for cninfo_get_land
-- ----------------------------
DROP TABLE IF EXISTS `cninfo_get_land`;
CREATE TABLE `cninfo_get_land` (
  `announcementId` varchar(32) NOT NULL DEFAULT '',
  `secCode` varchar(32) DEFAULT NULL,
  `secName` varchar(32) DEFAULT NULL,
  `announcementTime` date DEFAULT NULL,
  `announcementTitle` varchar(128) DEFAULT NULL,
  `adjunctUrl` varchar(128) DEFAULT NULL,
  `land_num` int(11) NOT NULL DEFAULT '0',
  `land_name` varchar(64) DEFAULT NULL,
  `deal_money` varchar(128) DEFAULT NULL,
  `rate_of_capacity` varchar(32) DEFAULT NULL,
  `Land_Purpose` varchar(32) DEFAULT NULL,
  `area_of_structure` varchar(64) DEFAULT NULL,
  `land_area` varchar(64) DEFAULT NULL,
  PRIMARY KEY (`announcementId`,`land_num`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for cninfo_incentive_plan
-- ----------------------------
DROP TABLE IF EXISTS `cninfo_incentive_plan`;
CREATE TABLE `cninfo_incentive_plan` (
  `announcementId` varchar(32) NOT NULL DEFAULT '',
  `secCode` varchar(32) DEFAULT NULL,
  `secName` varchar(32) DEFAULT NULL,
  `announcementTime` date DEFAULT NULL,
  `announcementTitle` varchar(128) DEFAULT NULL,
  `adjunctUrl` varchar(128) DEFAULT NULL,
  `content` varchar(1000) DEFAULT NULL,
  `base_from_year` int(11) DEFAULT NULL,
  `base_to_year` int(11) DEFAULT NULL,
  `incentive_year` int(11) NOT NULL DEFAULT '0',
  `increase_percent` decimal(6,2) DEFAULT NULL,
  `net_profit` decimal(15,3) DEFAULT NULL,
  `sales_from_operations` decimal(15,3) DEFAULT NULL,
  `downloadTime` date DEFAULT NULL,
  PRIMARY KEY (`announcementId`,`incentive_year`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for cninfo_share_holding
-- ----------------------------
DROP TABLE IF EXISTS `cninfo_share_holding`;
CREATE TABLE `cninfo_share_holding` (
  `announcementId` varchar(32) NOT NULL DEFAULT '',
  `secCode` varchar(32) DEFAULT NULL,
  `secName` varchar(32) DEFAULT NULL,
  `announcementTime` date DEFAULT NULL,
  `announcementTitle` varchar(128) DEFAULT NULL,
  `adjunctUrl` varchar(128) DEFAULT NULL,
  `holder_num` int(11) NOT NULL DEFAULT '0',
  `share_holder` varchar(64) DEFAULT NULL,
  `number_of_holding` int(11) DEFAULT NULL,
  `share_percent` decimal(5,2) DEFAULT NULL,
  `number_of_restrict` int(11) DEFAULT NULL,
  `holder_character` varchar(32) DEFAULT NULL,
  PRIMARY KEY (`announcementId`,`holder_num`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for cpdfanalysis
-- ----------------------------
DROP TABLE IF EXISTS `cpdfanalysis`;
CREATE TABLE `cpdfanalysis` (
  `announcementId` varchar(32) NOT NULL DEFAULT '',
  `code` int(11) NOT NULL DEFAULT '0',
  `content` varchar(1000) DEFAULT NULL,
  `parent_code` int(11) DEFAULT NULL,
  `announcementTitle` varchar(128) DEFAULT NULL,
  `adjunctUrl` varchar(128) DEFAULT NULL,
  PRIMARY KEY (`announcementId`,`code`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for csdcinvestorstat
-- ----------------------------
DROP TABLE IF EXISTS `csdcinvestorstat`;
CREATE TABLE `csdcinvestorstat` (
  `begin_date` date NOT NULL DEFAULT '0000-00-00',
  `end_date` date DEFAULT NULL,
  `final_a_natural_investor` decimal(12,2) DEFAULT NULL,
  `final_a_legal_investor` decimal(12,2) DEFAULT NULL,
  `final_b_natural_investor` decimal(12,2) DEFAULT NULL,
  `final_b_legal_investor` decimal(12,2) DEFAULT NULL,
  `final_a_position_investor` decimal(12,2) DEFAULT NULL,
  `final_b_position_investor` decimal(12,2) DEFAULT NULL,
  `period_a_trading_investor` decimal(12,2) DEFAULT NULL,
  `period_b_trading_investor` decimal(12,2) DEFAULT NULL,
  PRIMARY KEY (`begin_date`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for csdcsecurityissuestat
-- ----------------------------
DROP TABLE IF EXISTS `csdcsecurityissuestat`;
CREATE TABLE `csdcsecurityissuestat` (
  `begin_date` date NOT NULL DEFAULT '0000-00-00',
  `end_date` date DEFAULT NULL,
  `new_stock_num` int(11) DEFAULT NULL,
  `new_stock_capital` decimal(12,2) DEFAULT NULL,
  `new_stock_circulation_capital` decimal(12,2) DEFAULT NULL,
  `new_bond_num` int(11) DEFAULT NULL,
  `new_bond_denomination` decimal(12,2) DEFAULT NULL,
  `new_fundation_num` int(11) DEFAULT NULL,
  `new_fundation_denomination` decimal(12,2) DEFAULT NULL,
  PRIMARY KEY (`begin_date`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for csicsrcpedaily
-- ----------------------------
DROP TABLE IF EXISTS `csicsrcpedaily`;
CREATE TABLE `csicsrcpedaily` (
  `Code` varchar(32) NOT NULL DEFAULT '',
  `TradeDate` date NOT NULL DEFAULT '0000-00-00',
  `is_stock` int(11) NOT NULL DEFAULT '0',
  `name` varchar(128) DEFAULT NULL,
  `company_number` int(11) DEFAULT NULL,
  `parent_code` varchar(32) DEFAULT NULL,
  `static_pe` decimal(10,2) DEFAULT NULL,
  `dynamic_pe` decimal(10,2) DEFAULT NULL,
  `pb` decimal(10,2) DEFAULT NULL,
  `dividend_yield_ratio` decimal(10,2) DEFAULT NULL,
  PRIMARY KEY (`Code`,`TradeDate`,`is_stock`),
  KEY `idx_tradedate` (`TradeDate`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for csipedaily
-- ----------------------------
DROP TABLE IF EXISTS `csipedaily`;
CREATE TABLE `csipedaily` (
  `Code` int(11) NOT NULL DEFAULT '0',
  `TradeDate` date NOT NULL DEFAULT '0000-00-00',
  `is_stock` int(11) NOT NULL DEFAULT '0',
  `name` varchar(128) DEFAULT NULL,
  `company_number` int(11) DEFAULT NULL,
  `parent_code` int(11) DEFAULT NULL,
  `static_pe` decimal(10,2) DEFAULT NULL,
  `dynamic_pe` decimal(10,2) DEFAULT NULL,
  `pb` decimal(10,2) DEFAULT NULL,
  `dividend_yield_ratio` decimal(10,2) DEFAULT NULL,
  PRIMARY KEY (`Code`,`TradeDate`,`is_stock`),
  KEY `idx_tradedate` (`TradeDate`),
  KEY `idx_dynamic_pe` (`dynamic_pe`),
  KEY `idx_static_pe` (`static_pe`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for financereport
-- ----------------------------
DROP TABLE IF EXISTS `financereport`;
CREATE TABLE `financereport` (
  `Code` int(11) NOT NULL DEFAULT '0',
  `ReportDate` date NOT NULL DEFAULT '0000-00-00',
  `NetProfit` decimal(12,2) DEFAULT NULL,
  `PrimeOperatingRevenue` decimal(12,2) DEFAULT NULL,
  `TotalAssets` decimal(12,2) DEFAULT NULL,
  PRIMARY KEY (`Code`,`ReportDate`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for fundinsettlement
-- ----------------------------
DROP TABLE IF EXISTS `fundinsettlement`;
CREATE TABLE `fundinsettlement` (
  `begin_date` date NOT NULL DEFAULT '0000-00-00',
  `end_date` date DEFAULT NULL,
  `fund_type` varchar(32) NOT NULL DEFAULT '',
  `final_fund` decimal(12,2) DEFAULT NULL,
  `average_fund` decimal(12,2) DEFAULT NULL,
  `input_fund` decimal(12,2) DEFAULT NULL,
  `output_fund` decimal(12,2) DEFAULT NULL,
  `net_fund` decimal(12,2) DEFAULT NULL,
  `remark` varchar(128) DEFAULT NULL,
  PRIMARY KEY (`begin_date`,`fund_type`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for gross_profit
-- ----------------------------
DROP TABLE IF EXISTS `gross_profit`;
CREATE TABLE `gross_profit` (
  `code` int(11) NOT NULL DEFAULT '0',
  `report_date` date NOT NULL DEFAULT '0000-00-00',
  `sale_quarter` decimal(15,2) DEFAULT NULL,
  `cost_quarter` decimal(15,2) DEFAULT NULL,
  `taxs_quarter` decimal(15,2) DEFAULT NULL,
  `profit_quarter` decimal(15,2) DEFAULT NULL,
  `profit_rate_quarter` decimal(10,2) DEFAULT NULL,
  `profit_rate_4_quarter` decimal(10,2) DEFAULT NULL,
  `profit_rate_change_for_last` decimal(10,2) DEFAULT NULL,
  `profit_rate_change_for_average` decimal(10,2) DEFAULT NULL,
  PRIMARY KEY (`code`,`report_date`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for hexun_research_paper
-- ----------------------------
DROP TABLE IF EXISTS `hexun_research_paper`;
CREATE TABLE `hexun_research_paper` (
  `Code` int(11) NOT NULL DEFAULT '0',
  `report_date` date NOT NULL DEFAULT '0000-00-00',
  `title` varchar(256) NOT NULL DEFAULT '',
  `author` varchar(32) DEFAULT NULL,
  `source` varchar(32) DEFAULT NULL,
  `grade` varchar(32) DEFAULT NULL,
  `abstract` varchar(2000) DEFAULT NULL,
  `url` varchar(256) DEFAULT NULL,
  `initial_year` int(11) DEFAULT NULL,
  `first_eps` decimal(10,3) DEFAULT NULL,
  `second_eps` decimal(10,3) DEFAULT NULL,
  `third_eps` decimal(10,3) DEFAULT NULL,
  PRIMARY KEY (`Code`,`report_date`,`title`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for hushen_plate_index
-- ----------------------------
DROP TABLE IF EXISTS `hushen_plate_index`;
CREATE TABLE `hushen_plate_index` (
  `code` int(11) NOT NULL DEFAULT '0',
  `index_type` varchar(32) NOT NULL DEFAULT '',
  `name` varchar(256) DEFAULT NULL,
  `tradedate` date NOT NULL DEFAULT '0000-00-00',
  `circulating_value` decimal(16,2) DEFAULT NULL,
  `repair_value` decimal(14,2) DEFAULT NULL,
  `division` decimal(11,2) DEFAULT NULL,
  `point` decimal(8,2) DEFAULT NULL,
  PRIMARY KEY (`code`,`index_type`,`tradedate`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for hushen_stock_index
-- ----------------------------
DROP TABLE IF EXISTS `hushen_stock_index`;
CREATE TABLE `hushen_stock_index` (
  `code` int(11) NOT NULL DEFAULT '0',
  `abbreviation` varchar(32) DEFAULT NULL,
  `tradedate` date NOT NULL DEFAULT '0000-00-00',
  `circulating_value` decimal(15,2) DEFAULT NULL,
  `repair_value` decimal(13,2) DEFAULT NULL,
  `change_rate` decimal(8,2) DEFAULT NULL,
  `close` decimal(8,2) DEFAULT NULL,
  `close_index` decimal(8,2) DEFAULT NULL,
  `remark` varchar(32) DEFAULT NULL,
  PRIMARY KEY (`code`,`tradedate`),
  KEY `idx_code` (`code`),
  KEY `idx_tradedate` (`tradedate`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8
/*!50100 PARTITION BY RANGE (year(tradedate))
(PARTITION p2009 VALUES LESS THAN (2010) ENGINE = MyISAM,
 PARTITION p2010 VALUES LESS THAN (2011) ENGINE = MyISAM,
 PARTITION p2011 VALUES LESS THAN (2012) ENGINE = MyISAM,
 PARTITION p2012 VALUES LESS THAN (2013) ENGINE = MyISAM,
 PARTITION p2013 VALUES LESS THAN (2014) ENGINE = MyISAM,
 PARTITION p2014 VALUES LESS THAN (2015) ENGINE = MyISAM,
 PARTITION p2015 VALUES LESS THAN (2016) ENGINE = MyISAM,
 PARTITION p2016 VALUES LESS THAN (2017) ENGINE = MyISAM,
 PARTITION p2017 VALUES LESS THAN (2018) ENGINE = MyISAM,
 PARTITION p2018 VALUES LESS THAN (2019) ENGINE = MyISAM,
 PARTITION p2019 VALUES LESS THAN (2020) ENGINE = MyISAM,
 PARTITION p2020 VALUES LESS THAN (2021) ENGINE = MyISAM,
 PARTITION p2021 VALUES LESS THAN (2022) ENGINE = MyISAM,
 PARTITION p2022 VALUES LESS THAN (2023) ENGINE = MyISAM,
 PARTITION p2023 VALUES LESS THAN MAXVALUE ENGINE = MyISAM) */;

-- ----------------------------
-- Table structure for incomestatement
-- ----------------------------
DROP TABLE IF EXISTS `incomestatement`;
CREATE TABLE `incomestatement` (
  `code` int(11) NOT NULL DEFAULT '0',
  `report_date` date NOT NULL DEFAULT '0000-00-00',
  `colume_1` decimal(18,2) DEFAULT NULL,
  `colume_2` decimal(18,2) DEFAULT NULL,
  `colume_3` decimal(18,2) DEFAULT NULL,
  `colume_4` decimal(18,2) DEFAULT NULL,
  `colume_5` decimal(18,2) DEFAULT NULL,
  `colume_6` decimal(18,2) DEFAULT NULL,
  `colume_7` decimal(18,2) DEFAULT NULL,
  `colume_8` decimal(18,2) DEFAULT NULL,
  `colume_9` decimal(18,2) DEFAULT NULL,
  `colume_10` decimal(18,2) DEFAULT NULL,
  `colume_11` decimal(18,2) DEFAULT NULL,
  `colume_12` decimal(18,2) DEFAULT NULL,
  `colume_13` decimal(18,2) DEFAULT NULL,
  `colume_14` decimal(18,2) DEFAULT NULL,
  `colume_15` decimal(18,2) DEFAULT NULL,
  `colume_16` decimal(18,2) DEFAULT NULL,
  `colume_17` decimal(18,2) DEFAULT NULL,
  `colume_18` decimal(18,2) DEFAULT NULL,
  `colume_19` decimal(18,2) DEFAULT NULL,
  `colume_20` decimal(18,2) DEFAULT NULL,
  `colume_21` decimal(18,2) DEFAULT NULL,
  `colume_22` decimal(18,2) DEFAULT NULL,
  `colume_23` decimal(18,2) DEFAULT NULL,
  `colume_24` decimal(18,2) DEFAULT NULL,
  `colume_25` decimal(18,2) DEFAULT NULL,
  `colume_26` decimal(18,2) DEFAULT NULL,
  `colume_27` decimal(18,2) DEFAULT NULL,
  `colume_28` decimal(18,2) DEFAULT NULL,
  `colume_29` decimal(18,2) DEFAULT NULL,
  `colume_30` decimal(18,2) DEFAULT NULL,
  `colume_31` decimal(18,2) DEFAULT NULL,
  `colume_32` decimal(18,2) DEFAULT NULL,
  `colume_33` decimal(18,2) DEFAULT NULL,
  `colume_34` decimal(18,2) DEFAULT NULL,
  `colume_35` decimal(18,2) DEFAULT NULL,
  `colume_36` decimal(18,2) DEFAULT NULL,
  `colume_37` decimal(18,2) DEFAULT NULL,
  `colume_38` decimal(18,2) DEFAULT NULL,
  `colume_39` decimal(18,2) DEFAULT NULL,
  `colume_40` decimal(18,2) DEFAULT NULL,
  `colume_41` decimal(18,2) DEFAULT NULL,
  `colume_42` decimal(18,2) DEFAULT NULL,
  `colume_43` decimal(18,2) DEFAULT NULL,
  `colume_44` decimal(18,2) DEFAULT NULL,
  `colume_45` decimal(18,2) DEFAULT NULL,
  `colume_46` decimal(18,2) DEFAULT NULL,
  `colume_47` decimal(18,2) DEFAULT NULL,
  `colume_48` decimal(18,2) DEFAULT NULL,
  `colume_49` decimal(18,2) DEFAULT NULL,
  `release_date` date DEFAULT NULL,
  PRIMARY KEY (`code`,`report_date`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for macd
-- ----------------------------
DROP TABLE IF EXISTS `macd`;
CREATE TABLE `macd` (
  `code` int(11) NOT NULL DEFAULT '0',
  `trade_date` date NOT NULL DEFAULT '0000-00-00',
  `price` decimal(10,2) DEFAULT NULL,
  `a_total` decimal(15,2) DEFAULT NULL,
  `total_value` decimal(15,2) DEFAULT NULL,
  `ema12` decimal(15,2) DEFAULT NULL,
  `ema26` decimal(15,2) DEFAULT NULL,
  `dif` decimal(15,2) DEFAULT NULL,
  `dea` decimal(15,2) DEFAULT NULL,
  PRIMARY KEY (`code`,`trade_date`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for marketquotation
-- ----------------------------
DROP TABLE IF EXISTS `marketquotation`;
CREATE TABLE `marketquotation` (
  `Code` int(11) NOT NULL DEFAULT '0',
  `TradeDate` date NOT NULL DEFAULT '0000-00-00',
  `Name` varchar(256) DEFAULT NULL,
  `Yesterday_Close` decimal(10,2) DEFAULT NULL,
  `Today_Close` decimal(10,2) DEFAULT NULL,
  `Up_Down` decimal(10,2) DEFAULT NULL,
  `Turnover_Amount` decimal(15,2) DEFAULT NULL,
  `Price_Earnings_Ratio` decimal(10,2) DEFAULT NULL,
  PRIMARY KEY (`Code`,`TradeDate`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for market_value_daily
-- ----------------------------
DROP TABLE IF EXISTS `market_value_daily`;
CREATE TABLE `market_value_daily` (
  `code` int(11) NOT NULL DEFAULT '0',
  `trade_date` date NOT NULL DEFAULT '0000-00-00',
  `change_rate` decimal(10,2) DEFAULT NULL,
  `market_index` decimal(10,2) DEFAULT NULL,
  `market_index_change_rate` decimal(10,2) DEFAULT NULL,
  `close` decimal(10,4) DEFAULT NULL,
  `circulating_value` decimal(15,4) DEFAULT NULL,
  `total_value` decimal(15,4) DEFAULT NULL,
  `division_value` decimal(15,4) DEFAULT NULL,
  `stock_index` decimal(10,2) DEFAULT NULL,
  `stock_index_change_rate` decimal(10,2) DEFAULT NULL,
  `corrected_stock_index` decimal(10,2) DEFAULT NULL,
  PRIMARY KEY (`code`,`trade_date`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for market_value_daily_simple
-- ----------------------------
DROP TABLE IF EXISTS `market_value_daily_simple`;
CREATE TABLE `market_value_daily_simple` (
  `code` int(11) NOT NULL DEFAULT '0',
  `trade_date` date NOT NULL DEFAULT '0000-00-00',
  `change_rate` decimal(10,2) DEFAULT NULL,
  `market_index` decimal(10,2) DEFAULT NULL,
  `market_index_change_rate` decimal(10,2) DEFAULT NULL,
  `close` decimal(10,4) DEFAULT NULL,
  `stock_index` decimal(10,2) DEFAULT NULL,
  `close_corrected` decimal(10,2) DEFAULT NULL,
  `change_reason_desc` varchar(256) DEFAULT NULL,
  `curculate_rate` decimal(10,2) DEFAULT NULL,
  PRIMARY KEY (`code`,`trade_date`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for mcst
-- ----------------------------
DROP TABLE IF EXISTS `mcst`;
CREATE TABLE `mcst` (
  `code` int(11) NOT NULL DEFAULT '0',
  `trade_date` date NOT NULL DEFAULT '0000-00-00',
  `exit_right_date` date DEFAULT NULL,
  `weight` decimal(12,2) DEFAULT NULL,
  `cost_price` decimal(12,2) DEFAULT NULL,
  `mcst_price` decimal(10,2) DEFAULT NULL,
  `mcst_price_change` decimal(10,2) DEFAULT NULL,
  `mcst_price_change_rate` decimal(10,2) DEFAULT NULL,
  PRIMARY KEY (`code`,`trade_date`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for pedaily
-- ----------------------------
DROP TABLE IF EXISTS `pedaily`;
CREATE TABLE `pedaily` (
  `code` int(11) NOT NULL DEFAULT '0',
  `trade_date` date NOT NULL DEFAULT '0000-00-00',
  `base_date` date DEFAULT NULL,
  `net_profit` decimal(15,2) DEFAULT NULL,
  `gross_profit` decimal(15,2) DEFAULT NULL,
  `pe` decimal(10,2) DEFAULT NULL,
  `pe_gross` decimal(10,2) DEFAULT NULL,
  `total_shares` decimal(12,0) DEFAULT NULL,
  `total_value` decimal(15,2) DEFAULT NULL,
  `gross_profit_forecast` decimal(15,2) DEFAULT NULL,
  `deviation` decimal(10,2) DEFAULT NULL,
  PRIMARY KEY (`code`,`trade_date`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for selfselectstockplates
-- ----------------------------
DROP TABLE IF EXISTS `selfselectstockplates`;
CREATE TABLE `selfselectstockplates` (
  `Code` int(11) NOT NULL DEFAULT '0',
  `Name` varchar(256) DEFAULT NULL,
  `Company_Number` int(11) DEFAULT NULL,
  `Parent_Code` int(11) NOT NULL DEFAULT '0',
  `Is_Stock` int(11) DEFAULT NULL,
  PRIMARY KEY (`Code`,`Parent_Code`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for shanghai_capitalstructure
-- ----------------------------
DROP TABLE IF EXISTS `shanghai_capitalstructure`;
CREATE TABLE `shanghai_capitalstructure` (
  `code` int(11) NOT NULL DEFAULT '0',
  `real_date` date NOT NULL DEFAULT '0000-00-00',
  `abbreviation` varchar(32) DEFAULT NULL,
  `total_shares` decimal(10,2) DEFAULT NULL,
  `circulating_shares` decimal(10,2) DEFAULT NULL,
  `change_reason_desc` varchar(64) DEFAULT NULL,
  `prev_total` decimal(10,2) DEFAULT NULL,
  `prev_circulating` decimal(10,2) DEFAULT NULL,
  `prev_close` decimal(10,2) DEFAULT NULL,
  `prev_exit_right_price` decimal(10,2) DEFAULT NULL,
  `weight` decimal(10,2) DEFAULT NULL,
  PRIMARY KEY (`code`,`real_date`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for shanghai_index
-- ----------------------------
DROP TABLE IF EXISTS `shanghai_index`;
CREATE TABLE `shanghai_index` (
  `code` int(11) NOT NULL DEFAULT '0',
  `abbreviation` varchar(32) DEFAULT NULL,
  `tradedate` date NOT NULL DEFAULT '0000-00-00',
  `szindex` decimal(8,2) DEFAULT NULL,
  `circulating_value` decimal(15,2) DEFAULT NULL,
  `repair_circulating_value` decimal(12,2) DEFAULT NULL,
  `division` decimal(8,2) DEFAULT NULL,
  `self_index` decimal(8,2) DEFAULT NULL,
  `change_rate` decimal(8,2) DEFAULT NULL,
  PRIMARY KEY (`code`,`tradedate`),
  KEY `idx_code` (`code`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for shareholding
-- ----------------------------
DROP TABLE IF EXISTS `shareholding`;
CREATE TABLE `shareholding` (
  `Code` int(11) NOT NULL DEFAULT '0',
  `ReportDate` date NOT NULL DEFAULT '0000-00-00',
  `TotalShare` decimal(12,2) DEFAULT NULL,
  `CirculatingShare` decimal(12,2) DEFAULT NULL,
  PRIMARY KEY (`Code`,`ReportDate`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for sinadealdetail_20160727
-- ----------------------------
DROP TABLE IF EXISTS `sinadealdetail_20160727`;
CREATE TABLE `sinadealdetail_20160727` (
  `Code` int(11) NOT NULL DEFAULT '0',
  `deal_time` time NOT NULL DEFAULT '00:00:00',
  `deal_type` varchar(32) NOT NULL DEFAULT '',
  `deal_price` decimal(10,2) DEFAULT NULL,
  `deal_volume` decimal(12,0) DEFAULT NULL,
  `deal_turnover` decimal(15,2) DEFAULT NULL,
  PRIMARY KEY (`Code`,`deal_type`,`deal_time`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for sohufundhold
-- ----------------------------
DROP TABLE IF EXISTS `sohufundhold`;
CREATE TABLE `sohufundhold` (
  `Code` int(11) NOT NULL DEFAULT '0',
  `report_date` date NOT NULL DEFAULT '0000-00-00',
  `fund_code` int(11) NOT NULL DEFAULT '0',
  `fund_name` varchar(64) DEFAULT NULL,
  `value_hold` decimal(15,2) DEFAULT NULL,
  `number_hold` decimal(15,2) DEFAULT NULL,
  `percent` decimal(10,2) DEFAULT NULL,
  PRIMARY KEY (`Code`,`report_date`,`fund_code`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for sohuindexdaily
-- ----------------------------
DROP TABLE IF EXISTS `sohuindexdaily`;
CREATE TABLE `sohuindexdaily` (
  `Code` int(11) NOT NULL DEFAULT '0',
  `TradeDate` date NOT NULL DEFAULT '0000-00-00',
  `Open` decimal(10,2) DEFAULT NULL,
  `High` decimal(10,2) DEFAULT NULL,
  `Low` decimal(10,2) DEFAULT NULL,
  `Close` decimal(10,2) DEFAULT NULL,
  `Volume` int(11) DEFAULT NULL,
  `Change_Amount` decimal(10,2) DEFAULT NULL,
  `Change_Rate` decimal(10,2) DEFAULT NULL,
  `Turnover` decimal(15,4) DEFAULT NULL,
  PRIMARY KEY (`Code`,`TradeDate`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for sohumainincomecomposition
-- ----------------------------
DROP TABLE IF EXISTS `sohumainincomecomposition`;
CREATE TABLE `sohumainincomecomposition` (
  `Code` int(11) NOT NULL DEFAULT '0',
  `report_date` date NOT NULL DEFAULT '0000-00-00',
  `composition_type` varchar(64) NOT NULL DEFAULT '',
  `composition_name` varchar(64) NOT NULL DEFAULT '',
  `proportion_of_total_income` decimal(10,2) DEFAULT NULL,
  `income` decimal(15,4) DEFAULT NULL,
  `income_change_year_on_year` decimal(10,2) DEFAULT NULL,
  `cost` decimal(15,4) DEFAULT NULL,
  `gross_profit_rate` decimal(10,2) DEFAULT NULL,
  `gross_profit_rate_change_year_on_year` decimal(10,2) DEFAULT NULL,
  PRIMARY KEY (`Code`,`report_date`,`composition_type`,`composition_name`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for sohuperformanceforecast
-- ----------------------------
DROP TABLE IF EXISTS `sohuperformanceforecast`;
CREATE TABLE `sohuperformanceforecast` (
  `Code` int(11) NOT NULL DEFAULT '0',
  `report_date` date NOT NULL DEFAULT '0000-00-00',
  `report_type` varchar(32) DEFAULT NULL,
  `change_rate` decimal(10,4) DEFAULT NULL,
  PRIMARY KEY (`Code`,`report_date`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for souhustockplates
-- ----------------------------
DROP TABLE IF EXISTS `souhustockplates`;
CREATE TABLE `souhustockplates` (
  `Code` int(11) NOT NULL DEFAULT '0',
  `Name` varchar(256) DEFAULT NULL,
  `Company_Number` int(11) DEFAULT NULL,
  `Total_Volume` decimal(15,4) DEFAULT NULL,
  `Total_Turnover` decimal(15,4) DEFAULT NULL,
  `Parent_Code` int(11) NOT NULL DEFAULT '0',
  `Is_Stock` int(11) NOT NULL,
  PRIMARY KEY (`Code`,`Parent_Code`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for souhutradingdaily
-- ----------------------------
DROP TABLE IF EXISTS `souhutradingdaily`;
CREATE TABLE `souhutradingdaily` (
  `Code` int(11) NOT NULL DEFAULT '0',
  `TradeDate` date NOT NULL DEFAULT '0000-00-00',
  `Open` decimal(10,2) DEFAULT NULL,
  `High` decimal(10,2) DEFAULT NULL,
  `Low` decimal(10,2) DEFAULT NULL,
  `Close` decimal(10,2) DEFAULT NULL,
  `Volume` int(11) DEFAULT NULL,
  `Change_Amount` decimal(10,2) DEFAULT NULL,
  `Change_Rate` decimal(10,2) DEFAULT NULL,
  `Turnover` decimal(15,4) DEFAULT NULL,
  `Turnover_Rate` decimal(10,2) DEFAULT NULL,
  PRIMARY KEY (`Code`,`TradeDate`),
  KEY `idx_tradedate` (`TradeDate`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for ssemargintrading
-- ----------------------------
DROP TABLE IF EXISTS `ssemargintrading`;
CREATE TABLE `ssemargintrading` (
  `Code` int(11) NOT NULL DEFAULT '0',
  `TradeDate` date NOT NULL DEFAULT '0000-00-00',
  `name` varchar(32) DEFAULT NULL,
  `financing_buying` decimal(15,2) DEFAULT NULL,
  `financing_balance` decimal(15,2) DEFAULT NULL,
  `financing_payback` decimal(15,2) DEFAULT NULL,
  `stock_loan_selling` decimal(15,2) DEFAULT NULL,
  `stock_loan_remaider` decimal(15,2) DEFAULT NULL,
  `stock_loan_payback` decimal(15,2) DEFAULT NULL,
  PRIMARY KEY (`Code`,`TradeDate`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for statsgovfsjd
-- ----------------------------
DROP TABLE IF EXISTS `statsgovfsjd`;
CREATE TABLE `statsgovfsjd` (
  `zb_code` varchar(32) NOT NULL DEFAULT '',
  `zb_name` varchar(128) DEFAULT NULL,
  `reg_code` varchar(32) NOT NULL DEFAULT '',
  `reg_name` varchar(64) DEFAULT NULL,
  `sj_code` varchar(32) NOT NULL DEFAULT '',
  `data` decimal(15,2) DEFAULT NULL,
  `unit` varchar(32) DEFAULT NULL,
  `pid` varchar(32) DEFAULT NULL,
  PRIMARY KEY (`zb_code`,`sj_code`,`reg_code`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for statsgovfsnd
-- ----------------------------
DROP TABLE IF EXISTS `statsgovfsnd`;
CREATE TABLE `statsgovfsnd` (
  `zb_code` varchar(32) NOT NULL DEFAULT '',
  `zb_name` varchar(128) DEFAULT NULL,
  `reg_code` varchar(32) NOT NULL DEFAULT '',
  `reg_name` varchar(64) DEFAULT NULL,
  `sj_code` varchar(32) NOT NULL DEFAULT '',
  `data` decimal(15,2) DEFAULT NULL,
  `unit` varchar(32) DEFAULT NULL,
  `pid` varchar(32) DEFAULT NULL,
  PRIMARY KEY (`zb_code`,`sj_code`,`reg_code`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for statsgovfsyd
-- ----------------------------
DROP TABLE IF EXISTS `statsgovfsyd`;
CREATE TABLE `statsgovfsyd` (
  `zb_code` varchar(32) NOT NULL DEFAULT '',
  `zb_name` varchar(128) DEFAULT NULL,
  `reg_code` varchar(32) NOT NULL DEFAULT '',
  `reg_name` varchar(64) DEFAULT NULL,
  `sj_code` varchar(32) NOT NULL DEFAULT '',
  `data` decimal(15,2) DEFAULT NULL,
  `unit` varchar(32) DEFAULT NULL,
  `pid` varchar(32) DEFAULT NULL,
  PRIMARY KEY (`zb_code`,`sj_code`,`reg_code`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for statsgovhgjd
-- ----------------------------
DROP TABLE IF EXISTS `statsgovhgjd`;
CREATE TABLE `statsgovhgjd` (
  `zb_code` varchar(32) NOT NULL DEFAULT '',
  `zb_name` varchar(128) DEFAULT NULL,
  `sj_code` varchar(32) NOT NULL DEFAULT '',
  `data` decimal(15,2) DEFAULT NULL,
  `unit` varchar(32) DEFAULT NULL,
  `pid` varchar(32) DEFAULT NULL,
  PRIMARY KEY (`zb_code`,`sj_code`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for statsgovhgnd
-- ----------------------------
DROP TABLE IF EXISTS `statsgovhgnd`;
CREATE TABLE `statsgovhgnd` (
  `zb_code` varchar(32) NOT NULL DEFAULT '',
  `zb_name` varchar(128) DEFAULT NULL,
  `sj_code` varchar(32) NOT NULL DEFAULT '',
  `data` decimal(15,2) DEFAULT NULL,
  `unit` varchar(32) DEFAULT NULL,
  `pid` varchar(32) DEFAULT NULL,
  PRIMARY KEY (`zb_code`,`sj_code`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for statsgovhgyd
-- ----------------------------
DROP TABLE IF EXISTS `statsgovhgyd`;
CREATE TABLE `statsgovhgyd` (
  `zb_code` varchar(32) NOT NULL DEFAULT '',
  `zb_name` varchar(128) DEFAULT NULL,
  `sj_code` varchar(32) NOT NULL DEFAULT '',
  `data` decimal(15,2) DEFAULT NULL,
  `unit` varchar(32) DEFAULT NULL,
  `pid` varchar(32) DEFAULT NULL,
  PRIMARY KEY (`zb_code`,`sj_code`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for statsgovtree
-- ----------------------------
DROP TABLE IF EXISTS `statsgovtree`;
CREATE TABLE `statsgovtree` (
  `dbcode` varchar(32) NOT NULL DEFAULT '',
  `id` varchar(64) NOT NULL DEFAULT '',
  `isParent` int(11) DEFAULT NULL,
  `name` varchar(256) DEFAULT NULL,
  `pid` varchar(32) DEFAULT NULL,
  `wdcode` varchar(32) DEFAULT NULL,
  `updateDate` date DEFAULT NULL,
  PRIMARY KEY (`dbcode`,`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for stockinfo
-- ----------------------------
DROP TABLE IF EXISTS `stockinfo`;
CREATE TABLE `stockinfo` (
  `code` int(11) NOT NULL DEFAULT '0',
  `abbreviation` varchar(256) DEFAULT NULL,
  `name` varchar(256) DEFAULT NULL,
  `english_name` varchar(256) DEFAULT NULL,
  `register_addr` varchar(256) DEFAULT NULL,
  `a_code` int(11) DEFAULT NULL,
  `a_abbreviation` varchar(256) DEFAULT NULL,
  `a_time_to_market` date DEFAULT NULL,
  `a_total_share` int(11) DEFAULT NULL,
  `a_circulation_share` int(11) DEFAULT NULL,
  `b_code` int(11) DEFAULT NULL,
  `b_abbreviation` varchar(256) DEFAULT NULL,
  `b_time_to_market` date DEFAULT NULL,
  `b_total_share` int(11) DEFAULT NULL,
  `b_circulation_share` int(11) DEFAULT NULL,
  `area` varchar(256) DEFAULT NULL,
  `province` varchar(256) DEFAULT NULL,
  `city` varchar(256) DEFAULT NULL,
  `belong_to_industry` varchar(256) DEFAULT NULL,
  `web_addr` varchar(256) DEFAULT NULL,
  `stock_plate` varchar(256) DEFAULT NULL,
  `securities_exchange` varchar(256) DEFAULT NULL,
  `csi_first_industry_code` varchar(32) DEFAULT NULL,
  `csi_first_industry_name` varchar(64) DEFAULT NULL,
  `csi_second_industry_code` varchar(32) DEFAULT NULL,
  `csi_second_industry_name` varchar(64) DEFAULT NULL,
  `csi_third_industry_code` varchar(32) DEFAULT NULL,
  `csi_third_industry_name` varchar(64) DEFAULT NULL,
  `csi_fourth_industry_code` varchar(32) DEFAULT NULL,
  `csi_fourth_industry_name` varchar(64) DEFAULT NULL,
  `csrc_gate_industry_code` varchar(32) DEFAULT NULL,
  `csrc_gate_industry_name` varchar(64) DEFAULT NULL,
  `csrc_big_industry_code` varchar(32) DEFAULT NULL,
  `csrc_big_industry_name` varchar(64) DEFAULT NULL,
  `statsgov_area_code` varchar(32) DEFAULT NULL,
  `statsgov_area_name` varchar(64) DEFAULT NULL,
  `cninfo_orgid` varchar(32) DEFAULT NULL,
  `cninfo_category` varchar(32) DEFAULT NULL,
  `cninfo_code` varchar(32) DEFAULT NULL,
  `cninfo_pinyin` varchar(32) DEFAULT NULL,
  `cninfo_zwjc` varchar(32) DEFAULT NULL,
  `a_time_to_delisting` date DEFAULT NULL,
  `a_time_to_suspend` date DEFAULT NULL,
  PRIMARY KEY (`code`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for stockinfoex
-- ----------------------------
DROP TABLE IF EXISTS `stockinfoex`;
CREATE TABLE `stockinfoex` (
  `code` int(11) NOT NULL DEFAULT '0',
  `calculate_date` date DEFAULT NULL,
  `a_total_shares` decimal(15,4) DEFAULT NULL,
  `a_total_value` decimal(15,4) DEFAULT NULL,
  `earnings_per_share` decimal(15,4) DEFAULT NULL,
  `pe` decimal(15,4) DEFAULT NULL,
  `close` decimal(15,4) DEFAULT NULL,
  `abbreviation` varchar(256) DEFAULT NULL,
  `province` varchar(256) DEFAULT NULL,
  `belong_to_industry` varchar(256) DEFAULT NULL,
  `stock_plate` varchar(256) DEFAULT NULL,
  PRIMARY KEY (`code`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for szsemargintrading
-- ----------------------------
DROP TABLE IF EXISTS `szsemargintrading`;
CREATE TABLE `szsemargintrading` (
  `Code` int(11) NOT NULL DEFAULT '0',
  `TradeDate` date NOT NULL DEFAULT '0000-00-00',
  `name` varchar(32) DEFAULT NULL,
  `financing_buying` decimal(15,2) DEFAULT NULL,
  `financing_balance` decimal(15,2) DEFAULT NULL,
  `stock_loan_selling` decimal(15,2) DEFAULT NULL,
  `stock_loan_remaider` decimal(15,2) DEFAULT NULL,
  `stock_loan_balance` decimal(15,2) DEFAULT NULL,
  `financing_and_stock_loan_balance` decimal(15,2) DEFAULT NULL,
  PRIMARY KEY (`Code`,`TradeDate`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for test1
-- ----------------------------
DROP TABLE IF EXISTS `test1`;
CREATE TABLE `test1` (
  `code` int(11) NOT NULL DEFAULT '0',
  `real_date` date DEFAULT NULL,
  PRIMARY KEY (`code`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for tmmmp
-- ----------------------------
DROP TABLE IF EXISTS `tmmmp`;
CREATE TABLE `tmmmp` (
  `code` int(11) DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for tradingdaily
-- ----------------------------
DROP TABLE IF EXISTS `tradingdaily`;
CREATE TABLE `tradingdaily` (
  `Code` int(11) NOT NULL DEFAULT '0',
  `TradeDate` date NOT NULL DEFAULT '0000-00-00',
  `Open` decimal(10,2) DEFAULT NULL,
  `High` decimal(10,2) DEFAULT NULL,
  `Low` decimal(10,2) DEFAULT NULL,
  `Close` decimal(10,2) DEFAULT NULL,
  `Volume` int(11) DEFAULT NULL,
  `Adj_Close` decimal(10,2) DEFAULT NULL,
  PRIMARY KEY (`Code`,`TradeDate`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Procedure structure for add_col
-- ----------------------------
DROP PROCEDURE IF EXISTS `add_col`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `add_col`(IN p_tablename varchar(50), IN p_colname varchar(50), IN p_coltype varchar(20), IN p_comment varchar(255))
begin        DECLARE sql1 VARCHAR(200);        select count(*) into @cnt from information_schema.columns where table_name = p_tablename and column_name = p_colname;        if @cnt = 0 then         set @sql1=concat('ALTER table ', p_tablename, ' ADD COLUMN ', p_colname, ' ', p_coltype, ' COMMENT \'', p_comment, '\'');         prepare stmt FROM @sql1;         execute stmt;        end if;         end
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for add_col2
-- ----------------------------
DROP PROCEDURE IF EXISTS `add_col2`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `add_col2`(IN p_tablename varchar(50), IN p_colname varchar(50), IN p_coltype varchar(600), IN p_comment varchar(255), IN p_default varchar(50))
begin
	DECLARE sql1 VARCHAR(200);
	select count(*) into @cnt from information_schema.columns where table_name = p_tablename and column_name = p_colname;
	if @cnt = 0 then
		set @sql1=concat('ALTER table ', p_tablename, ' ADD COLUMN ', p_colname, ' ', p_coltype, ' COMMENT \'', p_comment, '\'',' DEFAULT ',p_default);
		prepare stmt FROM @sql1;
		execute stmt;
	end if;
end
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for add_ind
-- ----------------------------
DROP PROCEDURE IF EXISTS `add_ind`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `add_ind`(IN p_tablename varchar(50), IN p_colname varchar(500), IN p_idxname VARCHAR(50))
begin
DECLARE str VARCHAR(200);
set @str=concat('alter table ',p_tablename,' add index ',p_idxname,'(', p_colname,');');
select count(*) into @cnt from information_schema.statistics where table_name = p_tablename and index_name = p_idxname;
if @cnt = 0 then
prepare stmt FROM @str;
execute stmt;
end if;
end
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for add_table
-- ----------------------------
DROP PROCEDURE IF EXISTS `add_table`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `add_table`(IN p_tablename varchar(50), IN p_sql varchar(5000))
begin
	DECLARE sql1 VARCHAR(5000);
	select count(*) into @cnt from information_schema.tables where table_name = p_tablename;
	if @cnt = 0 then
		set @sql1=p_sql;
		prepare stmt FROM @sql1;
		execute stmt;
	end if;
end
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for CalculateAllStockPe
-- ----------------------------
DROP PROCEDURE IF EXISTS `CalculateAllStockPe`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `CalculateAllStockPe`(`fromDate` date)
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
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for CalculateAllStockRecentGrossProfit
-- ----------------------------
DROP PROCEDURE IF EXISTS `CalculateAllStockRecentGrossProfit`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `CalculateAllStockRecentGrossProfit`()
    READS SQL DATA
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
			select count(*) into existTable from information_schema.tables where table_name = 'gross_profit';
			if existTable = 0 THEN
				call CalculateGrossProfit(nCode, '1990-1-1');
			ELSE
				select max(report_date) into lastDate from gross_profit where code = nCode;
				if  lastDate = ''  or lastDate is null THEN
					call CalculateGrossProfit(nCode, '1990-1-1');
				else
					# select nCode, lastDate;
					call CalculateGrossProfit(nCode, lastDate);
				end if;
				
			end if;
		end if;
		fetch next from rs into nCode;
	until Done end repeat;
	close rs;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for CalculateAllStockRecentMACD
-- ----------------------------
DROP PROCEDURE IF EXISTS `CalculateAllStockRecentMACD`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `CalculateAllStockRecentMACD`()
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
				call CalculateMACD(nCode, '2013-1-1');
			ELSE
				set lastDate = MaxDateForMACD(nCode);
				if  lastDate = ''  or lastDate is null THEN
					call CalculateMACD(nCode, '2013-1-1');
				else
					call CalculateMACD(nCode, lastDate);
				end if;
			end if;
		end if;
		fetch next from rs into nCode;
	until Done end repeat;
	close rs;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for CalculateAllStockRecentMarketValue
-- ----------------------------
DROP PROCEDURE IF EXISTS `CalculateAllStockRecentMarketValue`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `CalculateAllStockRecentMarketValue`()
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
			select count(*) into existTable from information_schema.tables where table_name = 'market_value_daily';
			if existTable = 0 THEN
				call CalculateMarketValue(nCode, '2012-1-1');
			ELSE
				select max(trade_date) into lastDate from market_value_daily 
					where code = nCode and market_index is not null ;
				if  lastDate = ''  or lastDate is null THEN
					call CalculateMarketValue(nCode, '2012-1-1');
				else
					call CalculateMarketValue(nCode, lastDate);
				end if;
			end if;
		end if;
		fetch next from rs into nCode;
	until Done end repeat;
	close rs;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for CalculateAllStockRecentMarketValueSimple
-- ----------------------------
DROP PROCEDURE IF EXISTS `CalculateAllStockRecentMarketValueSimple`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `CalculateAllStockRecentMarketValueSimple`()
    MODIFIES SQL DATA
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
			select count(*) into existTable from information_schema.tables where table_name = 'market_value_daily_simple';
			if existTable = 0 THEN
				call CalculateMarketValueSimple(nCode, '2010-1-1');
			ELSE
				select max(trade_date) into lastDate from market_value_daily_simple 
					where code = nCode and market_index is not null ;
				if  lastDate = ''  or lastDate is null THEN
					call CalculateMarketValueSimple(nCode, '2010-1-1');
				else
					call CalculateMarketValueSimple(nCode, lastDate);
				end if;
			end if;
		end if;
		fetch next from rs into nCode;
	until Done end repeat;
	close rs;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for CalculateAllStockRecentMCST
-- ----------------------------
DROP PROCEDURE IF EXISTS `CalculateAllStockRecentMCST`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `CalculateAllStockRecentMCST`()
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
			select count(*) into existTable from information_schema.tables where table_name = 'MCST';
			if existTable = 0 THEN
				call CalculateMCST(nCode, '2013-1-1');
			ELSE
				set lastDate = MaxDateForMCST(nCode);
				if  lastDate = ''  or lastDate is null THEN
					call CalculateMCST(nCode, '2013-1-1');
				else
					call CalculateMCST(nCode, lastDate);
				end if;
				
			end if;
		end if;
		fetch next from rs into nCode;
	until Done end repeat;
	close rs;

END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for CalculateAllStockRecentPe
-- ----------------------------
DROP PROCEDURE IF EXISTS `CalculateAllStockRecentPe`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `CalculateAllStockRecentPe`()
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
				call CalculatePe(nCode, '2013-1-1');
			ELSE
				select max(trade_date) into lastDate from pedaily where code = nCode ;
				if  lastDate = ''  or lastDate is null THEN
					call CalculatePe(nCode, '2013-1-1');
				else
					call CalculatePe(nCode, lastDate);
				end if;
			end if;
		end if;
		fetch next from rs into nCode;
	until Done end repeat;
	close rs;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for CalculateGrossProfit
-- ----------------------------
DROP PROCEDURE IF EXISTS `CalculateGrossProfit`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `CalculateGrossProfit`(IN `p_nCode` int,IN `p_fromDate` date)
    MODIFIES SQL DATA
BEGIN
	#Routine body goes here...
	DECLARE p_report_date date;
	DECLARE p_report_date_1 date;
	DECLARE p_report_date_2 date;
	DECLARE p_report_date_3 date;
	DECLARE p_report_date_4 date;
	DECLARE p_sale DOUBLE;
	DECLARE p_cost DOUBLE;
	DECLARE p_taxs DOUBLE;
	declare p_profit double;
	declare p_profit_rate double;
	declare p_profit_rate_1 double;
	declare p_profit_rate_2 double;
	declare p_profit_rate_3 double;
	declare p_profit_rate_4 double;
	DECLARE p_sale_1 DOUBLE;
	DECLARE p_cost_1 DOUBLE;
	DECLARE p_taxs_1 DOUBLE;

	DECLARE isExist int;
	DECLARE	Done int DEFAULT 0;
	
	declare rs CURSOR for select report_date, DATE_SUB(report_date,INTERVAL 3 MONTH) as report_date_1,
		DATE_SUB(report_date,INTERVAL 6 MONTH) as report_date_2,
		DATE_SUB(report_date,INTERVAL 9 MONTH) as report_date_3,
		DATE_SUB(report_date,INTERVAL 12 MONTH) as report_date_4
		from incomestatement where code = p_nCode and report_date > p_fromDate and  
		report_date >= ( select date_sub(a_time_to_market, INTERVAL 6 MONTH) from stockinfo where code = p_nCode )
		order by report_date ;

	DECLARE CONTINUE HANDLER FOR NOT FOUND SET Done = 1;

	create table if not exists gross_profit ( code int , report_date date,
		sale_quarter Decimal(15,2), cost_quarter Decimal(15,2), taxs_quarter Decimal(15,2),
		profit_quarter Decimal(15,2), profit_rate_quarter Decimal(10,2), profit_rate_4_quarter Decimal(10,2), 
		profit_rate_change_for_last Decimal(10,2), profit_rate_change_for_average Decimal(10,2), PRIMARY key( code, report_date ));

	open rs;
	fetch next from rs into p_report_date, p_report_date_1, p_report_date_2, p_report_date_3, p_report_date_4;

label:
	REPEAT
		if Done THEN
			leave label;
		ELSE
			
			# 获取上一季度的主营收入、主营成本、税金及附加, 计算本季度毛利及毛利率
			select count(*) into isExist from gross_profit where code = p_nCode 
			and EXTRACT(YEAR_MONTH FROM report_date) = EXTRACT(YEAR_MONTH FROM p_report_date_1);
			if isExist > 0 THEN
				select colume_2 as sale, colume_9 as cost, colume_20 as taxs 
				into p_sale_1, p_cost_1, p_taxs_1  from incomestatement where code = p_nCode 
				and EXTRACT(YEAR_MONTH FROM report_date) = EXTRACT(YEAR_MONTH FROM p_report_date_1);
			ELSE
				set p_sale_1 = null;
				set p_cost_1 = null;
				set p_taxs_1 = null;
			end if;

			# 获取本季度的主营收入、主营成本、税金及附加
			select colume_2 as sale, colume_9 as cost, colume_20 as taxs 
			into p_sale, p_cost, p_taxs  from incomestatement where code = p_nCode 
			and EXTRACT(YEAR_MONTH FROM report_date) = EXTRACT(YEAR_MONTH FROM p_report_date);

			if p_sale is not null and p_cost is not null and p_taxs is not null  and 
				(( p_sale_1 is not null and p_cost_1 is not null and p_taxs_1 is not null )
				or ( MONTH(p_report_date) = 3 )) THEN
				
				if month(p_report_date) <> 3 then
					set p_sale = p_sale - p_sale_1;
					set p_cost = p_cost - p_cost_1;
					set p_taxs = p_taxs - p_taxs_1;
				end if;
				set p_profit = p_sale - p_cost - p_taxs;
				set p_profit_rate = p_profit * 100 / p_sale;

				select count(*) into isExist from gross_profit where code = p_nCode and report_date = p_report_date;
				if isExist = 0 THEN
					insert into gross_profit ( code, report_date, sale_quarter, cost_quarter, taxs_quarter, 
					profit_quarter, profit_rate_quarter )
					select p_nCode, p_report_date, p_sale, p_cost, p_taxs, p_profit, p_profit_rate from dual;

				ELSE
					update gross_profit set  sale_quarter = p_sale, cost_quarter = p_cost, taxs_quarter = p_taxs,
					profit_quarter = p_profit, profit_rate_quarter = p_profit_rate 
					where code = p_nCode and report_date = p_report_date;
				end if;
			end if;
		
			# 获取前几个季度的毛利率，计算毛利率的变动率
			select count(profit_rate_quarter) into isExist from gross_profit where code = p_nCode 
			and EXTRACT(YEAR_MONTH FROM report_date) = EXTRACT(YEAR_MONTH FROM p_report_date);
			if isExist >0 THEN
				select profit_rate_quarter into p_profit_rate from gross_profit where code = p_nCode 
				and EXTRACT(YEAR_MONTH FROM report_date) = EXTRACT(YEAR_MONTH FROM p_report_date);
			ELSE
				set p_profit_rate = NULL;
			end if;

			select count(profit_rate_quarter) into isExist from gross_profit where code = p_nCode 
			and EXTRACT(YEAR_MONTH FROM report_date) = EXTRACT(YEAR_MONTH FROM p_report_date_1);
			if isExist >0 THEN
				select profit_rate_quarter into p_profit_rate_1 from gross_profit where code = p_nCode 
				and EXTRACT(YEAR_MONTH FROM report_date) = EXTRACT(YEAR_MONTH FROM p_report_date_1);
			ELSE
				set p_profit_rate_1 = NULL;
			end if;

			select count(profit_rate_quarter) into isExist from gross_profit where code = p_nCode 
			and EXTRACT(YEAR_MONTH FROM report_date) = EXTRACT(YEAR_MONTH FROM p_report_date_2);
			if isExist >0 THEN
				select profit_rate_quarter into p_profit_rate_2 from gross_profit where code = p_nCode 
				and EXTRACT(YEAR_MONTH FROM report_date) = EXTRACT(YEAR_MONTH FROM p_report_date_2);
			ELSE
				set p_profit_rate_2 = NULL;
			end if;

			select count(profit_rate_quarter) into isExist from gross_profit where code = p_nCode 
			and EXTRACT(YEAR_MONTH FROM report_date) = EXTRACT(YEAR_MONTH FROM p_report_date_3);
			if isExist >0 THEN
				select profit_rate_quarter into p_profit_rate_3 from gross_profit where code = p_nCode 
				and EXTRACT(YEAR_MONTH FROM report_date) = EXTRACT(YEAR_MONTH FROM p_report_date_3);
			ELSE
				set p_profit_rate_3 = NULL;
			end if;

			select count(profit_rate_quarter) into isExist from gross_profit where code = p_nCode 
			and EXTRACT(YEAR_MONTH FROM report_date) = EXTRACT(YEAR_MONTH FROM p_report_date_4);
			if isExist >0 THEN
				select profit_rate_quarter into p_profit_rate_4 from gross_profit where code = p_nCode 
				and EXTRACT(YEAR_MONTH FROM report_date) = EXTRACT(YEAR_MONTH FROM p_report_date_4);
			ELSE
				set p_profit_rate_4 = NULL;
			end if;
			
			# 这一季度与上一季的毛利率变动比
			if p_profit_rate is not null and p_profit_rate_1 is not null THEN
				update gross_profit set profit_rate_change_for_last = (p_profit_rate - p_profit_rate_1) * 100 / ( p_profit_rate_1 + 100 )
				where code = p_nCode and report_date = p_report_date;
			end if;

			# 四个季度平均值的毛利率(含本季度)
			if p_profit_rate is not null and p_profit_rate_1 is not null and 
				p_profit_rate_2 is not null and p_profit_rate_3 is not null then
				update gross_profit set profit_rate_4_quarter = (p_profit_rate + p_profit_rate_1 + p_profit_rate_2 + p_profit_rate_3 ) / 4
				where code = p_nCode and report_date = p_report_date;
			end if;

			# 这一季度与前四个季度平均值的毛利率变动比
			if p_profit_rate is not null and p_profit_rate_1 is not null and 
				p_profit_rate_2 is not null and p_profit_rate_3 is not null and 
				p_profit_rate_4 is not null THEN
				set p_profit_rate_1 = (p_profit_rate_1 + p_profit_rate_2 + p_profit_rate_3 + p_profit_rate_4) / 4;
				update gross_profit set profit_rate_change_for_average = (p_profit_rate - p_profit_rate_1) * 100 / (p_profit_rate_1 + 100)
 				where code = p_nCode and report_date = p_report_date;
			end if;
		
		end if;
		fetch next from rs into p_report_date, p_report_date_1, p_report_date_2, p_report_date_3, p_report_date_4;
	until Done end repeat;
	close rs;

END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for CalculateMACD
-- ----------------------------
DROP PROCEDURE IF EXISTS `CalculateMACD`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `CalculateMACD`(`nCode` int,`fromDate` date)
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
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for CalculateMarketValue
-- ----------------------------
DROP PROCEDURE IF EXISTS `CalculateMarketValue`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `CalculateMarketValue`(IN `p_nCode` int,IN `p_fromDate` date)
BEGIN
	#Routine body goes here...
	declare p_odt_cur date;
	declare p_yesterday date;
	declare p_close double;
	declare p_yesterday_close double;
	declare p_temp_close double;
	declare p_change_rate double;
	declare p_market_index double;				/* 大盘指数 */
	declare p_market_index_change_rate double;		/* 大盘指数变化率 */
		
	declare p_yesterday_total_value double;
	declare p_a_total_share double;
	declare p_a_circulating_share double;
	declare p_is_bonus_share int; 

	declare p_circulating_value double;
	declare p_total_value double;
	declare p_corrected_value double;
	declare p_division_value double;
	declare p_stock_index double;
	declare p_yesterday_stock_index double;
	declare p_corrected_stock_index double;
	declare p_stock_index_change_rate double;
	
	declare p_first_date date;
	declare p_isFirst int DEFAULT 1;
	DECLARE isExist int;

	DECLARE	Done int DEFAULT 0;
	
	declare rs CURSOR for select  
	tradeDate, close,  Change_Rate , market_index,  market_index_change_rate 
	from (select code, TradeDate, close,  a.Change_Rate, 
	(select b.close from sohuindexdaily b where b.code=1 and b.TradeDate=a.TradeDate) as market_index,
	(select b.Change_Rate from sohuindexdaily b where b.code=1 and b.TradeDate=a.TradeDate) as market_index_change_rate 
	from souhutradingdaily a where code = p_nCode and TradeDate >= p_fromDate ) c ORDER BY tradeDate;

	DECLARE CONTINUE HANDLER FOR NOT FOUND SET Done = 1;

	create table if not exists market_value_daily ( code int , trade_date date, 
		change_rate decimal(10,2), market_index DECIMAL(10,2), market_index_change_rate DECIMAL(10,2), close decimal(10,4),
		circulating_value DECIMAL(15,4),  total_value DECIMAL(15,4), division_value DECIMAL(15,4),  
		stock_index Decimal(10,2), stock_index_change_rate decimal(10,2), 
		corrected_stock_index Decimal(10,2), PRIMARY KEY ( code, trade_date ));

	open rs;
	fetch next from rs into p_odt_cur, p_close, p_change_rate, p_market_index, p_market_index_change_rate;

label:
	REPEAT
		if Done THEN
			leave label;
		ELSE
			Call CurrentCapitalStructure(p_nCode, p_odt_cur, p_a_circulating_share, p_a_total_share, p_is_bonus_share);
			#股本结构还未下载
			if p_a_total_share <=0  THEN
				leave label;
			end if;
			
			/* 根据总市值 和 流通市值 */
			set p_circulating_value = p_close * p_a_circulating_share;
			set p_total_value = p_close * p_a_total_share;
			
			/* 确定除数 */
			if p_isFirst =1 THEN
				select max(trade_date) into p_first_date from market_value_daily 
					where code = p_nCode and trade_date < p_odt_cur;
				if p_first_date is not null THEN
					select  division_value, close, total_value, stock_index, corrected_stock_index
						into p_division_value, p_yesterday_close, p_yesterday_total_value, p_yesterday_stock_index, p_corrected_stock_index 
						from market_value_daily  where  code = p_nCode and  trade_date = p_first_date; 
					set p_temp_close = YesterdayClosePriceAfterExitRight_1(p_nCode, p_odt_cur, p_yesterday_close, p_first_date);
					/* 明日是除息除权日 */
					if p_yesterday_close<> p_temp_close THEN
						/* 修正后的总市值 */
						set p_corrected_value = p_temp_close * p_a_total_share;
						/* 修改后总市值 * 原除数 / 修正前总市值 = 新除数 */
						set p_division_value = p_corrected_value * p_division_value / p_yesterday_total_value;
					end if;
					/* 明日是股票增发上市日 */
					if IsAddIssuanceMarketDate(p_nCode, p_odt_cur, p_first_date) = 1 THEN
						/* 修正后的总市值 */
						set p_corrected_value = p_yesterday_close * p_a_total_share;
						/* 修改后总市值 * 原除数 / 修正前总市值 = 新除数 */
						set p_division_value = p_corrected_value * p_division_value / p_yesterday_total_value;
					end if;
				else 
					set p_yesterday_stock_index = 100; 
					set p_corrected_stock_index = 100;
					set p_division_value =  p_total_value;
				end if;
				set p_isFirst = 0;
			ELSE
				set p_temp_close = YesterdayClosePriceAfterExitRight_1(p_nCode, p_odt_cur, p_yesterday_close, p_yesterday);
				/* 明日是除息除权日 */
				if p_yesterday_close<> p_temp_close THEN
					/* 修正后的总市值 */
					set p_corrected_value = p_temp_close * p_a_total_share;
					/* 修改后总市值 * 原除数 / 修正前总市值 = 新除数 */
					set p_division_value = p_corrected_value * p_division_value / p_yesterday_total_value;
				end if;
				/* 明日是股票增发上市日 */
				if IsAddIssuanceMarketDate(p_nCode, p_odt_cur, p_yesterday) = 1 THEN
					/* 修正后的总市值 */
					set p_corrected_value = p_yesterday_close * p_a_total_share;
					/* 修改后总市值 * 原除数 / 修正前总市值 = 新除数 */
					set p_division_value = p_corrected_value * p_division_value / p_yesterday_total_value;
				end if;
			end if;

			/* 计算个股指数, 基准指数为 100 */
			set p_stock_index = 100 * p_total_value / p_division_value;
			set p_yesterday_stock_index = (p_stock_index - p_yesterday_stock_index)/p_yesterday_stock_index;  # 计算指数涨跌率
			set p_corrected_stock_index = p_corrected_stock_index * ( 1+ p_yesterday_stock_index - p_market_index_change_rate/100);
			

			select count(*) into isExist from market_value_daily where code = p_nCode and trade_date = p_odt_cur;
			if isExist = 0 THEN
				insert into market_value_daily (code, trade_date, market_index, market_index_change_rate, close , circulating_value, 
					total_value, division_value, stock_index, corrected_stock_index, change_rate, stock_index_change_rate )
					select p_nCode, p_odt_cur, p_market_index, p_market_index_change_rate, p_close, p_circulating_value, p_total_value, 
						p_division_value, p_stock_index, p_corrected_stock_index, p_change_rate, p_yesterday_stock_index * 100 from dual ; 
			ELSE
				update market_value_daily set market_index_change_rate = p_market_index_change_rate, 
					market_index = p_market_index,
					close = p_close, circulating_value = p_circulating_value,
					total_value = p_total_value, division_value = p_division_value,
					stock_index = p_stock_index, change_rate = p_change_rate,
					corrected_stock_index = p_corrected_stock_index,
					stock_index_change_rate = p_yesterday_stock_index * 100 
					where code = p_nCode and trade_date = p_odt_cur;
			end if;
	
			set p_yesterday_close = p_close;
			set p_yesterday = p_odt_cur;
			set p_yesterday_total_value = p_total_value;
			set p_yesterday_stock_index = p_stock_index;
			
		end if;
		fetch next from rs into p_odt_cur, p_close, p_change_rate, p_market_index, p_market_index_change_rate;
	until Done end repeat;
	close rs;

END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for CalculateMarketValueSimple
-- ----------------------------
DROP PROCEDURE IF EXISTS `CalculateMarketValueSimple`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `CalculateMarketValueSimple`(IN `p_nCode` int,IN `p_fromDate` date)
    MODIFIES SQL DATA
BEGIN
	#Routine body goes here...
	declare p_odt_cur date;
	declare p_close double;
	declare p_close_corrected double;
	declare p_yesterday_close_corrected double;
	
	declare p_change_rate double;
	declare p_market_index double;				/* 大盘指数 */
	declare p_market_index_change_rate double;		/* 大盘指数变化率 */
		
	declare p_stock_index double;
	declare p_yesterday_stock_index double;
	declare p_curculate_rate double;
	declare p_yesterday_curculate_rate double;
	declare p_change_reason_desc varchar(128);
		
	declare p_first_date date;
	declare p_isFirst int DEFAULT 1;
	DECLARE isExist int;

	DECLARE	Done int DEFAULT 0;
	
	declare rs CURSOR for select  
	tradeDate, close,  Change_Rate , market_index,  market_index_change_rate, change_reason_desc, curculate_rate
	from (select code, TradeDate, close,  a.Change_Rate, 
	(select b.change_reason_desc  from capitalstructure b where b.code=a.code and b.real_date=TradeDate ) as change_reason_desc,
	(select b.AShares/b.A_total_shares  from capitalstructure b where b.code=a.code and b.real_date=TradeDate ) as curculate_rate,
	(select b.close from sohuindexdaily b where b.code=1 and b.TradeDate=a.TradeDate) as market_index,
	(select b.Change_Rate from sohuindexdaily b where b.code=1 and b.TradeDate=a.TradeDate) as market_index_change_rate 
	from souhutradingdaily a where code = p_nCode and TradeDate >= p_fromDate ) c ORDER BY tradeDate;

	DECLARE CONTINUE HANDLER FOR NOT FOUND SET Done = 1;

	create table if not exists market_value_daily_simple ( code int , trade_date date, 
		change_rate decimal(10,2), market_index DECIMAL(10,2), market_index_change_rate DECIMAL(10,2), close decimal(10,4),
		stock_index Decimal(10,2), close_corrected Decimal(10,2), change_reason_desc varchar(256),
		curculate_rate Decimal(10,2), PRIMARY KEY ( code, trade_date ));

	open rs;
	fetch next from rs into p_odt_cur, p_close, p_change_rate, p_market_index, p_market_index_change_rate, p_change_reason_desc, p_curculate_rate;

label:
	REPEAT
		if Done THEN
			leave label;
		ELSE
							
			if p_isFirst =1 THEN
				select max(trade_date) into p_first_date from market_value_daily_simple 
					where code = p_nCode and trade_date < p_odt_cur;
				if p_first_date is not null THEN
					select  stock_index, close_corrected, curculate_rate into  p_yesterday_stock_index, p_yesterday_close_corrected, p_yesterday_curculate_rate
						from market_value_daily_simple  where  code = p_nCode and  trade_date = p_first_date; 
					/* 计算个股指数, 基准指数为 100 */
					set p_stock_index = p_yesterday_stock_index  + p_yesterday_stock_index * p_change_rate / 100 ;
					set p_close_corrected = p_yesterday_close_corrected + p_yesterday_close_corrected * p_change_rate / 100;
					if p_curculate_rate is null THEN
						set p_curculate_rate = p_yesterday_curculate_rate;
					end if;
				else 
					set p_stock_index = 100; 
					set p_close_corrected = p_close;
				end if;
				set p_isFirst = 0;
			else
				/* 计算个股指数, 基准指数为 100 */
				set p_stock_index = p_yesterday_stock_index  + p_yesterday_stock_index * p_change_rate / 100 ;
				set p_close_corrected = p_yesterday_close_corrected + p_yesterday_close_corrected * p_change_rate / 100;
				if p_curculate_rate is null THEN
						set p_curculate_rate = p_yesterday_curculate_rate;
					end if;
			end if;

						
			select count(*) into isExist from market_value_daily_simple where code = p_nCode and trade_date = p_odt_cur;
			if isExist = 0 THEN
				insert into market_value_daily_simple (code, trade_date, market_index, market_index_change_rate, close , stock_index, change_rate, 
					close_corrected, change_reason_desc, curculate_rate )
					select p_nCode, p_odt_cur, p_market_index, p_market_index_change_rate, p_close, p_stock_index, p_change_rate, 
					p_close_corrected, p_change_reason_desc, p_curculate_rate from dual ; 
			ELSE
				update market_value_daily_simple set market_index_change_rate = p_market_index_change_rate, 
					market_index = p_market_index,
					close = p_close, 
					stock_index = p_stock_index, 
					change_rate = p_change_rate,
					close_corrected = p_close_corrected,
					change_reason_desc = p_change_reason_desc,
					curculate_rate = p_curculate_rate 
					where code = p_nCode and trade_date = p_odt_cur;
			end if;
	
			set p_yesterday_stock_index = p_stock_index;
			set p_yesterday_close_corrected = p_close_corrected;
			set p_yesterday_curculate_rate = p_curculate_rate;
			
		end if;
		fetch next from rs into p_odt_cur, p_close, p_change_rate, p_market_index, p_market_index_change_rate , p_change_reason_desc, p_curculate_rate;
	until Done end repeat;
	close rs;

END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for CalculateMCST
-- ----------------------------
DROP PROCEDURE IF EXISTS `CalculateMCST`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `CalculateMCST`(`nCode` int,`fromDate` date)
BEGIN
	#Routine body goes here...
	DECLARE p_odt_cur date;
	
	DECLARE p_price double;
	declare p_a_total_share double;
	declare p_a_circulating_share double;
	declare p_is_bonus_share int; 
	declare p_volume double;
	declare p_turnover double;
	declare p_turnover_rate double;

	declare p_cost_price double;
	declare p_mcst_price double;
	declare p_mcst_temp_price double;
	declare p_mcst_price_change double;
	declare p_mcst_price_change_rate double;
	declare p_mcst_weight double;
	declare p_mcst_exit_right_date date;
	declare p_mcst_yesterday date;

	declare p_first_date date;
	declare p_isFirst int;
	DECLARE isExist int;

	DECLARE	Done int DEFAULT 0;
	
	declare rs CURSOR for select TradeDate, Close, Volume, Turnover, Turnover_Rate 
		from souhutradingdaily where code = nCode and TradeDate >= fromDate ORDER BY TradeDate;
	DECLARE CONTINUE HANDLER FOR NOT FOUND SET Done = 1;

	set p_isFirst = 1;

	create table if not exists MCST ( code int , trade_date date,
		exit_right_date date,  weight DECIMAL(12,2), cost_price DECIMAL(12,2),  
		mcst_price  DECIMAL(10,2) , mcst_price_change  DECIMAL(10,2),
		mcst_price_change_rate  DECIMAL(10,2), PRIMARY KEY ( code, trade_date ));

	open rs;
	fetch next from rs into p_odt_cur, p_price, p_volume, p_turnover, p_turnover_rate;

label:
	REPEAT
		if Done THEN
			leave label;
		ELSE
			Call CurrentCapitalStructure(nCode, p_odt_cur, p_a_circulating_share, p_a_total_share, p_is_bonus_share);
			#股本结构还未下载
			if p_a_total_share <=0  THEN
				leave label;
			end if;

			/*计算上一个交易日的 MACD 值 */
			if p_isFirst =1 THEN
				select max(trade_date) into p_first_date from MCST where  code = nCode and  trade_date < p_odt_cur;
				if p_first_date is not null THEN
					select  mcst_price , exit_right_date, trade_date into p_mcst_price, p_mcst_exit_right_date, p_mcst_yesterday
					from MCST  where  code = nCode and  trade_date = p_first_date; 
				else 
					set p_mcst_price =  p_price;
					set p_mcst_exit_right_date = p_odt_cur;
					set p_mcst_yesterday = p_odt_cur;
				end if;
				set p_isFirst = 0;
			end if;

			/* 如果为除权除息日，则调整上一个交易日的 mcst 价格 */
			set p_mcst_temp_price = YesterdayClosePriceAfterExitRight(nCode, p_odt_cur, p_mcst_price, p_mcst_yesterday);
			if p_mcst_temp_price <> p_mcst_price THEN
				set p_mcst_exit_right_date = p_odt_cur;
				set p_mcst_price = p_mcst_temp_price;
			end if;

			/* 根据成交量和流通股计算权重 */
			set p_mcst_weight = p_volume * 100 / (p_a_circulating_share * 10000);
			
			/*根据成交金额 p_turnover 和成交量 p_volume 计算当日的每股成本价*/
			set p_cost_price = p_turnover*10000/(p_volume *100);
			set p_mcst_price = p_mcst_weight* p_cost_price + (1-p_mcst_weight)*p_mcst_price;
			
			/* 计算相对于上一个交易日，每股成本价的涨跌幅 */
			set p_mcst_price_change = p_mcst_price - p_mcst_temp_price;
			set p_mcst_price_change_rate = p_mcst_price_change * 100 / p_mcst_temp_price; 
						
			select count(*) into isExist from MCST where code = nCode and trade_date = p_odt_cur;
			if isExist = 0 THEN
				insert into MCST (code, trade_date, exit_right_date, weight, cost_price, mcst_price,
					mcst_price_change, mcst_price_change_rate )
					select nCode, p_odt_cur,  p_mcst_exit_right_date, p_mcst_weight*100, 
					 p_cost_price,  p_mcst_price, p_mcst_price_change, p_mcst_price_change_rate from dual ; 
			ELSE
				update MCST set  exit_right_date = p_mcst_exit_right_date, cost_price = p_cost_price,
					weight = p_mcst_weight*100, mcst_price = p_mcst_price ,mcst_price_change = p_mcst_price_change,
					mcst_price_change_rate = p_mcst_price_change_rate where code = nCode and trade_date = p_odt_cur;
			end if;

			set p_mcst_yesterday = p_odt_cur;
						
		end if;
		fetch next from rs into p_odt_cur, p_price, p_volume, p_turnover, p_turnover_rate;
	until Done end repeat;
	close rs;

END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for CalculatePe
-- ----------------------------
DROP PROCEDURE IF EXISTS `CalculatePe`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `CalculatePe`(`nCode` int,`fromDate` date)
BEGIN
	#Routine body goes here...
	DECLARE odt_cur date;
	DECLARE price double;
	DECLARE	Done int DEFAULT 0;
	
	DECLARE A_Total double;
	DECLARE A_Total_Value double;
	DECLARE net_profit double;
	declare gross_profit double;
	declare gross_profit_forecast double;			# 保存预估的当年毛利
	declare gross_profit_last_middle double;				
	DECLARE gross_profit_last double;					# 去年底的毛利
	declare gross_profit_ratio_last double;
	declare gross_profit_forecast_deviation double;	# 估计的当年毛利的偏差率
	Declare baseDate varchar(32);
	declare nextDate varchar(32);
	declare isLast int;
	declare nowDate date;
	declare maxReportYear int;
	declare curMonth int;
	declare curYear int;
	declare gross_profit_curent_year double;
	declare pe 		double;
	declare pe_gross double;
	declare rs CURSOR for select TradeDate, Close  from souhutradingdaily 
		where code = nCode and TradeDate >= fromDate ORDER BY TradeDate;

	DECLARE CONTINUE HANDLER FOR NOT FOUND SET Done = 1;

	set nowDate = now();
	set net_profit = -1;
	set curYear = -1;
	set baseDate = '';
	set nextDate = '';

	create table if not exists PeDaily ( code int , trade_date date, base_date date, 
		net_profit DECIMAL(15,2), gross_profit DECIMAL(15,2),  pe DECIMAL(10,2), pe_gross DECIMAL(10,2),
		total_shares DECIMAL(12,0), total_value DECIMAL(15,2), gross_profit_forecast DECIMAL(15,2),
		deviation DECIMAL(10,2), PRIMARY KEY ( code, trade_date ));

	# 计算有年报的最后年份
	select max(report_date) from incomestatement where code = nCode into  odt_cur;
	set maxReportYear = year(odt_cur);
	if month(odt_cur)<12 then 
		set maxReportYear = maxReportYear - 1;
	end if;

	open rs;
	fetch next from rs into odt_cur, price;
label:
	REPEAT
		if Done THEN
			leave label;
		ELSE
			set A_Total = CurrentATotalShares(nCode, odt_cur);
			#股本结构还未下载
			if A_Total <=0 THEN
				Leave label;
			end if;

			#计算总市值
			set A_Total_Value = A_Total * price;
			
			#计算 PE
			if nextDate <>'' and odt_cur < nextDate THEN
				#本次日期不超过 一个季度，则使用上次获取的利润
				set net_profit = net_profit;
				set gross_profit = gross_profit;
				set gross_profit_ratio_last = gross_profit_ratio_last;
			ELSE
				call NetProfitBelongingToParentCompany(nCode, odt_cur, baseDate, isLast, net_profit);
				call GrossProfit2(nCode, odt_cur, gross_profit, baseDate, nextDate);
				# 根据去年各个季度的毛利比率来 预测今年底的毛利
				call GrossProfit(nCode, DATE_SUB(baseDate,INTERVAL 1 YEAR), gross_profit_last_middle);
				call GrossProfit(nCode, concat(year(baseDate)-1,'-12-',31), gross_profit_last);
				set  gross_profit_ratio_last = gross_profit_last/gross_profit_last_middle;				
			end if;

			if curYear <> year(odt_cur) THEN
				# 获取今年的实际毛利
				call GrossProfit(nCode, concat(year(odt_cur),'-12-',31), gross_profit_curent_year);
				set curYear = year(odt_cur);
			end if;
	
			#财务报表已经下载，可以计算PE
			if baseDate<>'' and gross_profit >0 THEN
				#估算本年的利润
				set curMonth = MONTH(baseDate);
				if curMonth =3 THEN
					set pe = 4 * net_profit;
					set gross_profit_forecast = gross_profit_ratio_last * gross_profit;
				end if;
				if curMonth =6 THEN
					set pe = 2 * net_profit;
					set gross_profit_forecast = gross_profit_ratio_last * gross_profit;
				end if;
				if curMonth =9 THEN
					set pe = 4 * net_profit / 3;
					set gross_profit_forecast = gross_profit_ratio_last * gross_profit;
				end if;
				if curMonth =12 THEN
					# 以去年的财务数据作为今年的预测
					set pe = net_profit;
					set gross_profit_forecast = gross_profit;
				end if;

				if year(odt_cur) <= maxReportYear THEN
						set gross_profit_forecast_deviation = gross_profit_curent_year - gross_profit_forecast;
						set gross_profit_forecast_deviation = gross_profit_forecast_deviation / gross_profit_forecast * 100;
				else 
						# 今年由于未知年度业绩，因此无法计算偏差 
						set gross_profit_forecast_deviation = NULL;
				end if;
				
				if pe > 0 THEN
					set pe = pe / (A_Total * 10000);
					set pe = price / pe;
					if pe > 500 THEN
						set pe = 500;
					end if;
				ELSE
					set pe = 500; 
				end if;

				if gross_profit_forecast > 0 THEN
					set pe_gross = gross_profit_forecast / (A_Total * 10000);
					set pe_gross = price / pe_gross;
					if pe_gross > 500 THEN
						set pe_gross = 500;
					end if;
				ELSE
					set pe_gross = 500; 
				end if;
				
				insert into PeDaily(code,trade_date, base_date,pe,pe_gross,net_profit,gross_profit,
					total_shares,total_value,gross_profit_forecast, deviation) 
				select nCode, odt_cur, baseDate, pe, pe_gross, net_profit, gross_profit, 
					A_Total, A_Total_Value, gross_profit_forecast, gross_profit_forecast_deviation
				from dual where not exists (select 1 from pedaily where code = nCode and trade_date = odt_cur );
			end if;
		end if;
		fetch next from rs into odt_cur, price;
	until Done end repeat;
	close rs;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for CalculateShanghaiCapitalStruct
-- ----------------------------
DROP PROCEDURE IF EXISTS `CalculateShanghaiCapitalStruct`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `CalculateShanghaiCapitalStruct`()
    MODIFIES SQL DATA
BEGIN
	#Routine body goes here...
	DECLARE Done INT DEFAULT 0;

	declare cur_code int;
	DECLARE	cur_real_date date;
	declare cur_a_total double;
	declare cur_a_circulating double;
	declare cur_change_reason varchar(64);
	declare cur_abbrev varchar(32);
	declare cur_close double;
	declare cur_change_amount double;
	DECLARE cur_prev_a_total double;
	declare cur_prev_a_circulating double;
	declare cur_prev_close double;

	declare isExist int DEFAULT 0;
	declare isSave int default 0;
	declare fromDate date;

	declare rs CURSOR for select code, real_date, A_total_shares, AShares, change_reason_desc , 
		(select abbreviation from stockinfo where code = e.code ) as abbreviation,
		CurrentClosePrice(code, real_date) as close,
		CurrentChangeAmount(code, real_date) as change_amount,
		CurrentATotalShares(code, DATE_SUB(real_date,INTERVAL 1 day)) as prev_A_total,
		CurrentACirculatingShares(code, DATE_SUB(real_date,INTERVAL 1 day)) as prev_A_circulating,
		CurrentClosePrice(code, DATE_SUB(real_date,INTERVAL 1 day)) as prev_close
		from capitalstructure e  where code >= 600000 
		and change_reason_desc<>'发行前股本' and real_date >= fromDate 
		and exists (select 1 from stockinfo a where a.code = e.code and real_date >=a_time_to_market )
		ORDER BY real_date;

	/* 异常处理 */
	DECLARE CONTINUE HANDLER FOR NOT FOUND SET Done = 1;

	create table if not exists Shanghai_CapitalStructure ( code int , real_date date, abbreviation varchar(32),
		total_shares Decimal(10,2), circulating_shares Decimal(10,2), change_reason_desc varchar(64), 
		prev_total Decimal(10,2), prev_circulating DECIMAL(10,2), prev_close Decimal(10,2), 
		prev_exit_right_price Decimal(10,2), weight DECIMAL(10,2), PRIMARY key( code, real_date ));

	select max(real_date) into fromDate from Shanghai_CapitalStructure;
	
	if fromDate is NULL THEN
		set fromDate = '1980-1-1'; #'1990-12-19';
	else 
		set fromDate = DATE_SUB(fromDate,INTERVAL 15 DAY);
	end if;	

	/* 打开游标 */
	set Done = 0;
	open rs;
	fetch next from rs into cur_code, cur_real_date, cur_a_total, cur_a_circulating,
		cur_change_reason, cur_abbrev, cur_close, cur_change_amount, cur_prev_a_total, 
		cur_prev_a_circulating, cur_prev_close;
label:
	REPEAT
		if Done THEN
			leave label;
		ELSE
			set isSave = 1;

			if POSITION('定期报告' IN cur_change_reason)>0 then 
				if cur_a_total = cur_prev_a_total and cur_a_circulating = cur_prev_a_circulating THEN
					set isSave = 0;
				end if;
			end if;

			if POSITION('转增股本' IN cur_change_reason)>0 then 
				if cur_a_total = cur_prev_a_total  THEN
					set isSave = 0;
				ELSE
					# 修改流通股
					if cur_a_circulating = cur_prev_a_total then
						set cur_a_circulating = cur_a_total/cur_prev_a_total * cur_a_circulating;
					end if;
				end if;
			end if;

			if isSave = 1 then
				select count(*) into isExist from Shanghai_CapitalStructure 
					where code = cur_code and real_date = cur_real_date;
				if isExist = 0 THEN
					insert into Shanghai_CapitalStructure (code, real_date, abbreviation, total_shares, circulating_shares,
						change_reason_desc, prev_total, prev_circulating, prev_close, prev_exit_right_price, weight )
						select cur_code, cur_real_date, cur_abbrev, cur_a_total, cur_a_circulating, cur_change_reason , 
						cur_prev_a_total, cur_prev_a_circulating, cur_prev_close, 
						cur_close-cur_change_amount , CalculateShanghaiWeight(cur_a_total, cur_a_circulating) from dual ; 
				ELSE
					update Shanghai_CapitalStructure set code = cur_code, 
						real_date = cur_real_date,
						abbreviation = cur_abbrev, 
						total_shares = cur_a_total, 
						circulating_shares = cur_a_circulating,
						change_reason_desc = cur_change_reason,
						prev_total = cur_prev_a_total,
						prev_circulating = cur_prev_a_circulating,
						prev_close = cur_prev_close,
						prev_exit_right_price = cur_close-cur_change_amount,
						weight = CalculateShanghaiWeight(cur_a_total, cur_a_circulating)
						where code = cur_code and real_date = cur_real_date;
				end if;
			end if;
		end if;
		fetch next from rs into cur_code, cur_real_date, cur_a_total, cur_a_circulating,
			cur_change_reason, cur_abbrev, cur_close, cur_change_amount, cur_prev_a_total, 
			cur_prev_A_circulating, cur_prev_close;
	until Done 
	end repeat;

	close rs;

END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for CalculateShanghaiIndex
-- ----------------------------
DROP PROCEDURE IF EXISTS `CalculateShanghaiIndex`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `CalculateShanghaiIndex`()
BEGIN
	#Routine body goes here...
	DECLARE Done INT DEFAULT 0;

	DECLARE	cur_trade_date date;
	declare p_circulating_value double DEFAULT 0;
	DECLARE p_circulating_value_yesterday double DEFAULT 0;
	declare p_repair_circulating_value double;
	declare p_division double DEFAULT 0;
	declare p_szindex double;
	declare p_self_index double;
	declare isExist int DEFAULT 0 ;

	declare p_fromDate date;

	declare  rs cursor for select tradedate from sohuindexdaily 
		where code = 1 and tradedate >= p_fromDate  order by TradeDate ;

	/* 异常处理 */
	DECLARE CONTINUE HANDLER FOR NOT FOUND SET Done = 1;

	create table if not exists Shanghai_Index ( code int, abbreviation VARCHAR(32), tradedate date, 
		szindex DECIMAL(8,2), circulating_value Decimal(15,2), repair_circulating_value Decimal(12,2), 
		division DECIMAL(8, 2), self_index DECIMAL(8,2), change_rate DECIMAL(8,2), PRIMARY key( code, tradedate ));

	call add_ind('Shanghai_Index', 'code', 'idx_code');

	/* 先确定起始日期 */
	select max(tradedate) into p_fromDate from  Shanghai_Index  where  code = 1;
	if p_fromDate is NULL then 
		set p_fromDate = '2012-8-30';
	ELSE
		set p_fromDate = DATE_SUB(p_fromDate,INTERVAL 15 day);
		if p_fromDate < '2012-8-30' THEN
			set p_fromDate = '2012-8-30';
		end if;
	end if;

	/* 先计算所有股票靠档后的流通市值，以及修正市值；再计算每天的 新除数 ；最后计算个股指数 */
	set Done = 0;
	open rs;
	fetch next from rs into cur_trade_date;
label:
	REPEAT
		if Done THEN
			leave label;
		ELSE
			call CalculateShanghaiTotalCirculatingDaily(cur_trade_date, p_circulating_value);
			call CalculateShanghaiRepairCirculatingDaily(cur_trade_date, p_repair_circulating_value);
			/* 获取上证指数 */
			select close into  p_szindex from sohuindexdaily where code = 1 and tradedate = cur_trade_date;
			
			if p_division = 0 THEN
				if cur_trade_date = '2012-8-30' then
					/* 确定开市第一天的 除数 */
					set p_division = 1190058895.96851 /2047.52;
				ELSE
					select circulating_value, division into p_circulating_value_yesterday, p_division 
					from Shanghai_Index where tradedate < cur_trade_date and code = 1 ORDER BY tradedate desc limit 1;
				end if;
			end if;
			if p_repair_circulating_value > 0 and cur_trade_date <> '2012-8-30' THEN
				/* 当日有特定事件（新股、配股、转增）等需要修正除数 */
				/* 通过昨天收盘的总靠档流通市值，调整今天的新除数 */
				set p_division =  p_division/ p_circulating_value_yesterday ;
				set p_division =  p_division * (p_circulating_value_yesterday  + p_repair_circulating_value);
			end if;

			/* 计算当日 指数 */
			set p_self_index = p_circulating_value/ p_division;

			select count(*) into isExist from Shanghai_Index where tradedate = cur_trade_date and code = 1;
			if isExist = 0 then 
				insert into Shanghai_Index (code, abbreviation, tradedate, szindex, circulating_value, 
					repair_circulating_value, division, self_index )
				select 1, '上证指数', cur_trade_date, p_szindex, p_circulating_value, 
					p_repair_circulating_value, p_division, p_self_index ;
			ELSE
				update Shanghai_Index set
				szindex = p_szindex,
				circulating_value = p_circulating_value,
				repair_circulating_value = p_repair_circulating_value,
				division = p_division,
				self_index = p_self_index
				where tradedate = cur_trade_date and code = 1;
			end if;

			/* 计算当日的所有股票的 个股指数 */
			call CalculateShanghaiStockIndexDaily(cur_trade_date, p_division);
		end if;
		set p_circulating_value_yesterday = p_circulating_value;
		fetch next from rs into cur_trade_date;
	until Done 
	end repeat;

	close rs;

END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for CalculateShanghaiPlateIndex
-- ----------------------------
DROP PROCEDURE IF EXISTS `CalculateShanghaiPlateIndex`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `CalculateShanghaiPlateIndex`()
BEGIN
	#Routine body goes here...
	DECLARE Done INT DEFAULT 0;

	DECLARE	cur_trade_date date;
	declare p_circulating_value double DEFAULT 0;
	DECLARE p_circulating_value_yesterday double DEFAULT 0;
	declare p_repair_circulating_value double;
	declare p_division double DEFAULT 0;
	declare p_szindex double;
	declare p_self_index double;
	declare isExist int DEFAULT 0 ;

	declare p_fromDate date;

	declare  rs cursor for select tradedate from sohuindexdaily 
		where code = 1 and tradedate >= p_fromDate  order by TradeDate ;

	/* 异常处理 */
	DECLARE CONTINUE HANDLER FOR NOT FOUND SET Done = 1;

	create table if not exists Shanghai_Plate_Index ( code int, name VARCHAR(64), 
		parent_code int, tradedate date,  self_index DECIMAL(8,2),
		change_rate DECIMAL(8,2), PRIMARY key( code, tradedate ));

	call add_ind('Shanghai_Plate_Index', 'code', 'idx_code');

	/* 先确定起始日期 */
	select max(tradedate) into p_fromDate from  Shanghai_Plate_Index ;
	if p_fromDate is NULL then 
		set p_fromDate = '2012-8-30';
	ELSE
		set p_fromDate = DATE_SUB(p_fromDate,INTERVAL 15 day);
		if p_fromDate < '2012-8-30' THEN
			set p_fromDate = '2012-8-30';
		end if;
	end if;

	/*  */
	set Done = 0;
	open rs;
	fetch next from rs into cur_trade_date;
label:
	REPEAT
		if Done THEN
			leave label;
		ELSE
			
			/* 计算当日的所有板块的 指数 */
			call CalculateShanghaiPlateIndexDaily(cur_trade_date);
		end if;
		fetch next from rs into cur_trade_date;
	until Done 
	end repeat;

	close rs;

END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for CalculateShanghaiPlateIndexDaily
-- ----------------------------
DROP PROCEDURE IF EXISTS `CalculateShanghaiPlateIndexDaily`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `CalculateShanghaiPlateIndexDaily`(IN `curDate` date)
    MODIFIES SQL DATA
BEGIN
	#Routine body goes here...
	DECLARE Done INT DEFAULT 0;

	declare cur_code int;
	declare cur_plate_code int;
	declare cur_plate_name varchar(64);
	declare cur_prarent_code int;
	
	declare totalIndex double DEFAULT 0;
	declare yesterdayIndex double;

	declare isExist int DEFAULT 0;
	
	# 获取直至当日已经上市的股市代码
	declare rs CURSOR for select code, name, Parent_Code 
		from souhustockplates where Parent_Code >0 and Is_Stock = 0;

	/* 异常处理 */
	DECLARE CONTINUE HANDLER FOR NOT FOUND SET Done = 1;

	/* 打开游标 */
	open rs;
	fetch next from rs into cur_plate_code, cur_plate_name , cur_prarent_code;
label:
	REPEAT
		if Done THEN
			leave label;
		ELSE
			set totalIndex = CurrentShanghaiPlateIndex(curDate, cur_plate_code);
			/*if cur_close is NULL then 
				select cur_code, curDate from dual;
			end if;*/

			/* 保存当日的 板块指数 */
			select count(*) into isExist from Shanghai_Plate_Index where tradedate = curDate and code = cur_plate_code;
			if isExist = 0 then 
				insert into Shanghai_Plate_Index (code, name, parent_code, tradedate, self_index)
				select cur_plate_code, cur_plate_name, cur_prarent_code, curDate, totalIndex ;
			ELSE
				update Shanghai_Plate_Index set self_index = totalIndex
				where tradedate = curDate and code = cur_plate_code;
			end if;

			/* 计算板块指数 的变化率 */
			select count(*) into isExist from Shanghai_Plate_Index where tradedate < curDate and code = cur_plate_code;
			if isExist > 0 THEN
				select self_index into yesterdayIndex from Shanghai_Plate_Index 
					where tradedate < curDate and code = cur_plate_code ORDER BY tradedate desc limit 1;

				set yesterdayIndex = (totalIndex - yesterdayIndex)/yesterdayIndex * 100;
				update Shanghai_Plate_Index set change_rate = yesterdayIndex
					where tradedate = curDate and code = cur_plate_code;
			end if;

		end if;
		fetch next from rs into cur_plate_code, cur_plate_name, cur_prarent_code;
	until Done 
	end repeat;

	close rs;
	#return circulating_value;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for CalculateShanghaiRepairCirculatingDaily
-- ----------------------------
DROP PROCEDURE IF EXISTS `CalculateShanghaiRepairCirculatingDaily`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `CalculateShanghaiRepairCirculatingDaily`(IN `curDate` date, OUT `totalRepair` double)
    MODIFIES SQL DATA
BEGIN
	#Routine body goes here...
	DECLARE Done INT DEFAULT 0;

	declare cur_code int;
	declare cur_abbreviation varchar(32);
	DECLARE	cur_real_date date;

	declare cur_prev_total double;
	declare cur_prev_circulating double;
	declare cur_prev_close double;
	declare cur_weight double;
	declare cur_prev_exit_right_price double;
	declare cur_total_shares double;

	declare circulating_value double;
	declare tmp double;
	declare isExist int DEFAULT 0 ;

	# 获取直至当日已经上市的股市代码
	declare rs CURSOR for select code, abbreviation, prev_total, prev_circulating, prev_close, 
		weight, prev_exit_right_price, total_shares
		from Shanghai_CapitalStructure where  real_date = curDate ORDER BY code ;

	/* 异常处理 */
	DECLARE CONTINUE HANDLER FOR NOT FOUND SET Done = 1;

	select count(*) into isExist from Shanghai_CapitalStructure where  real_date = curDate;
	/* 计算当日总流通市值，做了考档处理 */
	set circulating_value = 0;

	if isExist > 0 then 
		/* 打开游标 */
		open rs;
		fetch next from rs into cur_code, cur_abbreviation, cur_prev_total, cur_prev_circulating, cur_prev_close, 
			cur_weight, cur_prev_exit_right_price, cur_total_shares;
label:
		REPEAT
			if Done THEN
				leave label;
			ELSE
				set tmp = CurrentShanghaiStockRepairCirculating(curDate, cur_code);
				set circulating_value = circulating_value + tmp;

				select count(*) into isExist from Shanghai_Index where tradedate = curDate and code = cur_code;
				if isExist = 0 then 
					insert into Shanghai_Index (code, abbreviation, tradedate, repair_circulating_value)
					select cur_code, cur_abbreviation, curDate, tmp ;
				ELSE
					update Shanghai_Index set repair_circulating_value = tmp
					where tradedate = curDate and code = cur_code;
				end if;
					
			end if;
			fetch next from rs into cur_code, cur_abbreviation, cur_prev_total, cur_prev_circulating, cur_prev_close, 
				cur_weight, cur_prev_exit_right_price, cur_total_shares;
		until Done 
		end repeat;

		close rs;
	end if;

	set totalRepair = circulating_value;
	#return circulating_value;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for CalculateShanghaiStockIndexDaily
-- ----------------------------
DROP PROCEDURE IF EXISTS `CalculateShanghaiStockIndexDaily`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `CalculateShanghaiStockIndexDaily`(IN `curDate` date, IN `division` double)
    MODIFIES SQL DATA
BEGIN
	#计算个股指数 
	DECLARE Done INT DEFAULT 0;

	declare cur_code int;
	DECLARE	cur_real_date date;
	declare p_yesterday_index double;
	declare p_today_index double;
	declare isExist int;

	# 获取直至当日已经上市的股市代码
	declare rs CURSOR for select code from Shanghai_CapitalStructure where change_reason_desc like '%A股上市%' 
		and real_date <= curDate;

	/* 异常处理 */
	DECLARE CONTINUE HANDLER FOR NOT FOUND SET Done = 1;

	/* 打开游标 */
	open rs;
	fetch next from rs into cur_code;
label:
	REPEAT
		if Done THEN
			leave label;
		ELSE
			update Shanghai_Index set self_index = circulating_value/division
				where tradedate = curDate and code = cur_code;

			/* 计算个股指数的变化率*/
			select self_index into p_today_index from shanghai_index where tradedate = curDate and code = cur_code;
			
			select count(*) into isExist from shanghai_index 
				where code = cur_code and tradedate < curDate ORDER BY tradedate desc limit 1;
			if isExist > 0 then
				select self_index into p_yesterday_index from shanghai_index 
					where code = cur_code and tradedate < curDate ORDER BY tradedate desc limit 1;

				update shanghai_index set change_rate = (p_today_index - p_yesterday_index )/p_yesterday_index * 100
					where code = cur_code and tradedate = curDate;
			end if;
						
		end if;
		fetch next from rs into cur_code;
	until Done 
	end repeat;

	close rs;

	#return circulating_value;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for CalculateShanghaiTotalCirculatingDaily
-- ----------------------------
DROP PROCEDURE IF EXISTS `CalculateShanghaiTotalCirculatingDaily`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `CalculateShanghaiTotalCirculatingDaily`(IN `curDate` date, OUT `totalCirculating` double)
    MODIFIES SQL DATA
BEGIN
	#Routine body goes here...
	DECLARE Done INT DEFAULT 0;

	declare cur_code int;
	declare cur_abbreviation varchar(32);
	DECLARE	cur_real_date date;

	declare cur_close double;

	declare circulating_value double;
	declare isExist int DEFAULT 0;
	
	# 获取直至当日已经上市的股市代码
	declare rs CURSOR for select code, abbreviation from Shanghai_CapitalStructure 
		where change_reason_desc like '%A股上市%' and real_date <= curDate order by code;

	/* 异常处理 */
	DECLARE CONTINUE HANDLER FOR NOT FOUND SET Done = 1;

	/* 计算当日总流通市值，做了考档处理 */
	set circulating_value = 0;
	/* 打开游标 */
	open rs;
	fetch next from rs into cur_code, cur_abbreviation;
label:
	REPEAT
		if Done THEN
			leave label;
		ELSE
			set cur_close = CurrentShanghaiStockCirculatingValue(curDate, cur_code);
			/*if cur_close is NULL then 
				select cur_code, curDate from dual;
			end if;*/

			select count(*) into isExist from Shanghai_Index where tradedate = curDate and code = cur_code;
			if isExist = 0 then 
				insert into Shanghai_Index (code, abbreviation, tradedate, circulating_value)
				select cur_code, cur_abbreviation, curDate, cur_close ;
			ELSE
				update Shanghai_Index set circulating_value = cur_close
				where tradedate = curDate and code = cur_code;
			end if;

			set circulating_value = circulating_value +cur_close;
		end if;
		fetch next from rs into cur_code, cur_abbreviation;
	until Done 
	end repeat;

	close rs;

	set totalCirculating = circulating_value;
	#return circulating_value;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for CalculateShenzhenIndex
-- ----------------------------
DROP PROCEDURE IF EXISTS `CalculateShenzhenIndex`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `CalculateShenzhenIndex`()
    MODIFIES SQL DATA
BEGIN
	#Routine body goes here...
	DECLARE Done INT DEFAULT 0;

	DECLARE	cur_trade_date date;
	declare p_circulating_value double DEFAULT 0;
	DECLARE p_circulating_value_yesterday double DEFAULT 0;
	declare p_repair_circulating_value double;
	declare p_division double DEFAULT 0;
	declare p_szindex double;
	declare p_self_index double;
	declare isExist int DEFAULT 0 ;

	declare p_fromDate date;

	declare  rs cursor for select tradedate from sohuindexdaily 
		where code = 1 and tradedate >= p_fromDate  order by TradeDate ;

	/* 异常处理 */
	DECLARE CONTINUE HANDLER FOR NOT FOUND SET Done = 1;

	create table if not exists Shenzhen_Index ( code int, abbreviation VARCHAR(32), tradedate date, 
		szindex DECIMAL(8,2), circulating_value Decimal(15,2), repair_circulating_value Decimal(12,2), 
		division DECIMAL(8, 2), self_index DECIMAL(8,2), change_rate DECIMAL(8,2), PRIMARY key( code, tradedate ));

	call add_ind('Shenzhen_Index', 'code', 'idx_code');

	/* 先确定起始日期 */
	select max(tradedate) into p_fromDate from  Shenzhen_Index  where  code = 1;
	if p_fromDate is NULL then 
		set p_fromDate = '2014-01-10';
	ELSE
		set p_fromDate = DATE_SUB(p_fromDate,INTERVAL 15 day);
		if p_fromDate < '2014-01-10' THEN
			set p_fromDate = '2014-01-10';
		end if;
	end if;

	/* 先计算所有股票流通市值，以及修正市值；再计算每天的 新除数 ；最后计算个股指数 */
	set Done = 0;
	open rs;
	fetch next from rs into cur_trade_date;
label:
	REPEAT
		if Done THEN
			leave label;
		ELSE
			call CalculateShenzhenTotalCirculatingDaily(cur_trade_date, p_circulating_value);
			call CalculateShenzhenRepairCirculatingDaily(cur_trade_date, p_repair_circulating_value);
			/* 获取上证指数 */
			select close into  p_szindex from sohuindexdaily where code = 1 and tradedate = cur_trade_date;
			
			if p_division = 0 THEN
				if cur_trade_date = '2014-01-10' then
					/* 确定开市第一天的 除数 */
					set p_division = 1190058895.96851 /2047.52;
				ELSE
					select circulating_value, division into p_circulating_value_yesterday, p_division 
					from Shenzhen_Index where tradedate < cur_trade_date and code = 1 ORDER BY tradedate desc limit 1;
				end if;
			end if;
			if p_repair_circulating_value > 0 and cur_trade_date <> '2014-01-10' THEN
				/* 当日有特定事件（新股、配股、转增）等需要修正除数 */
				/* 通过昨天收盘的总靠档流通市值，调整今天的新除数 */
				set p_division =  p_division/ p_circulating_value_yesterday ;
				set p_division =  p_division * (p_circulating_value_yesterday  + p_repair_circulating_value);
			end if;

			/* 计算当日 指数 */
			set p_self_index = p_circulating_value/ p_division;

			select count(*) into isExist from Shenzhen_Index where tradedate = cur_trade_date and code = 1;
			if isExist = 0 then 
				insert into Shenzhen_Index (code, abbreviation, tradedate, szindex, circulating_value, 
					repair_circulating_value, division, self_index )
				select 1, '深证新指数', cur_trade_date, p_szindex, p_circulating_value, 
					p_repair_circulating_value, p_division, p_self_index ;
			ELSE
				update Shenzhen_Index set
				szindex = p_szindex,
				circulating_value = p_circulating_value,
				repair_circulating_value = p_repair_circulating_value,
				division = p_division,
				self_index = p_self_index
				where tradedate = cur_trade_date and code = 1;
			end if;

			/* 计算当日的所有股票的 个股指数 */
			call CalculateShenzhenStockIndexDaily(cur_trade_date, p_division);
		end if;
		set p_circulating_value_yesterday = p_circulating_value;
		fetch next from rs into cur_trade_date;
	until Done 
	end repeat;

	close rs;

END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for CalculateShenzhenRepairCirculatingDaily
-- ----------------------------
DROP PROCEDURE IF EXISTS `CalculateShenzhenRepairCirculatingDaily`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `CalculateShenzhenRepairCirculatingDaily`(IN `curDate` date, OUT `totalRepair` double)
    MODIFIES SQL DATA
BEGIN
	#Routine body goes here...
	DECLARE Done INT DEFAULT 0;

	declare cur_code int;
	declare cur_abbreviation varchar(32);
	DECLARE	cur_real_date date;

	declare cur_prev_total double;
	declare cur_prev_circulating double;
	declare cur_prev_close double;
	declare cur_weight double;
	declare cur_prev_exit_right_price double;
	declare cur_total_shares double;

	declare circulating_value double;
	declare tmp double;
	declare isExist int DEFAULT 0 ;

	# 获取直至当日已经上市的股市代码
	declare rs CURSOR for select code, abbreviation, prev_total, prev_circulating, prev_close, 
		weight, prev_exit_right_price, total_shares
		from Shenzhen_CapitalStructure where  real_date = curDate ORDER BY code ;

	/* 异常处理 */
	DECLARE CONTINUE HANDLER FOR NOT FOUND SET Done = 1;

	select count(*) into isExist from Shenzhen_CapitalStructure where  real_date = curDate;
	/* 计算当日总流通市值，做了考档处理 */
	set circulating_value = 0;

	if isExist > 0 then 
		/* 打开游标 */
		open rs;
		fetch next from rs into cur_code, cur_abbreviation, cur_prev_total, cur_prev_circulating, cur_prev_close, 
			cur_weight, cur_prev_exit_right_price, cur_total_shares;
label:
		REPEAT
			if Done THEN
				leave label;
			ELSE
				set tmp = CurrentShenzhenStockRepairCirculating(curDate, cur_code);
				set circulating_value = circulating_value + tmp;

				select count(*) into isExist from Shenzhen_Index where tradedate = curDate and code = cur_code;
				if isExist = 0 then 
					insert into Shenzhen_Index (code, abbreviation, tradedate, repair_circulating_value)
					select cur_code, cur_abbreviation, curDate, tmp ;
				ELSE
					update Shenzhen_Index set repair_circulating_value = tmp
					where tradedate = curDate and code = cur_code;
				end if;
					
			end if;
			fetch next from rs into cur_code, cur_abbreviation, cur_prev_total, cur_prev_circulating, cur_prev_close, 
				cur_weight, cur_prev_exit_right_price, cur_total_shares;
		until Done 
		end repeat;

		close rs;
	end if;

	set totalRepair = circulating_value;
	#return circulating_value;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for CalculateShenzhenStockIndexDaily
-- ----------------------------
DROP PROCEDURE IF EXISTS `CalculateShenzhenStockIndexDaily`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `CalculateShenzhenStockIndexDaily`(IN `curDate` date, IN `division` double)
    MODIFIES SQL DATA
BEGIN
	#计算个股指数 
	DECLARE Done INT DEFAULT 0;

	declare cur_code int;
	DECLARE	cur_real_date date;
	declare p_yesterday_index double;
	declare p_today_index double;
	declare isExist int;

	# 获取直至当日已经上市的股市代码
	declare rs CURSOR for select code from Shenzhen_CapitalStructure where change_reason_desc like '%A股上市%' 
		and real_date <= curDate;

	/* 异常处理 */
	DECLARE CONTINUE HANDLER FOR NOT FOUND SET Done = 1;

	/* 打开游标 */
	open rs;
	fetch next from rs into cur_code;
label:
	REPEAT
		if Done THEN
			leave label;
		ELSE
			update Shenzhen_Index set self_index = circulating_value/division
				where tradedate = curDate and code = cur_code;

			/* 计算个股指数的变化率*/
			select self_index into p_today_index from shenzhen_index where tradedate = curDate and code = cur_code;
			
			select count(*) into isExist from shenzhen_index 
				where code = cur_code and tradedate < curDate ORDER BY tradedate desc limit 1;
			if isExist > 0 then
				select self_index into p_yesterday_index from shenzhen_index 
					where code = cur_code and tradedate < curDate ORDER BY tradedate desc limit 1;

				update shenzhen_index set change_rate = (p_today_index - p_yesterday_index )/p_yesterday_index * 100
					where code = cur_code and tradedate = curDate;
			end if;
						
		end if;
		fetch next from rs into cur_code;
	until Done 
	end repeat;

	close rs;

	#return circulating_value;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for CurrentCapitalStructure
-- ----------------------------
DROP PROCEDURE IF EXISTS `CurrentCapitalStructure`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `CurrentCapitalStructure`(IN `nCode` int, IN `curDate` date, OUT `aShare` double, OUT `aTotalShare` double, OUT `isBonusShare` int)
    READS SQL DATA
BEGIN
	#Routine body goes here...
	DECLARE Done INT DEFAULT 0;
	DECLARE	p_odt_cur date;
	declare p_a_circulating double DEFAULT 0;
	declare p_a_total_shares double default 0;
	declare p_changeReasonDesc varchar(256);
	
	DECLARE rs cursor for select real_date, AShares, A_total_shares, change_reason_desc 
		from capitalstructure where code = nCode and real_date <= curDate order by real_date desc;
	/* 异常处理 */
	DECLARE CONTINUE HANDLER FOR NOT FOUND SET Done = 1;
	/* 打开游标 */
	open rs;
	fetch next from rs into p_odt_cur, p_a_circulating, p_a_total_shares, p_changeReasonDesc;
label:
	REPEAT
		if Done THEN
			leave label;
		ELSE
			if curDate >= p_odt_cur THEN
				leave label;
			end if;
		end if;
		fetch next from rs into p_odt_cur, p_a_circulating, p_a_total_shares, p_changeReasonDesc;
	until Done 
	end repeat;
	close rs;

	/*判断是否送股、转增股*/
	if POSITION('送股' IN p_changeReasonDesc)>0 or POSITION('转增' IN p_changeReasonDesc)>0 THEN
		set isBonusShare = 1;
	ELSE
		set isBonusShare = 0;
	end if;

	set aShare = p_a_circulating;
	set aTotalShare = p_a_total_shares;

END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for del_idx
-- ----------------------------
DROP PROCEDURE IF EXISTS `del_idx`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `del_idx`(IN p_tablename varchar(200), IN p_idxname VARCHAR(200))
begin  
DECLARE str VARCHAR(250);  
  set @str=concat(' drop index ',p_idxname,' on ',p_tablename);   
    
  select count(*) into @cnt from information_schema.statistics where table_name=p_tablename and index_name=p_idxname ;  
  if @cnt >0 then   
    PREPARE stmt FROM @str;  
    EXECUTE stmt ;  
  end if;   
 end
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for FinanceRptReleaseDateForAllStock
-- ----------------------------
DROP PROCEDURE IF EXISTS `FinanceRptReleaseDateForAllStock`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `FinanceRptReleaseDateForAllStock`()
BEGIN
	#Routine body goes here...
	DECLARE	Done int DEFAULT 0;
	DECLARE odt_cur date;
	declare releaseDate date;
	declare nCode int;
	DECLARE isOk int;
	
	DECLARE rs cursor for select code, report_date from incomestatement a
			where release_date is null and 
			exists (select 1 from stockinfo b where a.code = b.code and a.report_date >= b.a_time_to_market );
	/* 异常处理 */
	DECLARE CONTINUE HANDLER FOR NOT FOUND SET Done = 1;

	call add_col('incomestatement', 'release_date', 'DATE', '');
	/* 打开游标 */
	open rs;
	fetch next from rs into nCode, odt_cur;
label:
	REPEAT
		if Done THEN
			leave label;
		ELSE
			set releaseDate = FinanceRptReleaseDate(nCode, odt_cur);
			if releaseDate <> '1900-01-01' then
				update incomestatement set release_date = releaseDate 
					where code = nCode and report_date = odt_cur;
			end if;
		end if;
		fetch next from rs into nCode, odt_cur;
	until Done 
	end repeat;

	close rs;

END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for GrossProfit
-- ----------------------------
DROP PROCEDURE IF EXISTS `GrossProfit`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `GrossProfit`(`nCode` int,`curDate` date,OUT `gross_profit` double)
BEGIN
	#Routine body goes here...
	DECLARE	Done int DEFAULT 0;
	DECLARE odt_cur date;
	DECLARE odt_max date;
	DECLARE isOk int;
	DECLARE rs cursor for select report_date, (colume_2 - colume_9 - colume_20) as gross_profit 
		from incomestatement where code = nCode order by report_date desc;
	/* 异常处理 */
	DECLARE CONTINUE HANDLER FOR NOT FOUND SET Done = 1;
	
	set gross_profit = -1;
	set isOk = 0;
	/* 打开游标 */
	open rs;
	fetch next from rs into odt_cur, gross_profit;
label:
	REPEAT
		if Done THEN
			leave label;
		ELSE
			if curDate >= odt_cur THEN
				set isOk = 1;
				leave label;
			end if;
		end if;
		fetch next from rs into odt_cur, gross_profit;
	until Done 
	end repeat;
	close rs;

	/*计算本次利润基于的季度时间 */
	if isOk = 1 THEN
		set gross_profit = gross_profit;
	ELSE
		set gross_profit = -1;
	end if;

END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for GrossProfit2
-- ----------------------------
DROP PROCEDURE IF EXISTS `GrossProfit2`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `GrossProfit2`(`nCode` int,`curDate` date,OUT `gross_profit` double, OUT `baseDate` date, OUT `nextDate` date)
BEGIN
	#Routine body goes here...
	DECLARE	Done int DEFAULT 0;
	DECLARE odt_rpt date;		# 财报的季度时间
	DECLARE odt_rls date;		# 财报的发布时间
	DECLARE isOk int;
	DECLARE rs cursor for select report_date, release_date, (colume_2 - colume_9 - colume_20) as gross_profit 
		from incomestatement where code = nCode order by report_date desc;
	/* 异常处理 */
	DECLARE CONTINUE HANDLER FOR NOT FOUND SET Done = 1;
	
	set gross_profit = -1;
	set nextDate = NULL;
	set baseDate = NULL;
	set isOk = 0;
	/* 打开游标 */
	open rs;
	fetch next from rs into odt_rpt, odt_rls, gross_profit;
label:
	REPEAT
		if Done THEN
			leave label;
		ELSE
			if curDate >= odt_rls THEN
				set isOk = 1;
				leave label;
			end if;
		end if;
		set nextDate = odt_rls;
		fetch next from rs into odt_rpt ,odt_rls, gross_profit;
	until Done 
	end repeat;
	close rs;

	/*计算本次利润基于的季度时间 */
	if isOk = 1 THEN
		set gross_profit = gross_profit;
		set baseDate = odt_rpt;
		if nextDate is NULL then
			set nextDate = DATE_ADD(now(),INTERVAL 1 day);
		end if;
	ELSE
		set gross_profit = -1;
		set baseDate = '';
		set nextDate = '';
	end if;

END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for HuShen_CapitalStruct11111
-- ----------------------------
DROP PROCEDURE IF EXISTS `HuShen_CapitalStruct11111`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `HuShen_CapitalStruct11111`()
    MODIFIES SQL DATA
BEGIN
	#Routine body goes here...
	DECLARE Done INT DEFAULT 0;

	declare cur_code int;
	DECLARE	cur_real_date date;
	declare cur_a_total double;
	declare cur_a_circulating double;
	declare cur_change_reason varchar(64);
	declare cur_abbrev varchar(32);
	declare cur_close double;
	declare cur_change_amount double;
	DECLARE cur_prev_a_total double;
	declare cur_prev_a_circulating double;
	declare cur_prev_close double;

	declare isExist int DEFAULT 0;
	declare isSave int default 0;
	declare fromDate date;
	declare p_min_tradedate date;

	declare rs CURSOR for select code, real_date, A_total_shares, AShares, change_reason_desc , 
		(select abbreviation from stockinfo where code = e.code ) as abbreviation,
		CurrentClosePrice(code, real_date) as close,
		CurrentChangeAmount(code, real_date) as change_amount,
		CurrentATotalShares(code, DATE_SUB(real_date,INTERVAL 1 day)) as prev_A_total,
		CurrentACirculatingShares(code, DATE_SUB(real_date,INTERVAL 1 day)) as prev_A_circulating,
		CurrentClosePrice(code, DATE_SUB(real_date,INTERVAL 1 day)) as prev_close
		from capitalstructure e  where 1=1 
		and change_reason_desc<>'发行前股本' and real_date >= fromDate 
		and exists (select 1 from stockinfo a where a.code = e.code and real_date >=a_time_to_market )
		ORDER BY real_date;

	/* 异常处理 */
	DECLARE CONTINUE HANDLER FOR NOT FOUND SET Done = 1;

	/* begin_date 为该股票纳入指数计算的开始时间 */
	create table if not exists HuShen_CapitalStructure ( code int , real_date date, abbreviation varchar(32),
		total_shares Decimal(10,2), circulating_shares Decimal(10,2), change_reason_desc varchar(64), 
		prev_total Decimal(10,2), prev_circulating DECIMAL(10,2), prev_close Decimal(10,2), 
		prev_exit_right_price Decimal(10,2), PRIMARY key( code, real_date ));

	select max(real_date) into fromDate from HuShen_CapitalStructure;
	
	if fromDate is NULL THEN
		set fromDate = '1980-1-1'; #'1990-12-19';
	else 
		set fromDate = Date_sub(fromDate, interval 30 day);
		delete from hushen_capitalstructure where real_date>= fromDate;
	end if;	

	/* 打开游标 */
	set Done = 0;
	open rs;
	fetch next from rs into cur_code, cur_real_date, cur_a_total, cur_a_circulating,
		cur_change_reason, cur_abbrev, cur_close, cur_change_amount, cur_prev_a_total, 
		cur_prev_a_circulating, cur_prev_close;
label:
	REPEAT
		if Done THEN
			leave label;
		ELSE
			set isSave = 1;

			if POSITION('定期报告' IN cur_change_reason)>0 then 
				if cur_a_total = cur_prev_a_total and cur_a_circulating = cur_prev_a_circulating THEN
					set isSave = 0;
				end if;
			end if;

			if POSITION('转增股本' IN cur_change_reason)>0 then 
				if cur_a_total = cur_prev_a_total  THEN
					set isSave = 0;
				ELSE
					# 修改流通股
					if cur_a_circulating = cur_prev_a_total then
						set cur_a_circulating = cur_a_total/cur_prev_a_total * cur_a_circulating;
					end if;
				end if;
			end if;

			if POSITION('A股上市' IN cur_change_reason)>0 then 
				select count(*) into isExist from  souhutradingdaily  where  code = cur_code;
				if isExist > 0 then
					select min(TradeDate) into p_min_tradedate from souhutradingdaily  where  code = cur_code;
					if p_min_tradedate > cur_real_date THEN
						/* 由于数据有缺漏，补充插入一条*/
						insert into HuShen_CapitalStructure (code, real_date, abbreviation, total_shares, circulating_shares,
							change_reason_desc, prev_total, prev_circulating, prev_close, prev_exit_right_price )
							select cur_code, p_min_tradedate, cur_abbrev, 
							CurrentATotalShares(cur_code, p_min_tradedate), 
							CurrentACirculatingShares(cur_code, p_min_tradedate), 
							'纳入指数计算(交易数据遗漏)' , 
							CurrentATotalShares(cur_code, DATE_SUB(p_min_tradedate,INTERVAL 1 day)), 
							CurrentACirculatingShares(cur_code, DATE_SUB(p_min_tradedate,INTERVAL 1 day)), 
							0, CurrentClosePrice(cur_code, p_min_tradedate) ; 
					ELSE
						set p_min_tradedate = HuShen_GetTradeDate(p_min_tradedate, 11);
						if p_min_tradedate <> '0000-00-00' then
							insert into HuShen_CapitalStructure (code, real_date, abbreviation, total_shares, circulating_shares,
								change_reason_desc, prev_total, prev_circulating, prev_close, prev_exit_right_price )
								select cur_code, p_min_tradedate, cur_abbrev, 
								CurrentATotalShares(cur_code, p_min_tradedate), 
								CurrentACirculatingShares(cur_code, p_min_tradedate), 
								'纳入指数计算(推迟11个交易日)' , 
								CurrentATotalShares(cur_code, DATE_SUB(p_min_tradedate,INTERVAL 1 day)), 
								CurrentACirculatingShares(cur_code, DATE_SUB(p_min_tradedate,INTERVAL 1 day)), 
								0, CurrentClosePrice(cur_code, p_min_tradedate) ; 
						end if;
					end if;
				end if;
			end if;

			if isSave = 1 then
				select count(*) into isExist from HuShen_CapitalStructure 
					where code = cur_code and real_date = cur_real_date;
				if isExist = 0 THEN
					insert into HuShen_CapitalStructure (code, real_date, abbreviation, total_shares, circulating_shares,
						change_reason_desc, prev_total, prev_circulating, prev_close, prev_exit_right_price )
						select cur_code, cur_real_date, cur_abbrev, cur_a_total, cur_a_circulating, cur_change_reason , 
						cur_prev_a_total, cur_prev_a_circulating, cur_prev_close, cur_close-cur_change_amount  ; 
				ELSE
					update HuShen_CapitalStructure set code = cur_code, 
						real_date = cur_real_date,
						abbreviation = cur_abbrev, 
						total_shares = cur_a_total, 
						circulating_shares = cur_a_circulating,
						change_reason_desc = cur_change_reason,
						prev_total = cur_prev_a_total,
						prev_circulating = cur_prev_a_circulating,
						prev_close = cur_prev_close,
						prev_exit_right_price = cur_close-cur_change_amount
						where code = cur_code and real_date = cur_real_date;
				end if;
			end if;
		end if;
		fetch next from rs into cur_code, cur_real_date, cur_a_total, cur_a_circulating,
			cur_change_reason, cur_abbrev, cur_close, cur_change_amount, cur_prev_a_total, 
			cur_prev_A_circulating, cur_prev_close;
	until Done 
	end repeat;

	close rs;

END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for HuShen_GetStockStatus
-- ----------------------------
DROP PROCEDURE IF EXISTS `HuShen_GetStockStatus`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `HuShen_GetStockStatus`(IN `p_currentDate` date,IN `p_previousDate` date,IN `p_nCode` int,OUT `p_close` double,OUT `p_change_rate` double,OUT `p_CirculatingValue` double,OUT `p_RepairValue` double,OUT  `p_Remark` varchar(32))
    READS SQL DATA
BEGIN
	# 输入参数 p_previousDate = 前一个交易日
	DECLARE Done INT DEFAULT 0;

	declare p_tradedate date;
	declare p_prev_tradedate date;
	declare p_isExist int;
	declare p_prev_close double;
	declare p_change_amount double;
	
	declare p_real_date date;
	declare p_prev_circulating double;
	declare p_circulating double;
	declare p_prev_exit_right_price double;
		
	/* 异常处理 */
	DECLARE CONTINUE HANDLER FOR NOT FOUND SET Done = 1;

	select close, tradedate, change_rate, change_amount 
		into p_close, p_tradedate, p_change_rate, p_change_amount 
		from souhutradingdaily where code = p_nCode 
		and tradeDate<= p_currentDate order by tradeDate desc limit 1;
			
	if p_tradedate < p_currentDate THEN
		set p_remark = '停牌';
		set p_change_rate = 0;
		set p_RepairValue = 0;	# 停牌期间，转增、送股等操作等到复牌后的第一天进行计算
			
		select AShares into p_circulating 
			from capitalstructure where code = p_nCode and  real_date <= p_tradedate 
			ORDER BY real_date desc limit 1;
	
		# 以停牌前最后一个交易日的流通市值 作为今日的流通市值
		set p_CirculatingValue =  p_circulating * p_close ;

	ELSEIF p_previousDate is null then
		# 新股上市，纳入指数计算
		set p_remark = '纳入指数';

		select AShares into p_circulating 
			from capitalstructure where  real_date <= p_currentDate 
			and code = p_nCode  ORDER BY real_date desc limit 1;
	
		set p_CirculatingValue =  p_circulating * p_close ;
		set p_RepairValue = p_CirculatingValue;
	ELSE
		# 今日为正常交易日
		set p_remark = '';
		select tradedate, close into p_prev_tradedate, p_prev_close 
			from souhutradingdaily where code = p_nCode
			and tradedate <= p_previousDate ORDER BY tradedate desc limit 1;

		# 前一个交易日为停牌，今日复牌则，计算停牌以来的所有 p_RepairValue
		# 前一个交易日为正常交易日，则计算今日的 p_RepairValue

		select count(*) into p_isExist from capitalstructure 
			where  code = p_nCode  and real_date > p_prev_tradedate and real_date <= p_currentDate;
	#		and not exists (select 1 from capitalstructure  
	#		where code = p_nCode  and real_date > p_prev_tradedate and real_date <= p_currentDate
	#		and change_reason_desc like '%定期报告%' );
		
		if p_isExist > 0 then
			select AShares into p_circulating 
				from capitalstructure where real_date > p_prev_tradedate and real_date <= p_currentDate 
				and code = p_nCode  ORDER BY real_date desc limit 1;

			select AShares into p_prev_circulating 
				from capitalstructure where real_date <= p_prev_tradedate  
				and code = p_nCode ORDER BY real_date desc limit 1;
			
			set p_prev_exit_right_price = p_close - p_change_amount;
			set p_RepairValue =  (p_prev_exit_right_price * p_circulating) - (p_prev_close * p_prev_circulating); 

			# 计算今日 流通市值
			set p_CirculatingValue =  p_circulating * p_close ;

		else 
			set p_RepairValue = 0;

			# 计算今日 流通市值
			select AShares into p_circulating 
				from capitalstructure where  real_date <= p_currentDate 
				and code = p_nCode  ORDER BY real_date desc limit 1;
	
			set p_CirculatingValue =  p_circulating * p_close ;
		end if;
	end if;

	# 
	
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for HuShen_PlateIndexByCode
-- ----------------------------
DROP PROCEDURE IF EXISTS `HuShen_PlateIndexByCode`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `HuShen_PlateIndexByCode`(IN `p_IndexCode` int, IN  `p_IndexName` varchar(256), IN  `p_IndexType` varchar(32), IN `p_StartPoint` double, IN `p_StartDate` date)
    MODIFIES SQL DATA
BEGIN
	#Routine body goes here...
	DECLARE Done INT DEFAULT 0;
	declare p_division double;
	declare p_circulating_value double;
	declare p_yesterday_circulating_value double;
	declare p_repair_value double;
	declare p_tradedate date;
	declare p_newpoint double;
	declare p_isExist int ;
	
	declare rs cursor for select tradedate from sohuindexdaily 
		where code = 1 and TradeDate >= p_StartDate ORDER BY tradedate;

	/* 异常处理 */
	DECLARE CONTINUE HANDLER FOR NOT FOUND SET Done = 1;

	create table if not exists hushen_plate_index(code int, index_type varchar(32), name varchar(256),
		tradedate date, circulating_value DECIMAL(16,2), repair_value DECIMAL(14,2), division DECIMAL(11,2), 
		point DECIMAL(8,2), PRIMARY key( code, index_type, tradedate ));

	set p_yesterday_circulating_value = 0;

	open rs;
	fetch rs into p_tradedate;
label:
	REPEAT
		if Done THEN
			leave label;
		ELSE
			#select sum(circulating_value), sum(repair_value)  into  p_circulating_value, p_repair_value
			#	from hushen_stock_index  where  p_tradedate = tradedate 
			#	and HuShen_IsStockInThePlate(code, p_IndexCode, p_IndexType) = 1;
			call HuShen_SumPlateIndexDaily(p_IndexCode, p_IndexType, p_tradedate, p_circulating_value, p_repair_value);

			#call HuShen_SumPlateIndexDaily(p_IndexCode, p_IndexType, p_tradedate, p_circulating_value, p_repair_value);
			if p_yesterday_circulating_value = 0 THEN
				set p_division = p_circulating_value/p_StartPoint;
			else
				if p_repair_value <> 0 THEN
					/* 本日需要调整 指数的除数 */
					set p_division = (p_yesterday_circulating_value + p_repair_value)/p_newpoint;
				end if;
			end if;
			/* 计算本日的新指数  */
			set p_newpoint = p_circulating_value/p_division;
			set p_yesterday_circulating_value = p_circulating_value;

			/*保存到数据库*/
			select count(*) into p_isExist from hushen_plate_index 
				where tradedate = p_tradedate and code = p_IndexCode and index_type = p_IndexType;
			if p_isExist = 0 THEN
				insert into hushen_plate_index(code,name,index_type, tradedate, circulating_value,repair_value,division, point)
					select p_IndexCode,p_IndexName,p_IndexType, p_tradedate, p_circulating_value, p_repair_value, p_division, p_newpoint;
			ELSE
				update hushen_plate_index set circulating_value = p_circulating_value,
					repair_value = p_repair_value, division = p_division, point = p_newpoint 
					where code = p_IndexCode and tradedate = p_tradedate and index_type = p_IndexType ;
			end if;
		end if;
		fetch rs into p_tradedate;
	until Done end repeat;
	close rs;
	
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for HuShen_PlateIndexByCodeTotally
-- ----------------------------
DROP PROCEDURE IF EXISTS `HuShen_PlateIndexByCodeTotally`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `HuShen_PlateIndexByCodeTotally`(IN `p_IndexCode` int, IN  `p_IndexName` varchar(256), IN  `p_IndexType` varchar(32), IN `p_StartPoint` double, IN `p_StartDate` date)
    READS SQL DATA
BEGIN
	#Routine body goes here...
	declare p_existTable int ;
	declare p_fromDate date;
	declare p_point double;

	# 建立该板块所有的股票代码表 
	create temporary table tmp_table (code int not null) type = heap;
	insert into tmp_table select code  from stockinfo 
		where  HuShen_IsStockInThePlate(code, p_IndexCode, p_IndexType) = 1;

	select count(*) into p_existTable from information_schema.tables where table_name = 'HuShen_plate_index';
	if p_existTable = 0 THEN
			call HuShen_PlateIndexByCode(p_IndexCode, p_IndexName, p_IndexType, p_StartPoint, p_StartDate );
	ELSE
			select count(*) into p_existTable from HuShen_plate_index where code = p_IndexCode and index_type = p_IndexType ;
			if  p_existTable > 0 THEN
					select max(tradedate) into p_fromDate from hushen_plate_index 
						where code =p_IndexCode and index_type = p_IndexType;
					set p_fromDate = HuShen_GetTradeDate(p_fromDate, -5);
					select point into p_point from hushen_plate_index 
						where code =p_IndexCode and index_type =p_IndexType and tradedate = p_fromDate;
					call HuShen_PlateIndexByCode(p_IndexCode, p_IndexName, p_IndexType, p_point, p_fromDate);
			else
					call HuShen_PlateIndexByCode(p_IndexCode, p_IndexName, p_IndexType, p_StartPoint, p_StartDate);
			end if;
	end if;

	drop table tmp_table;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for HuShen_SohuPlateIndex
-- ----------------------------
DROP PROCEDURE IF EXISTS `HuShen_SohuPlateIndex`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `HuShen_SohuPlateIndex`(IN `p_nCode` int, IN `p_StartPoint` double, IN `p_StartDate` date)
    READS SQL DATA
BEGIN
	#Routine body goes here...
		#Routine body goes here...
	DECLARE Done INT DEFAULT 0;

	declare p_fromDate date;
	declare cur_code int;
	declare p_existTable int;
	declare p_point double;
	declare p_plateName varchar(256);
	
	# 获取直至当日已经上市的股市代码
	declare rs CURSOR for select code, name from souhustockplates 
		where is_stock =0 and parent_code= p_nCode order by code;

	/* 异常处理 */
	DECLARE CONTINUE HANDLER FOR NOT FOUND SET Done = 1;

	open rs;
	fetch next from rs into cur_code, p_plateName;		
label:
	REPEAT
		if Done THEN
			leave label;
		ELSE
			call HuShen_PlateIndexByCodeTotally(cur_code, p_plateName, 'Sohu', p_StartPoint, p_StartDate);
		end if;
		fetch next from rs into cur_code, p_plateName;
	until Done end repeat;
	close rs;

END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for HuShen_StockIndexByCode
-- ----------------------------
DROP PROCEDURE IF EXISTS `HuShen_StockIndexByCode`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `HuShen_StockIndexByCode`(IN `p_fromDate` date, IN `p_toDate` date, IN `p_nCode` int)
    MODIFIES SQL DATA
BEGIN
	#Routine body goes here...
	DECLARE Done INT DEFAULT 0;

	declare p_abbreviation varchar(32);
	DECLARE	curDate date;

	declare p_previousDate date DEFAULT NULL;
	declare p_close_price double;		/* 本日收盘价 */
	declare p_close_index double;		/* 本日复权价 */
	declare p_change_rate double;		/**/
	declare p_circulating_value double;		/* 本日流通市值 */
	declare p_repair_value double;				/* 本日针对昨日流通市值的修正值 */
	declare p_index_begin_date date;	
	declare p_remark varchar(32);		

	declare isExist int DEFAULT 0;
	
	/* 以上证综指日期作为 股市开市交易的依据, 但仅仅计算到 本机获取到最晚的交易数据日期 */
	declare  rs cursor for select tradedate from sohuindexdaily 
		where code = 1 and tradedate >= p_fromDate and tradedate <= p_toDate 
		and tradedate >= p_index_begin_date order by TradeDate ;

	/* 异常处理 */
	DECLARE CONTINUE HANDLER FOR NOT FOUND SET Done = 1;

	create table if not exists HuShen_Stock_Index ( code int, abbreviation VARCHAR(32), tradedate date, 
		circulating_value DECIMAL(15, 2), repair_value DECIMAL(13, 2), change_rate DECIMAL(8,2), 
		close DECIMAL(8, 2), close_index DECIMAL(8, 2), remark varchar(32), PRIMARY key( code, tradedate ))
		partition by range(year(tradedate))(
		partition p2009 values less than (2010),
		partition p2010 values less than (2011),
		partition p2011 values less than (2012),
		partition p2012 values less than (2013),
		partition p2013 values less than (2014),
		partition p2014 values less than (2015),
		partition p2015 values less than (2016),
		partition p2016 values less than (2017),
		partition p2017 values less than (2018),
		partition p2018 values less than (2019),
		partition p2019 values less than (2020),
		partition p2020 values less than (2021),
		partition p2021 values less than (2022),
		partition p2022 values less than (2023),
		partition p2023 values less than maxvalue);

	call add_ind('HuShen_Stock_Index', 'code', 'idx_code');
	call add_ind('HuShen_Stock_Index', 'tradedate', 'idx_tradedate');

	/*  */
	set p_circulating_value = 0;
	set p_repair_value = 0;
	set p_change_rate = 0;
	set p_close_price = 0;
	
	/* 获取前一个交易日的复权价 */
	select close_index into p_close_index from HuShen_Stock_Index 
		where code = p_nCode and tradedate < p_fromDate ORDER BY tradedate desc limit 1;
	if p_close_index is null THEN
		set p_close_index = 0;
	end if;

	select abbreviation into p_abbreviation from stockinfo where code = p_nCode;

	select HuShen_GetTradeDate(real_date, 11) into p_index_begin_date from capitalstructure 
		where  code = p_nCode  and  change_reason_desc like '%A股上市%' ;

	/* 打开游标 */
	set Done = 0;
	open rs;
	fetch next from rs into curDate;
label:
	REPEAT
		if Done THEN
			leave label;
		ELSE

			if p_previousDate is NULL THEN
				if curDate > p_index_begin_date then 
					set p_previousDate = HuShen_GetTradeDate(curDate, -1);
				end if;
			end if;

			call HuShen_GetStockStatus(curDate, p_previousDate, p_nCode, p_close_price,
				p_change_rate, p_circulating_value, p_repair_value, p_remark);
			if p_close_index = 0 THEN
				set p_close_index = p_close_price;
			ELSE
				set p_close_index = p_close_index * (1+ p_change_rate /100);
			end if;

			/*if cur_close is NULL then 
				select cur_code, curDate from dual;
			end if;*/

			select count(*) into isExist from HuShen_Stock_Index where tradedate = curDate and code = p_nCode;
			if isExist = 0 then 
				insert into HuShen_Stock_Index (code, abbreviation, tradedate, circulating_value,
					repair_value, change_rate, close, close_index, remark)
				select p_nCode, p_abbreviation, curDate, p_circulating_value,
					p_repair_value, p_change_rate, p_close_price, p_close_index ,p_remark ;
			ELSE
				update HuShen_Stock_Index 
				set circulating_value = p_circulating_value, 
				repair_value = p_repair_value,
				change_rate = p_change_rate,
				close = p_close_price ,
				close_index = p_close_index,
				remark = p_remark
				where tradedate = curDate and code = p_nCode;
			end if;

			set p_previousDate = curDate;
		end if;
		fetch next from rs into curDate;
	until Done 
	end repeat;

	close rs;

	#return circulating_value;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for HuShen_StockIndexTotally
-- ----------------------------
DROP PROCEDURE IF EXISTS `HuShen_StockIndexTotally`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `HuShen_StockIndexTotally`()
    MODIFIES SQL DATA
BEGIN
	#Routine body goes here...
		#Routine body goes here...
	DECLARE Done INT DEFAULT 0;

	declare p_fromDate date;
	declare p_toDate date;
	declare cur_code int;
	declare cur_date date;
	declare p_existTable int;
	
	# 获取直至当日已经上市的股市代码
	declare rs CURSOR for select code, real_date from CapitalStructure 
		where change_reason_desc like '%A股上市%' order by code;

	/* 异常处理 */
	DECLARE CONTINUE HANDLER FOR NOT FOUND SET Done = 1;

	# 获取交易数据最晚的日期
	call add_ind('souhutradingdaily', 'tradedate', 'idx_tradedate');
	select max(tradedate) into p_toDate from souhutradingdaily;

	open rs;
	fetch next from rs into cur_code, cur_date;		/* cur_date 保存 A股上市日期 */
label:
	REPEAT
		if Done THEN
			leave label;
		ELSE
			select count(*) into p_existTable from information_schema.tables where table_name = 'HuShen_stock_index';
			if p_existTable = 0 THEN
				if cur_date > '2010-1-1' then
					call HuShen_StockIndexByCode(cur_date, p_toDate, cur_code);	
				else 
					call HuShen_StockIndexByCode('2010-1-1', p_toDate, cur_code);
				end if;
			ELSE
				select max(tradedate) into p_fromDate from HuShen_stock_index where code = cur_code ;
				if  p_fromDate = ''  or p_fromDate is null THEN
					if cur_date > '2010-1-1' then
						call HuShen_StockIndexByCode(cur_date, p_toDate, cur_code);	
					else 
						call HuShen_StockIndexByCode('2010-1-1', p_toDate, cur_code);
					end if;
				else
					set p_fromDate = HuShen_GetTradeDate(p_fromDate, -5);
					call HuShen_StockIndexByCode(p_fromDate,  p_toDate, cur_code);
				end if;
			end if;
		end if;
		fetch next from rs into cur_code, cur_date;
	until Done end repeat;
	close rs;

END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for HuShen_SumPlateIndexDaily
-- ----------------------------
DROP PROCEDURE IF EXISTS `HuShen_SumPlateIndexDaily`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `HuShen_SumPlateIndexDaily`(IN `p_IndexCode` int, IN  `p_IndexType` varchar(32), IN `p_CurrentDate` date, OUT `p_circulating_sum` double, OUT `p_repair_sum`  double)
    READS SQL DATA
BEGIN
	#Routine body goes here...
	DECLARE Done INT DEFAULT 0;
	declare p_code int;
	declare p_circulating_value double;
	declare p_repair_value double;
	declare p_tradedate date;
	declare p_isExist int;

	declare rs cursor for select code from tmp_table;  
	
	/* 异常处理 */
	DECLARE CONTINUE HANDLER FOR NOT FOUND SET Done = 1;

	set p_circulating_sum = 0;
	set p_repair_sum = 0;
	open rs;
	fetch rs into p_code;
label:
	REPEAT
		if Done THEN
			leave label;
		ELSE
			select count(*) into p_isExist from hushen_stock_index 
				where tradedate = p_CurrentDate and code = p_code and circulating_value is not null 
				and repair_value is not null ;
			if p_isExist > 0 THEN
				select circulating_value, repair_value into p_circulating_value, p_repair_value
				from hushen_stock_index where tradedate = p_CurrentDate and code = p_code ;

				set p_circulating_sum = p_circulating_sum + p_circulating_value;
				if p_repair_value <> 0 then
					set p_repair_sum = p_repair_sum + p_repair_value;
				end if;
			end if;
		end if;
		fetch rs into p_code;
	until Done end repeat;
	close rs;
	
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for NetProfitBelongingToParentCompany
-- ----------------------------
DROP PROCEDURE IF EXISTS `NetProfitBelongingToParentCompany`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `NetProfitBelongingToParentCompany`(nCode int, `curDate` date, out `baseDate` varchar(32), out `isLast` int,out net_profit double)
    READS SQL DATA
BEGIN
	#Routine body goes here...
	DECLARE	Done int DEFAULT 0;
	DECLARE odt_cur date;
	DECLARE odt_max date;
	DECLARE isOk int;
	DECLARE rs cursor for select report_date, colume_41 from incomestatement where code = nCode order by report_date desc;
	/* 异常处理 */
	DECLARE CONTINUE HANDLER FOR NOT FOUND SET Done = 1;
	
	set net_profit = -1;
	set isOk = 0;
	/* 打开游标 */
	open rs;
	fetch next from rs into odt_cur, net_profit;
label:
	REPEAT
		if Done THEN
			leave label;
		ELSE
			if curDate >= odt_cur THEN
				set isOk = 1;
				leave label;
			end if;
		end if;
		fetch next from rs into odt_cur, net_profit;
	until Done 
	end repeat;
	close rs;

	/*计算本次利润基于的季度时间 */
	if isOk = 1 THEN
		select max(report_date) into odt_max from incomestatement where code = nCode;
		if odt_max= odt_cur THEN
			set isLast = 1;
		ELSE
			set isLast = 0;
		end if;

		set baseDate = odt_cur;
	ELSE
		set net_profit = -1;
		set baseDate = '';
	end if;

END
;;
DELIMITER ;

-- ----------------------------
-- Function structure for CalculateShanghaiWeight
-- ----------------------------
DROP FUNCTION IF EXISTS `CalculateShanghaiWeight`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` FUNCTION `CalculateShanghaiWeight`(`total_shares` double,`circulating_shares` double) RETURNS double
    NO SQL
BEGIN
	#Routine body goes here...
	declare  weight double;
	set weight = circulating_shares/total_shares * 100;
	if weight <= 15 then 
		set weight = weight;
	ELSEIF weight <= 20 THEN
		set weight = 20;
	ELSEIF weight <= 30 THEN
		set weight = 30;
	ELSEIF weight <= 40 THEN
		set weight = 40;
	ELSEIF weight <= 50 THEN
		set weight = 50;
	ELSEIF weight <= 60 THEN
		set weight = 60;
	ELSEIF weight <= 70 THEN
		set weight = 70;
	ELSEIF weight <= 80 THEN
		set weight = 80;
	ELSE
		set weight = 100;
	end if;
	set weight = weight/100;
	RETURN weight;
END
;;
DELIMITER ;

-- ----------------------------
-- Function structure for CurrentACirculatingShares
-- ----------------------------
DROP FUNCTION IF EXISTS `CurrentACirculatingShares`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` FUNCTION `CurrentACirculatingShares`(`nCode` int,`curDate` date) RETURNS double
    READS SQL DATA
BEGIN
	#Routine body goes here...
	DECLARE Done INT DEFAULT 0;
	DECLARE	odt_cur date;
	declare a_circulating double DEFAULT 0;
	
	select AShares into a_circulating from capitalstructure 
		where code = nCode and real_date <= curDate order by real_date desc limit 1; 
	
	if a_circulating is NULL THEN
		set a_circulating = 0;
	end if;
	RETURN a_circulating;
END
;;
DELIMITER ;

-- ----------------------------
-- Function structure for CurrentAddIssuancePrice
-- ----------------------------
DROP FUNCTION IF EXISTS `CurrentAddIssuancePrice`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` FUNCTION `CurrentAddIssuancePrice`(`p_code` int,`p_cur_date` date, `p_add_issuance_price` double) RETURNS double
    READS SQL DATA
BEGIN
	#Routine body goes here...
	DECLARE Done INT DEFAULT 0;
	DECLARE	p_right_record_date date;
	declare p_exit_right_date date;
	
	DECLARE rs cursor for select right_record_date, exit_right_date from 10jqka_bonus 
		where code = p_code and right_record_date >= p_cur_date and exit_right_date > right_record_date 
		order by right_record_date ;
	/* 异常处理 */
	DECLARE CONTINUE HANDLER FOR NOT FOUND SET Done = 1;
	/* 打开游标 */
	open rs;
	fetch next from rs into p_right_record_date, p_exit_right_date;
label:
	REPEAT
		if Done THEN
			leave label;
		ELSE
			set p_add_issuance_price = YesterdayClosePriceAfterExitRight(p_code, p_exit_right_date,
				p_add_issuance_price , p_right_record_date);
		end if;
		fetch next from rs into p_right_record_date, p_exit_right_date;
	until Done 	end repeat;

	close rs;
	RETURN p_add_issuance_price;
END
;;
DELIMITER ;

-- ----------------------------
-- Function structure for CurrentATotalShares
-- ----------------------------
DROP FUNCTION IF EXISTS `CurrentATotalShares`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` FUNCTION `CurrentATotalShares`(`nCode` int,`curDate` date) RETURNS double
    READS SQL DATA
BEGIN
	#Routine body goes here...
	DECLARE Done INT DEFAULT 0;
	DECLARE	odt_cur date;
	declare a_total double DEFAULT 0;
	
	select A_total_shares into a_total 
		from capitalstructure where code = nCode  and real_date <= curDate order by real_date desc limit 1;
	
	if a_total is NULL THEN
		set a_total = 0;
	end if;
	RETURN a_total;
END
;;
DELIMITER ;

-- ----------------------------
-- Function structure for CurrentChangeAmount
-- ----------------------------
DROP FUNCTION IF EXISTS `CurrentChangeAmount`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` FUNCTION `CurrentChangeAmount`(`nCode` int,`curDate` date) RETURNS double
    READS SQL DATA
BEGIN
	#Routine body goes here...
	DECLARE Done INT DEFAULT 0;
	DECLARE	odt_cur date;
	declare close_price double DEFAULT 0;
	
	select Change_Amount into close_price  from souhutradingdaily 
		where code = nCode and TradeDate <= curDate ORDER BY TradeDate desc limit 1;
	
	if close_price is NULL THEN
		set close_price = 0;
	end if;

	RETURN close_price;
END
;;
DELIMITER ;

-- ----------------------------
-- Function structure for CurrentClosePrice
-- ----------------------------
DROP FUNCTION IF EXISTS `CurrentClosePrice`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` FUNCTION `CurrentClosePrice`(`nCode` int,`curDate` date) RETURNS double
    READS SQL DATA
BEGIN
	#Routine body goes here...
	DECLARE Done INT DEFAULT 0;
	DECLARE	odt_cur date;
	declare close_price double DEFAULT 0;
	
	select Close into close_price  from souhutradingdaily 
		where code = nCode and TradeDate <= curDate ORDER BY TradeDate desc limit 1;
	
	if close_price is NULL THEN
		set close_price = 0;
	end if;
	RETURN close_price;
END
;;
DELIMITER ;

-- ----------------------------
-- Function structure for CurrentOpenPrice
-- ----------------------------
DROP FUNCTION IF EXISTS `CurrentOpenPrice`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` FUNCTION `CurrentOpenPrice`(`nCode` int,`curDate` date) RETURNS double
    READS SQL DATA
BEGIN
	#Routine body goes here...
	DECLARE Done INT DEFAULT 0;
	DECLARE	odt_cur date;
	declare close_price double DEFAULT 0;
	
	select Open into close_price from souhutradingdaily 
		where code = nCode and TradeDate <= curDate ORDER BY TradeDate desc limit 1;
	
	if close_price is NULL THEN
		set close_price = 0;
	end if;
	RETURN close_price;
END
;;
DELIMITER ;

-- ----------------------------
-- Function structure for CurrentShanghaiPlateIndex
-- ----------------------------
DROP FUNCTION IF EXISTS `CurrentShanghaiPlateIndex`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` FUNCTION `CurrentShanghaiPlateIndex`(`curDate` date,`nPlateCode` int) RETURNS double
    READS SQL DATA
BEGIN
	#Routine body goes here...
	DECLARE Done INT DEFAULT 0;
	declare cur_code int;
	declare tmp double;
	declare total_index double DEFAULT 0 ;
	declare isExist int ;

	declare rs CURSOR for select code  
		from souhustockplates where Parent_Code = nPlateCode 
		and Is_Stock = 1 and code >= 600000 and code < 700000;

	/* 异常处理 */
	DECLARE CONTINUE HANDLER FOR NOT FOUND SET Done = 1;

	/* 打开游标 */
	open rs;
	fetch next from rs into cur_code;
label:
	REPEAT
		if Done THEN
			leave label;
		ELSE
			select count(*) into isExist from shanghai_index where code = cur_code and tradedate = curDate;
			if isExist = 0 THEN
				# 当时应该该股未上市
				set tmp = 0;
			ELSE
				select self_index into tmp from shanghai_index where code = cur_code and tradedate = curDate;
			end if;
							
			set total_index = total_index + tmp;
		end if;
		fetch next from rs into cur_code;
	until Done 
	end repeat;

	close rs;

	RETURN total_index;
END
;;
DELIMITER ;

-- ----------------------------
-- Function structure for CurrentShanghaiStockCirculatingValue
-- ----------------------------
DROP FUNCTION IF EXISTS `CurrentShanghaiStockCirculatingValue`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` FUNCTION `CurrentShanghaiStockCirculatingValue`(`curDate` date,`nCode` int) RETURNS double
    READS SQL DATA
BEGIN
	#Routine body goes here...
	declare circulating_value double DEFAULT 0 ;

	declare cur_total_shares double;
	declare cur_weight double;
	declare cur_close double;

	select total_shares, weight into cur_total_shares, cur_weight from Shanghai_CapitalStructure 
		where code = nCode and real_date <= curDate ORDER BY real_date desc limit 1;

	select close into cur_close from souhutradingdaily where code = nCode 
		and tradeDate<= curDate order by tradeDate desc limit 1;
			
	set circulating_value =  cur_total_shares * cur_weight * cur_close;

	RETURN circulating_value;
END
;;
DELIMITER ;

-- ----------------------------
-- Function structure for CurrentShanghaiStockRepairCirculating
-- ----------------------------
DROP FUNCTION IF EXISTS `CurrentShanghaiStockRepairCirculating`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` FUNCTION `CurrentShanghaiStockRepairCirculating`(`curDate` date,`nCode` int) RETURNS double
    READS SQL DATA
BEGIN
	#Routine body goes here...
	DECLARE Done INT DEFAULT 0;

	declare cur_code int;
	DECLARE	cur_real_date date;

	declare cur_prev_total double;
	declare cur_prev_circulating double;
	declare cur_prev_close double;
	declare cur_weight double;
	declare cur_prev_exit_right_price double;
	declare cur_total_shares double;

	declare circulating_value double;
	declare tmp double;
	declare isExist int DEFAULT 0 ;

	# 获取直至当日已经上市的股市代码
	declare rs CURSOR for select prev_total, prev_circulating, prev_close, weight, prev_exit_right_price, total_shares
		from Shanghai_CapitalStructure where  real_date = curDate and code = nCode;

	/* 异常处理 */
	DECLARE CONTINUE HANDLER FOR NOT FOUND SET Done = 1;

	select count(*) into isExist from Shanghai_CapitalStructure where  real_date = curDate and code = nCode;
	/* 计算当日总流通市值，做了考档处理 */
	set circulating_value = 0;

	if isExist > 0 then 
		/* 打开游标 */
		open rs;
		fetch next from rs into cur_prev_total, cur_prev_circulating, cur_prev_close, 
			cur_weight, cur_prev_exit_right_price, cur_total_shares;
label:
		REPEAT
			if Done THEN
				leave label;
			ELSE
				set tmp = CalculateShanghaiWeight(cur_prev_total, cur_prev_circulating);
				set circulating_value = circulating_value - (cur_prev_total * tmp * cur_prev_close);
				set circulating_value = circulating_value + cur_weight * cur_total_shares * cur_prev_exit_right_price;
					
			end if;
			fetch next from rs into cur_prev_total, cur_prev_circulating, cur_prev_close, 
				cur_weight, cur_prev_exit_right_price, cur_total_shares;
		until Done 
		end repeat;

		close rs;
	end if;

	return circulating_value;
END
;;
DELIMITER ;

-- ----------------------------
-- Function structure for EstimateNetProfits
-- ----------------------------
DROP FUNCTION IF EXISTS `EstimateNetProfits`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` FUNCTION `EstimateNetProfits`(`nCode` int,`curDate` date) RETURNS double
    READS SQL DATA
BEGIN
	#Routine body goes here...
	declare baseDate date;
	declare isLast int;
	declare net_profit double;
	declare curMonth int;
	declare curYear int;
	
	call NetProfitBelongingToParentCompany(nCode, curDate, baseDate, isLast, net_profit);

	#¹ÀËã±¾ÄêµÄÀûÈó
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
;;
DELIMITER ;

-- ----------------------------
-- Function structure for FinanceRptReleaseDate
-- ----------------------------
DROP FUNCTION IF EXISTS `FinanceRptReleaseDate`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` FUNCTION `FinanceRptReleaseDate`(`nCode` int,`reportDate` date) RETURNS date
    READS SQL DATA
BEGIN
	#Routine body goes here...
	# 查找某个季度的 财务报告的发布日期

	DECLARE	Done int DEFAULT 0;
	DECLARE odt_cur date;
	DECLARE isOk int;
	declare strLike varchar(32);
	declare strCode varchar(16);
	declare nYear int;
	declare nMonth int;
	DECLARE rs cursor for select announcementTime from cninfoannouncement 
			where secCode = strCode and announcementTitle like strLike;
	/* 异常处理 */
	DECLARE CONTINUE HANDLER FOR NOT FOUND SET Done = 1;
	
	set isOk = 0;
	set strCode = convert(nCode, char(6));
	while LENGTH(strCode) < 6 do
		set strCode = concat('0', strCode);
	end while;
	
	set nYear = year(reportDate);
	set nMonth = MONTH(reportDate);
	if  nMonth=3 THEN
		set strLike = CONCAT(nYear,'年','第一季度报告%');
	end if;
	if nMonth=6 THEN
		set strLike = CONCAT(nYear,'年','半年度报告%');
	end if;
	if nMonth=9 THEN
		set strLike = CONCAT(nYear,'年','第三季度报告%');
	end if;
	if nMonth=12 THEN
		set strLike = CONCAT(nYear,'年','年度报告%');
	end if;
	/* 打开游标 */
	open rs;
	fetch next from rs into odt_cur;
label:
	REPEAT
		if Done THEN
			leave label;
		ELSE
			set isOk = 1;
			leave label;
		end if;
		#fetch next from rs into odt_cur, net_profit;
	until Done 
	end repeat;

	close rs;
	if isOk = 1 THEN
		return odt_cur;
	else 
		RETURN '1900-01-01';
	end if;
END
;;
DELIMITER ;

-- ----------------------------
-- Function structure for HuShen_GetChangeRate1111
-- ----------------------------
DROP FUNCTION IF EXISTS `HuShen_GetChangeRate1111`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` FUNCTION `HuShen_GetChangeRate1111`(`curDate` date,`nCode` int) RETURNS double
    READS SQL DATA
BEGIN
	#Routine body goes here...
	DECLARE Done INT DEFAULT 0;
	DECLARE	odt_cur date;
	declare p_change_rate double DEFAULT 0;
	
	select Change_Rate into p_change_rate  from souhutradingdaily 
		where code = nCode and TradeDate = curDate ORDER BY TradeDate desc limit 1;
	
	if p_change_rate is NULL THEN
		set p_change_rate = 0;
	end if;

	RETURN p_change_rate;
END
;;
DELIMITER ;

-- ----------------------------
-- Function structure for HuShen_GetStockCirculatingValue1111
-- ----------------------------
DROP FUNCTION IF EXISTS `HuShen_GetStockCirculatingValue1111`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` FUNCTION `HuShen_GetStockCirculatingValue1111`(`curDate` date,`nCode` int) RETURNS double
    READS SQL DATA
BEGIN
	# 计算沪深股市中某个股票某日的流通市值
	declare circulating_value double DEFAULT 0 ;

	declare p_circulating_shares double;
	declare p_close double;

	select circulating_shares into p_circulating_shares from HuShen_CapitalStructure 
		where code = nCode and real_date <= curDate ORDER BY real_date desc limit 1;

	select close into p_close from souhutradingdaily where code = nCode 
		and tradeDate<= curDate order by tradeDate desc limit 1;
			
	set circulating_value =  p_circulating_shares * p_close ;

	RETURN circulating_value;
END
;;
DELIMITER ;

-- ----------------------------
-- Function structure for HuShen_GetStockRepairCirculating1111
-- ----------------------------
DROP FUNCTION IF EXISTS `HuShen_GetStockRepairCirculating1111`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` FUNCTION `HuShen_GetStockRepairCirculating1111`(`curDate` date,`nCode` int) RETURNS double
    READS SQL DATA
BEGIN
	#计算沪深股市中某个股票某日 针对昨日流通市值的修正值
	DECLARE Done INT DEFAULT 0;

	declare cur_code int;
	DECLARE	cur_real_date date;

	declare cur_prev_total double;
	declare cur_prev_circulating double;
	declare cur_prev_close double;
	declare cur_prev_exit_right_price double;
	declare cur_total_shares double;
	declare cur_circulating_shares double;

	declare circulating_value double;
	declare tmp double;
	declare isExist int DEFAULT 0 ;

	# 获取直至当日已经上市的股市代码
	declare rs CURSOR for select prev_total, prev_circulating, prev_close, prev_exit_right_price, 
		total_shares, circulating_shares from HuShen_CapitalStructure where real_date = curDate and code = nCode;

	/* 异常处理 */
	DECLARE CONTINUE HANDLER FOR NOT FOUND SET Done = 1;

	select count(*) into isExist from HuShen_CapitalStructure where  real_date = curDate and code = nCode;
	/* 计算当日总流通市值，做了考档处理 */
	set circulating_value = 0;

	if isExist > 0 then 
		/* 打开游标 */
		open rs;
		fetch next from rs into cur_prev_total, cur_prev_circulating, cur_prev_close, 
			cur_prev_exit_right_price, cur_total_shares, cur_circulating_shares;
label:
		REPEAT
			if Done THEN
				leave label;
			ELSE
				set circulating_value = cur_prev_circulating * cur_prev_close;
				set circulating_value = cur_circulating_shares * cur_prev_exit_right_price - circulating_value ;
					
			end if;
			fetch next from rs into cur_prev_total, cur_prev_circulating, cur_prev_close, 
				cur_prev_exit_right_price, cur_total_shares, cur_circulating_shares;
		until Done 
		end repeat;

		close rs;
	end if;

	return circulating_value;
END
;;
DELIMITER ;

-- ----------------------------
-- Function structure for HuShen_GetTradeDate
-- ----------------------------
DROP FUNCTION IF EXISTS `HuShen_GetTradeDate`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` FUNCTION `HuShen_GetTradeDate`(`fromDate` date,`nDay` int) RETURNS date
    READS SQL DATA
BEGIN
	# 计算第 n 个交易日的日期
	DECLARE Done INT DEFAULT 0;

	DECLARE	cur_real_date date;
	
	declare rs CURSOR for select tradedate from sohuindexdaily 
		where code = 1 and TradeDate >= fromDate ORDER BY tradedate;

	declare rs1 CURSOR for select tradedate from sohuindexdaily 
		where code = 1 and TradeDate <= fromDate ORDER BY tradedate desc;

	/* 异常处理 */
	DECLARE CONTINUE HANDLER FOR NOT FOUND SET Done = 1;
	
	if nDay > 0 then 
		/* 打开游标 */
		set Done = 0;
		open rs;
		fetch next from rs into  cur_real_date;
label:
		REPEAT
			if Done THEN
				leave label;
			ELSE
				if nDay = 0 THEN
					leave label;
				ELSE 
					set nDay = nDay - 1;
				end if;
			end if;
			fetch next from rs into cur_real_date;
		until Done 
		end repeat;
		close rs;
	ELSE
		/* 打开游标 */
		set Done = 0;
		open rs1;
		fetch next from rs1 into  cur_real_date;
label1:
		REPEAT
			if Done THEN
				leave label1;
			ELSE
				if nDay = 0 THEN
					leave label1;
				ELSE 
					set nDay = nDay + 1;
				end if;
			end if;
			fetch next from rs1 into cur_real_date;
		until Done 
		end repeat;
		close rs1;
	end if;
	
	if nDay <> 0 THEN
		set cur_real_date = '0000-00-00';
	end if;
	RETURN  cur_real_date;
END
;;
DELIMITER ;

-- ----------------------------
-- Function structure for HuShen_IsStockInThePlate
-- ----------------------------
DROP FUNCTION IF EXISTS `HuShen_IsStockInThePlate`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` FUNCTION `HuShen_IsStockInThePlate`(`p_StockCode` int, `p_IndexCode` int, `p_IndexType` varchar(32)) RETURNS int(11)
    READS SQL DATA
BEGIN
	#Routine body goes here...
	declare nYesOrNo int DEFAULT 0 ;
	if p_IndexType = 'HuShen' then
		if p_IndexCode = 399102 THEN
			/* 创业板综合指数 */
			if p_StockCode >=300000 and p_StockCode < 399999 THEN
				set nYesOrNo = 1;
			end if;
		ELSEIF p_IndexCode = 399101 THEN 
			/* 中小板综合指数 */
			if p_StockCode >=2000 and p_StockCode < 2999 THEN
				set nYesOrNo = 1;
			end if;
		elseif p_IndexCode = 399100 THEN
			/* 深圳新指数 */
			if p_StockCode >=000001 and p_StockCode < 600000 THEN
				set nYesOrNo = 1;
			end if;
		elseif p_IndexCode = 1 THEN
			/* 上证综合指数 */
			if p_StockCode >=600000  THEN
				set nYesOrNo = 1;
			end if;
		end if;
	ELSEIF p_IndexType = 'Sohu' THEN	
		select count(*) into nYesOrNo from souhustockplates 
			where code =p_StockCode and Is_Stock = 1 and Parent_Code = p_IndexCode;
	end if;

	RETURN nYesOrNo;
END
;;
DELIMITER ;

-- ----------------------------
-- Function structure for IsAddIssuanceMarketDate
-- ----------------------------
DROP FUNCTION IF EXISTS `IsAddIssuanceMarketDate`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` FUNCTION `IsAddIssuanceMarketDate`(`p_nCode` int,`p_curDate` date,`p_yesterday` date) RETURNS int(11)
    READS SQL DATA
BEGIN
	#Routine body goes here...
	DECLARE p_result int default 0;
	select 1 into  p_result 
		from cfi_add_issuance where  market_date > p_yesterday
		and  market_date <= p_curDate and code = p_nCode limit 0,1;
	RETURN p_result;
END
;;
DELIMITER ;

-- ----------------------------
-- Function structure for MaxDateForCsiCsrcPeDaily
-- ----------------------------
DROP FUNCTION IF EXISTS `MaxDateForCsiCsrcPeDaily`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` FUNCTION `MaxDateForCsiCsrcPeDaily`() RETURNS date
    READS SQL DATA
BEGIN
	#Routine body goes here...

	DECLARE csipedaily_max_date date;

	select max(TradeDate) into csipedaily_max_date from ( select TradeDate from csicsrcpedaily GROUP BY TradeDate ) a  ;

	RETURN csipedaily_max_date;
END
;;
DELIMITER ;

-- ----------------------------
-- Function structure for MaxDateForCsiPeDaily
-- ----------------------------
DROP FUNCTION IF EXISTS `MaxDateForCsiPeDaily`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` FUNCTION `MaxDateForCsiPeDaily`() RETURNS date
    READS SQL DATA
BEGIN
	#Routine body goes here...

	DECLARE csipedaily_max_date date;

	select max(TradeDate) into csipedaily_max_date from ( select TradeDate from csipedaily GROUP BY TradeDate ) a  ;

	RETURN csipedaily_max_date;
END
;;
DELIMITER ;

-- ----------------------------
-- Function structure for MaxDateForMACD
-- ----------------------------
DROP FUNCTION IF EXISTS `MaxDateForMACD`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` FUNCTION `MaxDateForMACD`(`nCode` int) RETURNS date
    READS SQL DATA
BEGIN
	#Routine body goes here...
	DECLARE Done INT DEFAULT 0;
	DECLARE	odt_cur date;
	declare a_total double DEFAULT 0;
	DECLARE isFirst int default 1;
	DECLARE macd_a_total double ;
	DECLARE macd_max_date date;
	
	DECLARE rs cursor for select real_date, A_total_shares from capitalstructure where code = nCode order by real_date desc;
	/* 异常处理 */
	DECLARE CONTINUE HANDLER FOR NOT FOUND SET Done = 1;

	set macd_max_date = NULL;
	/* 打开游标 */
	open rs;
	fetch next from rs into odt_cur, a_total;
label:
	REPEAT
		if Done THEN
			leave label;
		ELSE
			select min(macd.a_total) into macd_a_total from MACD where code = nCode and trade_date >= odt_cur ;
			if macd_a_total is not null THEN
				if macd_a_total = a_total THEN
					if isFirst = 1 THEN
							select max(trade_date) into macd_max_date from MACD where code = nCode
							and ema12 is not null and ema26 is not null 
							and dif is not null and dea is not null;
					ELSE
							set macd_max_date = odt_cur;
					end if;
					leave label;
				end if;
			end if;
			set isFirst = 0;
		end if;
		fetch next from rs into odt_cur, a_total;
	until Done end repeat;

	close rs;
	RETURN macd_max_date;

END
;;
DELIMITER ;

-- ----------------------------
-- Function structure for MaxDateForMCST
-- ----------------------------
DROP FUNCTION IF EXISTS `MaxDateForMCST`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` FUNCTION `MaxDateForMCST`(`nCode` int) RETURNS date
    READS SQL DATA
BEGIN
	#Routine body goes here...
	DECLARE Done INT DEFAULT 0;
	DECLARE p_exit_right_date date ;
	declare p_odt_cur date;
	DECLARE p_max_date date default NULL;
	declare p_odt_last date default null;
	
	/* 按除权日期倒序排列 */
	DECLARE rs cursor for select exit_right_date 
		from (select exit_right_date from 10jqka_bonus where code = nCode 
		union select exit_right_date from cfi_placement_of_share where code = nCode ) a
		order by exit_right_date desc;
	/* 异常处理 */
	DECLARE CONTINUE HANDLER FOR NOT FOUND SET Done = 1;

	/* 打开游标 */
	open rs;
	fetch next from rs into p_odt_cur;

	/* 如果该股票一直未有分红融资记录 */
	if Done THEN
		select max(trade_date) into p_max_date from MCST where code = nCode
			and mcst_price is not null ;
	end if;

	/* 找到 mcst 表中与除权融资记录相符的最近除权日 */
label:
	REPEAT
		if Done THEN
			leave label;
		ELSE
			select min(exit_right_date) into p_exit_right_date from MCST where code = nCode and trade_date >= p_odt_cur ;
			if p_exit_right_date is not null THEN
				if p_exit_right_date >= p_odt_cur THEN
					if p_odt_last is null THEN
							select max(trade_date) into p_max_date from MCST where code = nCode
							and mcst_price is not null and trade_date >= p_odt_cur ;
					ELSE
							select max(trade_date) into p_max_date from MCST where code = nCode
							and mcst_price is not null and trade_date >= p_odt_cur and trade_date< p_odt_last;
					end if;
					leave label;
				end if;
			end if;
			set p_odt_last = p_odt_cur;
		end if;
		fetch next from rs into p_odt_cur;
	until Done end repeat;

	close rs;
	RETURN p_max_date;
END
;;
DELIMITER ;

-- ----------------------------
-- Function structure for RecentFinanceDate
-- ----------------------------
DROP FUNCTION IF EXISTS `RecentFinanceDate`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` FUNCTION `RecentFinanceDate`(`nCode` int,`curDate` date) RETURNS date
    READS SQL DATA
BEGIN
	#Routine body goes here...
	declare baseDate date;
	declare isLast int;
	declare net_profit double;
	
	call NetProfitBelongingToParentCompany(nCode, curDate, baseDate, isLast, net_profit);

	RETURN baseDate;
END
;;
DELIMITER ;

-- ----------------------------
-- Function structure for RecentTradeDateForMACD
-- ----------------------------
DROP FUNCTION IF EXISTS `RecentTradeDateForMACD`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` FUNCTION `RecentTradeDateForMACD`(`nCode` int,`beforeDate` date) RETURNS date
    READS SQL DATA
BEGIN
	#Routine body goes here...
	DECLARE max_date date;
	select max(trade_date) into max_date from macd where code = nCode and trade_date <= beforeDate;
	RETURN max_date;
END
;;
DELIMITER ;

-- ----------------------------
-- Function structure for search_table
-- ----------------------------
DROP FUNCTION IF EXISTS `search_table`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` FUNCTION `search_table`(p_in_tablename varchar(32)) RETURNS varchar(3200) CHARSET utf8
    READS SQL DATA
BEGIN
	#Routine body goes here...
	DECLARE Done INT DEFAULT 0;
	DECLARE p_out_tablename varchar(3200);
	DECLARE p_tablename varchar(32);
	declare rs CURSOR for select table_name from information_schema.tables 
		where table_name like CONCAT('%', p_in_tablename ,'%');
	DECLARE CONTINUE HANDLER FOR NOT FOUND SET Done = 1;

	set p_out_tablename = '';

	open rs;
	fetch next from rs into p_tablename;
label:
	REPEAT
		if Done THEN
			leave label;
		ELSE
			set p_out_tablename = concat(p_out_tablename, p_tablename);
		end if;
		fetch next from rs into p_tablename;

		if Done = 0 THEN
			set p_out_tablename = concat(p_out_tablename, ',');
		end if;
	until Done end repeat;
	close rs;
	
	return p_out_tablename;
END
;;
DELIMITER ;

-- ----------------------------
-- Function structure for test1
-- ----------------------------
DROP FUNCTION IF EXISTS `test1`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` FUNCTION `test1`() RETURNS varchar(32) CHARSET utf8
    READS SQL DATA
BEGIN
	#Routine body goes here...

	RETURN '44444';
END
;;
DELIMITER ;

-- ----------------------------
-- Function structure for YesterdayClosePriceAfterExitRight
-- ----------------------------
DROP FUNCTION IF EXISTS `YesterdayClosePriceAfterExitRight`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` FUNCTION `YesterdayClosePriceAfterExitRight`(`nCode` int,`curDate` date,`closePrice` double, `yesterdayDate` date) RETURNS double
    READS SQL DATA
BEGIN
	#Routine body goes here...
	DECLARE	p_odt_cur date;
	declare p_bonus_cash double DEFAULT 0;
	declare p_bonus_share double DEFAULT 0;
	declare p_reserve_to_common double DEFAULT 0;
	declare p_placement_ratio double DEFAULT 0 ;
	declare p_placement_price double DEFAULT 0;
	declare p_close_price double;
	
	select placement_ratio, placement_price into  p_placement_ratio, p_placement_price 
		from cfi_placement_of_share where  exit_right_date >yesterdayDate 
		and  exit_right_date <= curDate and code = nCode limit 0,1;

	select bonus_share, bonus_cash, reserve_to_common_share into p_bonus_share, p_bonus_cash, p_reserve_to_common  
		from 10jqka_bonus where  exit_right_date >yesterdayDate 
		and exit_right_date <= curDate and code = nCode limit 0,1;
	
	/*股票除权后的昨天收盘参考价格=(股票除权前昨天的收盘价格-每股现金分红+每股配股比例*配股价格)
		/(1+每股转增比例+每股送股比例+每股配股比例)。 */
	set p_close_price = (closePrice - p_bonus_cash/10 + p_placement_price * p_placement_ratio/10)/
			(1 + p_reserve_to_common/10 +  p_bonus_share/10 + p_placement_ratio /10);
	
	RETURN p_close_price;
END
;;
DELIMITER ;

-- ----------------------------
-- Function structure for YesterdayClosePriceAfterExitRight_1
-- ----------------------------
DROP FUNCTION IF EXISTS `YesterdayClosePriceAfterExitRight_1`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` FUNCTION `YesterdayClosePriceAfterExitRight_1`(`nCode` int,`curDate` date,`closePrice` double, `yesterdayDate` date) RETURNS double
    READS SQL DATA
BEGIN
	# 该函数不同与 YesterdayClosePriceAfterExitRight ,时为了计算个股指数，现金红利不做计算
	DECLARE	p_odt_cur date;
	declare p_bonus_cash double DEFAULT 0;
	declare p_bonus_share double DEFAULT 0;
	declare p_reserve_to_common double DEFAULT 0;
	declare p_placement_ratio double DEFAULT 0 ;
	declare p_placement_price double DEFAULT 0;
	declare p_close_price double;
	
	select placement_ratio, placement_price into  p_placement_ratio, p_placement_price 
		from cfi_placement_of_share where  exit_right_date >yesterdayDate 
		and  exit_right_date <= curDate and code = nCode limit 0,1;

	select bonus_share, bonus_cash, reserve_to_common_share into p_bonus_share, p_bonus_cash, p_reserve_to_common  
		from 10jqka_bonus where  exit_right_date >yesterdayDate 
		and exit_right_date <= curDate and code = nCode limit 0,1;
	
	/*股票除权后的昨天收盘参考价格=(股票除权前昨天的收盘价格+每股配股比例*配股价格)
		/(1+每股转增比例+每股送股比例+每股配股比例)。 */
	set p_close_price = (closePrice + p_placement_price * p_placement_ratio/10)/
			(1 + p_reserve_to_common/10 +  p_bonus_share/10 + p_placement_ratio /10);
	
	RETURN p_close_price;
END
;;
DELIMITER ;
