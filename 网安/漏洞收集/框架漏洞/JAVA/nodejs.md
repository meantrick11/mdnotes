# node.js

> **相关笔记**: [[ApacheStruts2|Struts2]] | [[Spring|Spring]] | [[../../PHP/ThinkPHP|ThinkPHP]] | [[../../../../计算机通用/languages/JavaScript/javaScript|JavaScript]]

node.js是服务端运行javascript的运行环境，可以做：
- 写网站后端接口（Web 服务）
- 写爬虫、脚本、工具
- 搭建服务器、内网服务、接口靶场
- 开发小程序、桌面程序

## 文件包含（目录穿越
原因是Node.js 8.5.0 对目录进行normalize操作时出现了逻辑错误，导致向上层跳跃的时候（如…/…/…/…/…/…/etc/passwd），在中间位置增加foo/…/（如…/…/…/foo/…/…/…/…/etc/passwd），即可使normalize返回/etc/passwd，但实际上正确结果应该是…/…/…/…/…/…/etc/passwd。

express这类web框架，通常会提供了静态文件服务器的功能，这些功能依赖于normalize函数。比如，express在判断path是否超出静态目录范围时，就用到了normalize函数，上述BUG导致normalize函数返回错误结果导致绕过了检查，造成任意文件读取漏洞。

当然，normalize的BUG可以影响的绝非仅有express，更有待深入挖掘。不过因为这个BUG是node 8.5.0 中引入的，在 8.6 中就进行了修复，所以影响范围有限

## systeminformation模块RCE(CVE-2021-21315)

Node.js-systeminformation是用于获取各种系统信息的Node.js模块,在存在命令注入漏洞的版本中，攻击者可以通过未过滤的参数中注入payload执行系统命令。