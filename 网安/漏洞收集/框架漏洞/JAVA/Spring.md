# Spring

> **相关笔记**: [[ApacheStruts2|Struts2]] | [[../PHP/ThinkPHP|ThinkPHP]] | [[../PYTHON/Django|Django]] | [[../PYTHON/FLASK|Flask]]

## 介绍

## 远程代码执行（CVE-2017-4971)

Spring WebFlow 是一个适用于开发基于流程的应用程序的框架（如购物逻辑），可以将流程的定义和实现流程行为的类和视图分离开来。在其 2.4.x 版本中，如果我们控制了数据绑定时的field，将导致一个SpEL表达式注入漏洞，最终造成任意命令执行。

## SPEL表达式注入(CVE-2018-1273)
Spring Data是一个用于简化数据库访问，并支持云服务的开源框架，Spring Data Commons是Spring Data下所有子项目共享的基础框架。Spring Data Commons 在2.0.5及以前版本中，存在一处SpEL表达式注入漏洞，攻击者可以注入恶意SpEL表达式以执行任意命令。
