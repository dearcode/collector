-- MySQL dump 10.16  Distrib 10.1.21-MariaDB, for Linux (x86_64)
--
-- Host: 127.0.0.1    Database: 127.0.0.1
-- ------------------------------------------------------
-- Server version	10.1.21-MariaDB

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `class_tag`
--

DROP TABLE IF EXISTS `class_tag`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `class_tag` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `part_begin` varchar(256) NOT NULL DEFAULT '',
  `part_end` varchar(256) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=6 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `content_tag`
--

DROP TABLE IF EXISTS `content_tag`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `content_tag` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `part_begin` varchar(256) NOT NULL,
  `part_end` varchar(256) NOT NULL,
  `href_begin` varchar(256) NOT NULL,
  `href_end` varchar(256) NOT NULL,
  `date_begin` varchar(256) NOT NULL,
  `date_end` varchar(256) NOT NULL,
  `pic_begin` varchar(256) NOT NULL,
  `pic_end` varchar(256) NOT NULL,
  `caption_begin` varchar(256) NOT NULL,
  `caption_end` varchar(256) NOT NULL,
  `desc_end` varchar(256) NOT NULL,
  `desc_begin` varchar(256) NOT NULL,
  `content_begin` varchar(256) NOT NULL,
  `content_end` varchar(256) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=6 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cwind_log`
--

DROP TABLE IF EXISTS `cwind_log`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cwind_log` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `url_crc32` bigint(33) NOT NULL,
  `url` varchar(128) NOT NULL,
  `type` int(10) unsigned NOT NULL COMMENT '类型，1：列表页，2：内容页',
  `fetch_date` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '最后更新日期',
  `status` int(11) unsigned NOT NULL COMMENT '1:采集成功 2:采集失败了',
  PRIMARY KEY (`id`),
  UNIQUE KEY `uniq_url` (`url_crc32`,`url`) USING BTREE,
  KEY `idx_type` (`type`) USING BTREE
) ENGINE=MyISAM AUTO_INCREMENT=3639 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `html_filter`
--

DROP TABLE IF EXISTS `html_filter`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `html_filter` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `key1` varchar(1024) NOT NULL,
  `key2` varchar(1024) NOT NULL,
  `group_id` int(10) unsigned DEFAULT NULL,
  `type` smallint(2) unsigned NOT NULL DEFAULT '0' COMMENT '类型1是用key2替换key1，0是删除两个key之间内容',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=8 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `list_tag`
--

DROP TABLE IF EXISTS `list_tag`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `list_tag` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `list_part_begin` varchar(256) NOT NULL,
  `list_part_end` varchar(256) NOT NULL,
  `href_part_begin` varchar(256) NOT NULL,
  `href_part_end` varchar(256) NOT NULL,
  `href_begin` varchar(256) NOT NULL,
  `href_end` varchar(256) NOT NULL,
  `caption_begin` varchar(256) NOT NULL,
  `caption_end` varchar(256) NOT NULL,
  `page_part_begin` varchar(256) NOT NULL,
  `page_part_end` varchar(256) NOT NULL,
  `next_caption` varchar(64) NOT NULL DEFAULT '0',
  `update_time` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=6 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `remote_mysql`
--

DROP TABLE IF EXISTS `remote_mysql`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `remote_mysql` (
  `remote_mysql_id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `host` varchar(64) NOT NULL,
  `port` int(11) NOT NULL,
  `username` varchar(64) NOT NULL,
  `password` varchar(64) NOT NULL,
  `dbname` varchar(64) NOT NULL,
  PRIMARY KEY (`remote_mysql_id`)
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `site_5_log`
--

DROP TABLE IF EXISTS `site_5_log`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `site_5_log` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `url_crc32` bigint(33) NOT NULL,
  `url` varchar(128) NOT NULL,
  `type` int(10) unsigned NOT NULL COMMENT '类型，1：列表页，2：内容页',
  `fetch_date` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '最后更新日期',
  `status` int(11) unsigned NOT NULL COMMENT '1:采集成功 2:采集失败了',
  PRIMARY KEY (`id`),
  UNIQUE KEY `uniq_url` (`url_crc32`,`url`) USING BTREE,
  KEY `idx_type` (`type`) USING BTREE
) ENGINE=MyISAM AUTO_INCREMENT=3214 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `site_info`
--

DROP TABLE IF EXISTS `site_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `site_info` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `status` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `name` varchar(64) NOT NULL,
  `url` varchar(128) NOT NULL,
  `md5` varchar(64) NOT NULL DEFAULT '',
  `level` int(10) unsigned NOT NULL DEFAULT '0',
  `hot_part_begin` varchar(256) NOT NULL DEFAULT '',
  `hot_part_end` varchar(256) NOT NULL,
  `list_tag_id` bigint(20) unsigned NOT NULL,
  `class_tag_id` bigint(20) unsigned NOT NULL,
  `content_tag_id` bigint(20) unsigned NOT NULL,
  `html_filter_id` bigint(20) unsigned NOT NULL,
  `remote_mysql_id` bigint(20) unsigned NOT NULL,
  `ctime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `mtime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=6 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2017-02-06 18:19:05
