# windows 11 openclaw agent build guide

1. 步骤一：管理员身份运行powershell
> 1. 点击开始按钮
> 2. 在搜索栏输入powershell
> ![搜索powershell](./pictures/powershell1.png)
> 3. 右键powershell并选择以管理员身份运行
> ![以管理员身份运行](./pictures/powershell2.png)
> 4. 点击“是”选项
> ![点击“是”选项](./pictures/powershell3.png)
> 5. 等待并进入powershell界面
> ![等待并进入powershell界面](./pictures/powershell4.png)

2. 步骤二：安装node.js
> 1. 在powershell中输入`winget install OpenJS.NodeJS.LTS`命令并enter
> ![安装node.js](./pictures/installnodejs1.png)
> 2. 遇到卡壳输入Y键并enter，继续安装
> ![继续安装](./pictures/installnodejs2.png)
> ![继续安装](./pictures/installnodejs3.png)
> 3. 等待安装完成
> ![等待安装完成](./pictures/installnodejs4.png)
> 4. 按照步骤一重新开启新的powershell窗口刷新环境变量，并输入`node -v`命令查看node.js版本
> ![查看node.js版本](./pictures/installnodejs5.png)

3. 步骤三：安装open claw

> 1. 在powershell中输入`npm install -g openclaw@latest`命令并enter

> ![安装open claw](./pictures/installopenclaw1.png)
> >可能会出现一下情况：
> >![安装open claw](./pictures/npm无法加载.png)
> >解决方案：
> > 1. 在powershell中输入`Set-ExecutionPolicy RemoteSigned -Scope CurrentUser`命令并enter
> > ![设置执行策略](./pictures/设置执行策略.png)
> > 2. 遇到卡壳，输入Y键并enter
> > ![输入Y键并enter](./pictures/设置执行策略2.png)
> > 可能会出现一下问题：
> > ![无git](./pictures/无git.png)
> > 解决方案：
> > 1. 在powershell中输入`winget install --id Git.Git -e --source winget`命令并enter,安装git
> > ![安装git](./pictures/安装git.png)
> > ![安装git](./pictures/安装git2.png)

> > 2. 安装编译工具和核心工具
> > 输入一下命令`
Invoke-WebRequest -Uri "https://aka.ms/vs/17/release/vs_BuildTools.exe" -OutFile "$env:TEMP\vs_BuildTools.exe"
Start-Process -FilePath "$env:TEMP\vs_BuildTools.exe" -ArgumentList "--quiet --wait --norestart --add Microsoft.VisualStudio.Workload.VCTools --add Microsoft.VisualStudio.Component.Windows10SDK.20348" -NoNewWindow -Wait
Remove-Item "$env:TEMP\vs_BuildTools.exe"`来安装必要工具
> > ![安装编译工具和核心工具](./pictures/编译工具1.png)
> > 继续安装python,输入`winget install --id Python.Python.3.12 -e --source winget`命令并enter
> > ![安装编译工具和核心工具](./pictures/安装python.png)
> > 继续安装 CMake，输入`winget install --id Kitware.CMake -e --source winget`命令并enter
> > ![安装编译工具和核心工具](./pictures/安装cmake.png)
> > 配置npm使用Visual Studio 和Python
> > 3. 同步骤一，重启powershell

> 2. 继续open claw的安装并持续等待
> ![继续安装open claw](./pictures/installopenclaw2.png)

> 3. 安装完成后，会有如下提示：
> ![安装完成](./pictures/installopenclaw3.png)

4. 步骤四：开启open claw 并配置

> 1. 在powershell中输入`openclaw onboard`命令并enter
> ![配置open claw](./pictures/openclaw1.png)
> 2. 安全确认界面，用←键选择Yes并enter
> ![安全确认界面](./pictures/openclaw2.png)
> 3. 选择模式QuickStart并enter
> ![选择模式QuickStart](./pictures/openclaw3.png)
> 4. 选择AI大模型(如果有其他ai的api密钥/国外大多数ai的token是有限的，且由于国内网络限制，最好选择千问Qwen,虽然也有token限制，但对国内用户正常使用还是足够),用下键选择"Qwen"并enter
> ![选择AI大模型](./pictures/openclaw4.png)
> 5. 自动进入Qwen网页并注册或者登录你的Qwen账号
> ![自动进入Qwen网页](./pictures/openclaw5.png)
> 6. 注册界面，输入你的Qwen账号信息并enter
> ![注册界面](./pictures/openclaw6.png)
> 7. 邮箱确认界面，一般会收到一封确认邮件，点击链接激活，然后退出这个界面
> ![邮箱确认界面](./pictures/openclaw7.png)
> 8. 最后确认登录界面，点击确认
> ![最后确认登录界面](./pictures/openclaw8.png)
> 9. 登录成功后，会自动进入open claw的界面,此时返回powershell界面
> ![登录成功](./pictures/openclaw9.png)
> 10. 返回powershell界面并选择“Keep current"默认选项
> ![返回powershell界面](./pictures/openclaw10.png)
> 11. 选择channel(就是可以实时通信设备，后续也可以添加，建议选择跳过，而使用它自带的ui界面或者终端使用).选择"Skip for now"跳过
> ![选择channel](./pictures/openclaw11.png)
> 12. 选择搜索引擎(同理，相应的搜索引擎需要对应的api大多数要付费)，建议选择"Skip for now"跳过
> ![选择搜索引擎](./pictures/openclaw12.png)
> 13. 选择技能（对应需要下载相应的包，不如选择github，Gemini等，同理，大多数也需要对应的api接口），使用空格选择，选择完成之后enter，建议选择"Skip for now"跳过
> ![选择技能](./pictures/openclaw13.png)
> > 倘若误选了，对应需要api的一律选"No"就可以了
> > ![选择技能](./pictures/openclaw14.png)
> 14. 选择Hooks，高级自动化程序的接口（同上任然需要api），选择"Skip for now"跳过
> ![选择Hooks](./pictures/openclaw15.png)
> 15. 孵化你的open claw(简单讲就是设置喜好，根据需求设置)，这个时候就成功安装好了open claw，选择"Hatch in TUI"进入tui界面
> ![孵化你的open claw](./pictures/openclaw16.png)
> 16. 进入tui界面，然后就可以开始对话了，比如
> ![tui界面](./pictures/openclaw18.png)
> 17. 这个时候就已经配置好了，接下来就是使用了。可以直接退出，后面再启动就可以了

**接下来是两个启动方法**：
1. 启动方法一：
>  按照步骤一打开powershell，然后输入`openclaw gateway start`,打开前导程序
> ![启动方法一](./pictures/启动方法一.png)
>  然后在powershell中输入`openclaw tui`,启动tui界面就可以开始使用了
> ![启动方法一](./pictures/启动方法一2.png)

2. 启动方法二：

>  按照步骤一打开powershell，然后输入`openclaw dashboard`,如下图
> ![启动方法二](./pictures/启动方法二1.png)
> 最后电脑会自动打开一个open claw自己创建的网页，然后就可以开始使用了，如下图
![启动方法二](./pictures/启动方法二2.png)

