# Struts2 -- Java

> **相关笔记**: [[../Spring|Spring]] | [[../PHP/ThinkPHP|ThinkPHP]] | [[../PYTHON/Django|Django]] | [[../PYTHON/FLASK|Flask]] | [[../../../../靶场/DVWA/CommandInjection|命令注入]] | [[../../../../计算机通用/languages/JavaScript/javaScript|Java相关]]

## 远程代码执行

Apache Struts2框架是一个用于开发Java EE网络应用程序的Web框架。Apache Struts于2020年12月08日披露 S2-061 Struts 远程代码执行漏洞(CVE-2020-17530)，在使用某些tag等情况下可能存在OGNL表达式注入漏洞，从而造成远程代码执行，风险极大，S2-061是对S2-059漏洞修复后的绕过。

该漏洞由于对CVE-2020-17530的修复不完整造成的，CVE-2020-17530漏洞是由于Struts2 会对某些标签属性(比如id) 的属性值进行二次表达式解析，因此当这些标签属性中使用了 %{x} 且 其中x 的值用户可控时，用户再传入一个 %{payload} 即可造成OGNL表达式执行。在CVE-2021-31805漏洞中，仍然存在部分标签属性会造成攻击者恶意构造的OGNL表达式执行，导致远程代码执行。
