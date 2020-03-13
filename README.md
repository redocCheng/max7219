# max7219

## 介绍

本软件包是在数码管上应用 MAX7219 的驱动包。

##  支持情况
| 包含设备         | 数码管驱动 |
| ---------------- | -------- |
| **通讯接口**     |          |
| IIC              |         |
| SPI              |  √        |

##  目录结构

| 名称 | 说明 |
| ---- | ---- |
| examples | 例子目录 |
| inc  | 头文件目录 |
| src  | 源代码目录 |
| port | 移植目录 |

##  许可证

max7219 package 遵循 Apache-2.0 许可，详见 `LICENSE` 文件。

## 依赖

- RT-Thread 3.0+
- RT-Thread 4.0+
- SPI 驱动：max7219 使用 SPI 进行数据通讯，需要系统 SPI 驱动支持；

## 获取软件包

使用 max7219 package 需要在 RT-Thread 的包管理器中选择它，具体路径如下：

```shell
RT-Thread online packages
    peripheral libraries and drivers --->
        [*] MAX7219: for the digital tube
            (1)   the chips of digital tubes
            (spi1) spi bus name
            (spi10) spi device name
                Version (v1.0.0)  --->
```

然后让 RT-Thread 的包管理器自动更新，或者使用 `pkgs --update` 命令更新包到 BSP 中。


## 设置参数

配置时需要修改项目中的 `max7219_cfg.h` 文件，只有一项 `MAX7219_CHIPS_SCAN_NUMBER_TABLE` 需要手动设置





## API说明

1. 消隐数码管

```C
int max7219_clear(uint16_t dig)
```

|参数|注释|
|----|----|
|dig|选择的数码管|

|返回|注释|
|----|----|
|RT_EOK|正常|
|-RT_ERROR|异常|

2. 消隐全部数码管

```C
int max7219_clear_all(void)
```

|参数|注释|
|----|----|
|void|没有|
|返回|注释|
|----|----|
|RT_EOK|正常|
|-RT_ERROR|异常|

3. 向数码管寄存器写入数据

```C
int max7219_write(uint16_t dig, uint8_t data)
```

|参数|注释|
|----|----|
|dig|选择的数码管|
|data|数据|
|返回|注释|
|----|----|
|RT_EOK|正常|
|-RT_ERROR|异常|

4. 向数码管寄存器写入数字

```C
int max7219_write_num(uint16_t dig, uint8_t num)
```

|参数|注释|
|----|----|
|dig|选择的数码管|
|num|数字（范围0-0xf） 输入 0 就显示 0|
|返回|注释|
|----|----|
|RT_EOK|正常|
|-RT_ERROR|异常|


5. 亮度设置

```C
int max7219_intensity_set(uint8_t value)
```

|参数|注释|
|----|----|
|value|设置亮度（0-0x0f）|

|返回|注释|
|----|----|
|RT_EOK|成功|
|!=RT_OK|异常|


## 示例

使用示例在 [examples](./examples) 下。

## 芯片手册

芯片手册在 [docs](./docs) 下。

## 注意事项

1. 参数中的dig和硬件设计的数码管顺序有关。
2. 支持非编码模式和全编码模式，建议只用非编码模式，所以不加编码配置选项，可以在[drv_max7219.h](./inc/drv_max7219.h)的 `MAX7219_INFO_DEFAULT` 选择配置。
3. 你还可以搞自定义字符，不过编码要去手册里面找，比如 写出 `r` 的编码为 0x05。
4. 亮度只支持 0 到 f。
5. 还没有用到点阵的驱动，后续会在这块优化更新。

## 5、联系方式 & 感谢

* 维护：redoccheng
* 主页：<https://github.com/redocCheng/max7219>
* 邮箱：<619675912@qq.com>
