Git的分支开发管理根据不同的该公司有所不同，比较通用的环境分为dev、test、release、master分支分别对应着开发、测试、预发布、生产环境，当然除了上述主要的分支外，还可能有基于某个产品版本的多人协作的各自feature分支，每次开发功能等等都在各自分支上开发，然后进行PR，需要不断跟踪当前开发的dev分支，如果生产环境分支出现bug等，那么需要新建hotfix分支，详细的教程可以参考：[git原理与实践--企业版](https://cloud.tencent.com/developer/article/2617318)

