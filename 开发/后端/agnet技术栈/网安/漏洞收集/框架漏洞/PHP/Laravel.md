# Laravel

> **相关笔记**: [[ThinkPHP|ThinkPHP]] | [[../../JAVA/ApacheStruts2|Struts2]] | [[../../PYTHON/Django|Django]] | [[../../../../靶场/DVWA/Fileupload|文件上传]] | [[../../../../计算机通用/languages/PHP/phplearn|PHP笔记]]

## 远程代码执行（CVE-2021-3129)

当Laravel开启了Debug模式时，由于Laravel自带的Ignition 组件对file_get_contents()和file_put_contents()函数的不安全使用，攻击者可以通过发起恶意请求，构造恶意Log文件等方式触发Phar反序列化，最终造成远程代码执行。


## 文件包含（CVE-2018-17246)


