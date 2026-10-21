# Django

> **相关笔记**: [[FLASK|Flask]] | [[../../JAVA/ApacheStruts2|Struts2]] | [[../../PHP/ThinkPHP|ThinkPHP]] | [[../../../../靶场/DVWA/SQLInjection|SQL注入]] | [[../../../../计算机通用/languages/pynote/import|Python笔记]]

Django是一款广为流行的开源web框架，由Python编写，许多网站和app都基于Django开发。Django采用了MTV的框架模式，即模型M，视图V和模版T，使用Django，程序员可以方便、快捷地创建高品质、易维护、数据库驱动的应用程序。而且Django还包含许多功能强大的第三方插件，使得Django具有较强的可扩展性

## SQL注入(CVE-2019-14234)
该漏洞需要开发者使用了JSONField/HStoreField，且用户可控queryset查询时的键名，在键名的位置注入SQL语句。可以利用CVE-2019-14234(SQL注入漏洞)向服务器传送包含CVE-2019-9193(命令执行漏洞)系统命令的SQL语句，从而利用SQL语句，执行任意系统命令。
## SQL 注入(CVE-2021-350424)
Django 组件存在 SQL 注入漏洞，该漏洞是由于对 QuerySet.order_by()中用户提供数据的过滤不足，攻击者可利用该漏洞在未授权的情况下，构造恶意数据执行 SQL 注入攻击，最终造成服务器敏感信息泄露



