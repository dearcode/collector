-- MySQL dump 10.14 Distrib 5.5 .37 - MariaDB, for Linux
	(x86_64)
-- -- Host: localhost Database:cwind-- --
	    -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --Server
	    version 5.5 .37 - MariaDB
/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */ ;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */ ;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */ ;
/*!40101 SET NAMES utf8 */ ;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */ ;
/*!40103 SET TIME_ZONE='+00:00' */ ;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */ ;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */
    ;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */ ;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */ ;

-- --Table structure for table
	`class_tag ` --DROP TABLE IF EXISTS ` class_tag `;
/*!40101 SET @saved_cs_client     = @@character_set_client */ ;
/*!40101 SET character_set_client = utf8 */ ;
CREATE TABLE ` class_tag ` (`class_id ` int (11) unsigned NOT NULL
			    AUTO_INCREMENT,
			    `part_begin ` varchar(256) DEFAULT NULL,
			    `part_end ` varchar(256) DEFAULT NULL,
			    PRIMARY KEY(`class_id `)
)ENGINE = MyISAM AUTO_INCREMENT = 6 DEFAULT CHARSET = utf8;
/*!40101 SET character_set_client = @saved_cs_client */ ;

-- --Dumping data for table
	`class_tag ` --LOCK TABLES ` class_tag ` WRITE;
/*!40000 ALTER TABLE `class_tag` DISABLE KEYS */ ;
INSERT INTO ` class_tag ` VALUES(1,
				 '<table width=\"100%\" border=\"0\" cellpadding=\"4\" cellspacing=\"0\" class=\"left_4bh\" align=\"center\" id=\"classlist\" style=\"display:block\">',
				 '<table width=\"50\" height=\"6\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">'),
    (2, '<div><h2><a href=\"/Qq.asp\">QQ表情大全</a></h2>',
     '</div><div class=\"clear\"></div></div>'), (3,
						  '<h2><a href=\"http://gaoxiao.jokeji.cn/list/list_1.htm\">搞笑图片大全</a></h2>',
						  '</ul></div><div class=\"clear\"></div></div>'),
    (4, '<div class=\"hrTitle lan14b\">笑话分类</div>',
     '<div class=\"hrT1 lan14b mt6\">热门关键词</div>'), (5,
							       '<a href=\"http://www.nixiba.com/hot/month/\">一个月内</a>',
							       '</ul>');
/*!40000 ALTER TABLE `class_tag` ENABLE KEYS */ ;
UNLOCK TABLES;

-- --Table structure for table
	`content_tag ` --DROP TABLE IF EXISTS ` content_tag `;
/*!40101 SET @saved_cs_client     = @@character_set_client */ ;
/*!40101 SET character_set_client = utf8 */ ;
CREATE TABLE ` content_tag ` (`content_id ` bigint(20) unsigned NOT NULL
			      AUTO_INCREMENT,
			      `part_begin ` varchar(1024) NOT NULL,
			      `part_end ` varchar(1024) DEFAULT NULL,
			      `href_begin ` varchar(1024) DEFAULT NULL,
			      `href_end ` varchar(1024) DEFAULT NULL,
			      `date_begin ` varchar(1024) DEFAULT NULL,
			      `date_end ` varchar(1024) DEFAULT NULL,
			      `pic_begin ` varchar(1024) DEFAULT NULL,
			      `pic_end ` varchar(1024) DEFAULT NULL,
			      `caption_begin ` varchar(1024) DEFAULT NULL,
			      `caption_end ` varchar(1024) DEFAULT NULL,
			      `desc_end ` varchar(1024) DEFAULT NULL,
			      `desc_begin ` varchar(1024) DEFAULT NULL,
			      `content_begin ` varchar(1024) DEFAULT NULL,
			      `content_end ` varchar(1024) DEFAULT NULL,
			      PRIMARY KEY(`content_id `)
)ENGINE = MyISAM AUTO_INCREMENT = 6 DEFAULT CHARSET = utf8;
/*!40101 SET character_set_client = @saved_cs_client */ ;

-- --Dumping data for table
	`content_tag ` --LOCK TABLES ` content_tag ` WRITE;
/*!40000 ALTER TABLE `content_tag` DISABLE KEYS */ ;
INSERT INTO ` content_tag ` VALUES(1, '<div class=\"main margin10\">',
				   '<div class=\"bbs_title\" id=\"plcontent\">评论内容</div>',
				   '0', '0', '<li><i>发布时间：',
				   '</i></li>', '0', '0', '-> ', '</h',
				   '<div class=\"clear\"></div>',
				   '<span id=\"text110\">', '0', '0'), (2,
									'<div class=\"pic_pview\">',
									'<div class=\"pviev_joke\">',
									'0',
									'0',
									'<div class=\"pic_fx\"><span>',
									'</span>',
									'0',
									'0',
									'</a> ->',
									'</h',
									'0',
									'0',
									'<ul>',
									'</ul>'),
    (3, '<div class=\"photo_view_title\">', '<div class=\"photo_view_list1\">',
     '0', '0', '0', '0', '0', '0', '<span class=\"photo_view_t\">', '</span>',
     '0', '0', '<ul>', '</ul>'), (4, '<div class=\"location\">',
				  '<div class=\"page hui12\" style=\"padding:0\">',
				  '0', '0', '0', '0', '0', '0', '<h1>', '</h1>',
				  '0', '0',
				  '<div class=\"lastC\" style=\"word-wrap:break-word;word-break:break-all\">',
				  '<div class=\"lastVote\">'), (5,
								'<div class=\"main\">',
								'<div class=\"comment\"></div>',
								'0', '0',
								'<meta itemprop=\"datePublished\" content=\"',
								'\"', '0', '0',
								'<div class=\"topic\"><h1 itemprop=\"name\">',
								'</h1></div>',
								'0', '0',
								'<div class=\"content\">',
								'<div class=\"share\">');
/*!40000 ALTER TABLE `content_tag` ENABLE KEYS */ ;
UNLOCK TABLES;

-- --Table structure for table
	`cwind_log ` --DROP TABLE IF EXISTS ` cwind_log `;
/*!40101 SET @saved_cs_client     = @@character_set_client */ ;
/*!40101 SET character_set_client = utf8 */ ;
CREATE TABLE ` cwind_log ` (`id ` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
			    `url_crc32 ` bigint(33) NOT NULL,
			    `url ` varchar(128) NOT NULL,
			    `type ` int (10) unsigned NOT NULL COMMENT
			    '类型，1：列表页，2：内容页',
			    `fetch_date ` int (11) unsigned NOT NULL DEFAULT '0'
			    COMMENT '最后更新日期',
			    `status ` int (11) unsigned NOT NULL COMMENT
			    '1:采集成功 2:采集失败了',
			    PRIMARY KEY(`id `),
			    UNIQUE KEY ` uniq_url ` (`url_crc32 `,
						     `url `) USING BTREE,
			    KEY ` idx_type ` (`type `) USING BTREE)ENGINE =
    MyISAM AUTO_INCREMENT = 3639 DEFAULT CHARSET = utf8;
/*!40101 SET character_set_client = @saved_cs_client */ ;

-- --Dumping data for table
	`cwind_log ` --LOCK TABLES ` cwind_log ` WRITE;
/*!40000 ALTER TABLE `cwind_log` DISABLE KEYS */ ;
/*!40000 ALTER TABLE `cwind_log` ENABLE KEYS */ ;
UNLOCK TABLES;

-- --Table structure for table
	`html_filter ` --DROP TABLE IF EXISTS ` html_filter `;
/*!40101 SET @saved_cs_client     = @@character_set_client */ ;
/*!40101 SET character_set_client = utf8 */ ;
CREATE TABLE ` html_filter ` (`id ` int (10) unsigned NOT NULL AUTO_INCREMENT,
			      `key1 ` varchar(1024) NOT NULL,
			      `key2 ` varchar(1024) NOT NULL,
			      `group_id ` int (10) unsigned DEFAULT NULL,
			      `type ` smallint(2) unsigned NOT NULL DEFAULT '0'
			      COMMENT
			      '类型1是用key2替换key1，0是删除两个key之间内容',
			      PRIMARY KEY(`id `)
)ENGINE = MyISAM AUTO_INCREMENT = 8 DEFAULT CHARSET = utf8;
/*!40101 SET character_set_client = @saved_cs_client */ ;

-- --Dumping data for table
	`html_filter ` --LOCK TABLES ` html_filter ` WRITE;
/*!40000 ALTER TABLE `html_filter` DISABLE KEYS */ ;
INSERT INTO ` html_filter ` VALUES(1, '<span id=\"text110\">', ' ', 1, 1), (2,
									    '</span><br />',
									    ' ',
									    1,
									    1),
    (3, 'alt=\'点右键可加到QQ表情里\'', ' ', 2, 1), (4, '(', ')', 2,
							      0), (5,
								   'class=\"',
								   '\"', 2, 0),
    (6, 'style=\"', '\"', 2, 0), (7, 'style=\"', '\"', 4, 0);
/*!40000 ALTER TABLE `html_filter` ENABLE KEYS */ ;
UNLOCK TABLES;

-- --Table structure for table
	`list_tag ` --DROP TABLE IF EXISTS ` list_tag `;
/*!40101 SET @saved_cs_client     = @@character_set_client */ ;
/*!40101 SET character_set_client = utf8 */ ;
CREATE TABLE ` list_tag ` (`list_id ` bigint(20) unsigned NOT NULL
			   AUTO_INCREMENT,
			   `list_part_begin ` varchar(256) NOT NULL,
			   `list_part_end ` varchar(256) NOT NULL,
			   `href_part_begin ` varchar(256) NOT NULL,
			   `href_part_end ` varchar(256) NOT NULL,
			   `href_begin ` varchar(256) NOT NULL,
			   `href_end ` varchar(256) NOT NULL,
			   `caption_begin ` varchar(256) NOT NULL,
			   `caption_end ` varchar(256) NOT NULL,
			   `page_part_begin ` varchar(256) NOT NULL,
			   `page_part_end ` varchar(256) NOT NULL,
			   `next_caption ` varchar(64) NOT NULL DEFAULT '0',
			   `update_time ` int (10) unsigned NOT NULL DEFAULT
			   '0', PRIMARY KEY(`list_id `)
)ENGINE = MyISAM AUTO_INCREMENT = 6 DEFAULT CHARSET = utf8;
/*!40101 SET character_set_client = @saved_cs_client */ ;

-- --Dumping data for table
	`list_tag ` --LOCK TABLES ` list_tag ` WRITE;
/*!40000 ALTER TABLE `list_tag` DISABLE KEYS */ ;
INSERT INTO ` list_tag ` VALUES(1,
				'<table width=\"100%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" class=\"b3\">',
				'<table width=\"300\" height=\"250\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\" style=\" margin-bottom:4px;\">',
				'<td width=\"397\" align=\"left\">', '</td>',
				'<li><b>', '</b><span>',
				'<div class=\"list_title\">', '</div>',
				'<tr align=\"center\" height=\"20\">',
				'</tr></form></table>', '', 0), (2,
								 '<div class=\"photo_info\">',
								 '<div class=\"photoi_right\">',
								 '<ul>',
								 '</ul>',
								 '<span>',
								 '</span>', '',
								 '',
								 '<div class=\"next_page\">',
								 '<div class=\"clear\"></div>',
								 '0', 0), (3,
									   '<div class=\"listpic\">',
									   '<div class=\"right\">',
									   '<b></b>',
									   '<div class=\"clear\"></div>',
									   '<div class=\"list_list\">',
									   '</div>',
									   '',
									   '',
									   '<div class=\"tags_page\">',
									   '</div>',
									   '0',
									   0),
    (4, '<div class=\"lC\">', '<div class=\"hrT\">', '', '',
     '<div class=\"left_font\">', '<div class=\"lcListR\">', '', '',
     '<div class=\"page hui12\" >',
     '<script language=\"javascript\" type=\"text/javascript\">', '0', 0), (5,
									    '<div class=\"main\">',
									    '<div class=\"rightbox\">',
									    '',
									    '',
									    '<div class=\"topic\">',
									    '</div>',
									    '',
									    '',
									    '<div class=\"page\">',
									    '</div>',
									    '0',
									    0);
/*!40000 ALTER TABLE `list_tag` ENABLE KEYS */ ;
UNLOCK TABLES;

-- --Table structure for table
	`remote_mysql ` --DROP TABLE IF EXISTS ` remote_mysql `;
/*!40101 SET @saved_cs_client     = @@character_set_client */ ;
/*!40101 SET character_set_client = utf8 */ ;
CREATE TABLE ` remote_mysql ` (`remote_mysql_id ` int (11) unsigned NOT NULL
			       AUTO_INCREMENT, `host ` varchar(64) NOT NULL,
			       `port ` int (11) NOT NULL,
			       `username ` varchar(64) NOT NULL,
			       `password ` varchar(64) NOT NULL,
			       `dbname ` varchar(64) NOT NULL,
			       PRIMARY KEY(`remote_mysql_id `)
)ENGINE = MyISAM AUTO_INCREMENT = 2 DEFAULT CHARSET = utf8;
/*!40101 SET character_set_client = @saved_cs_client */ ;

-- --Dumping data for table
	`remote_mysql ` --LOCK TABLES ` remote_mysql ` WRITE;
/*!40000 ALTER TABLE `remote_mysql` DISABLE KEYS */ ;
INSERT INTO ` remote_mysql ` VALUES(1, '127.0.0.1', 3306, 'root', 'mc123456',
				    'kxbao');
/*!40000 ALTER TABLE `remote_mysql` ENABLE KEYS */ ;
UNLOCK TABLES;

-- --Table structure for table
	`site_5_log ` --DROP TABLE IF EXISTS ` site_5_log `;
/*!40101 SET @saved_cs_client     = @@character_set_client */ ;
/*!40101 SET character_set_client = utf8 */ ;
CREATE TABLE ` site_5_log ` (`id ` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
			     `url_crc32 ` bigint(33) NOT NULL,
			     `url ` varchar(128) NOT NULL,
			     `type ` int (10) unsigned NOT NULL COMMENT
			     '类型，1：列表页，2：内容页',
			     `fetch_date ` int (11) unsigned NOT NULL DEFAULT
			     '0' COMMENT '最后更新日期',
			     `status ` int (11) unsigned NOT NULL COMMENT
			     '1:采集成功 2:采集失败了',
			     PRIMARY KEY(`id `),
			     UNIQUE KEY ` uniq_url ` (`url_crc32 `,
						      `url `) USING BTREE,
			     KEY ` idx_type ` (`type `) USING BTREE)ENGINE =
    MyISAM AUTO_INCREMENT = 3214 DEFAULT CHARSET = utf8;
/*!40101 SET character_set_client = @saved_cs_client */ ;

-- --Dumping data for table
	`site_5_log ` --LOCK TABLES ` site_5_log 