一个网页定向采集程序

使用说明:

cwind.sql中为我采集所需要的数据库

数据库配置：
site_info表中配置需要采集的网站首页信息
class_tag(内容分类),list_tag(文章列表),content_tag(文章内容),配置好的id写到对应site_info中
html_filter表中配置常用网页中无用信息过滤规则
remote_mysql中存储用来保存采集结果的目标数据库

运行过程中会产生site_xxxxxxx_log表，存储指定站点的采集记录防止重复采集

