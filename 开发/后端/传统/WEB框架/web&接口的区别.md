# Web框架和接口框架的区别

web框架更侧重于浏览器，而接口框架是任何能够调用的接口都可以使用，不局限于web浏览器。

但是，前后端的本质是接口访问，所以最后的表现形式可能都是前端访问对应的接口，所以接口框架也能够实现简单的浏览器接口搭建功能，只不过两者侧重不同。


## 框架推荐

python:
web: Django(不过太重了，用的也不是很多)、Flask(轻量可用)
接口：FastAPI（很多企业的python相关的接口都是用这个写的）

java:
web: Spring Boot（也可以做页面渲染）
接口：Spring Boot（企业主流，大量用来写REST接口）

Go:
web: Gin
接口：Gin（Go生态写接口最常用；也可选Hertz，体验对标FastAPI）