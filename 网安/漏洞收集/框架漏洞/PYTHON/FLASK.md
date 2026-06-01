# Flask

> **相关笔记**: [[Django|Django]] | [[../../JAVA/ApacheStruts2|Struts2]] | [[../../PHP/ThinkPHP|ThinkPHP]] | [[../../../../靶场/DVWA/CommandInjection|命令注入]] | [[../../../../计算机通用/languages/pynote/import|Python笔记]]

## 介绍

Flask 是一个 web 框架。也就是说 Flask 为你提供工具，库和技术来允许你构建一个 web 应用程序。这个 wdb 应用程序可以使一些 web 页面、博客、wiki、基于 web 的日历应用或商业网站。
Flask 属于微框架（micro-framework）这一类别，微架构通常是很小的不依赖于外部库的框架。这既有优点也有缺点，优点是框架很轻量，更新时依赖少，并且专注安全方面的 bug，缺点是，你不得不自己做更多的工作，或通过添加插件增加自己的依赖列表。Flask 的依赖如下：

Werkzeug 一个 WSGI 工具包
jinja2 模板引擎

## Jinja2五负端的SSTi模板注入

Jinja 2是一种面向Python的现代和设计友好的模板语言，它是以Django的模板为模型的
Jinja2 是 Flask 框架的一部分。Jinja2 会把模板参数提供的相应的值替换了 {{…}} 块
Jinja2 模板同样支持控制语句，像在 {%…%} 块中

