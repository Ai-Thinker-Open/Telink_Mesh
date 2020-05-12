# AT Mesh 透传

## 简介
AT Mesh透固件块支持Mesh自组网，支持手机传送数据到手机。

## 指令集

|序号|指令|功能|备注|
|----|-----|----|----|
|1|AT|测试AT|
|2|ATE|开关回显|暂未实现
|3|AT+GMR|查询固件版本|
|4|AT+RST|重启模组
|5|AT+SLEEP|深度睡眠|暂未实现
|6|AT+RESTORE|恢复出厂设置|暂未实现|
|7|AT+BAUD|查询或设置波特率|暂未实现|
|8|AT+NAME|查询或设置蓝牙广播名称|暂未实现|
|9|AT+MAC|设置或查询模组MAC地址|暂未实现|
|10|AT+STATE|查询蓝牙连接状态|暂未实现
|11|AT+ADDR|查询或设置模块的地址|
|12|AT+MESHNAME|查询或设置模块的网络名称|
|13|AT+MESHPWD|查询或设置模块的网络密码|只能设置，不能查询
|14|AT+SEND|发送数据|
|15|+DATA|收到数据|

## 网络密钥

此Mesh网络采用```MESHNAME```和```MESHPWD```进行加密(可对应于SigMesh中的```NetworkKey```和```ApplicationKey```)，<font color='red'> 只要两个设备的```MESHNAME```和```MESHPWD```相同，即可互发数据。</font>模块出厂默认的  ```MESHNAME```是```at_mesh```, 默认的```MESHPWD```是```123456```, 用户可以在```vendor/commom/user_config.h```文件中修改这些默认值。

模块出厂后，亦可通过AT指令配置```MESHNAME```和```MESHPWD```，指令如下：

设置网络名称：

    AT+MESHNAME=my_mesh123456

设置网络密码：

    AT+MESHPWD=my_password

> 备注：```MESHNAME```和```MESHPWD```最大长度为16个字节。

## 使用示例

查询模块地址：
> 首次上电会生成一个随机地址

    AT+ADDR?
    +ADDR:00a8

设置模块地址:

    AT+ADDR=53

模块间发送数据：(向地址为a8的模块发送五个数据，内容为12345)

    AT+SEND=a8,5,12345

模块向手机发送数据:
> 手机的地址定义为：0xFFF0，只要手机连上Mesh网络中的任一设备，网内的其他设备都可以与手机通信(即使该设备没有直接与手机连接)

    AT+SEND=FFF0,5,12345

手机向模块发送数据：
> 目前手机只能与与之相连的模块通信，暂不支持与网内其他模组通信。