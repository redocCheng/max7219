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
| doc |  文档 |

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


配置时需要修改项目中的 `max7219_cfg.h` 文件，和配置 menuconfig。

### 芯片数量：
`MAX7219_CHIPS_NUMBER`  可通过 menuconfig 中的 `(1)   the chips of digital tubes` 选项设置，


也可以直接在 `max7219_cfg.h` cfg 文件中设置，一片就设置 1。
```C
#define MAX7219_CHIPS_NUMBER    1
```


### 芯片的共极配置
`MAX7219_CHIPS_SCAN_NUMBER_TABLE` 配置单个芯片的共极，以便后续扫描计算；
每个max7219可支持8个数码管，所以这里采用二进制从左到右进行表示，既 `0B11111111`，有为 1 ，没有为 0。
比如挂了一个芯片，用了dig 1到6,应设置 `0B11111100` ，对应十六进制 0xfc。

```C
#define MAX7219_CHIPS_SCAN_NUMBER_TABLE     \
{                                           \
    0xfc,                                   \
}
```

两个及其多个芯片级联时，只需要根据芯片顺序，依次往下配置即可，
例如，级联了3片芯片：
```C
#define MAX7219_CHIPS_SCAN_NUMBER_TABLE     \
{                                           \
    0xfc,                                   \
    0xa5,                                   \
    0x3f,                                   \
}
```

芯片配置如下表：

|  | 芯片一|芯片二|芯片三|
|----|----|----|----|
|配置| 0xfc | 0xa5 | 0x3f  |
|芯片配置|`1 1 1 1 1 1 0 0 `| `1 0 1 0 0 1 0 1`|`0 0 1 1 1 1 1 1`
|位置标号|1 2 3 4 5 6 - - |7 - 8 - - 9 - 10 | - - 11 12 13 14 15 16  |


如下图，从左到右显示：

![config.png](https://github.com/redocCheng/max7219/blob/master/examples/figer/config.png)



## API说明


1. 消隐全部数码管

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

2. 向数码管寄存器写入线段数据

```C
int max7219_write_dig(uint16_t dig, uint8_t data);
```

|参数|注释|
|----|----|
|dig|在所有数码管中的位置，未加入配置的数码管，不进行计数|
|data|数据|
|返回|注释|
|----|----|
|RT_EOK|正常|
|-RT_ERROR|异常|

说明：直接将数据写入显示寄存器，即直接控制(DP/A/B/C/D/E/F/G)。

3. 向数码管寄存器写入数字和字符

```C
int max7219_write(uint16_t dig, uint8_t data);
```

|参数|注释|
|----|----|
|dig|在所有数码管中的位置，同上|
|data|数字、字符和点|
|返回|注释|
|----|----|
|RT_EOK|正常|
|-RT_ERROR|异常|


说明：支持数字：0-0xf；支持右方字符： `' '`（空格）、 `'-'`、 `'_'`、 `'H'`、 `'h'`、 `'P'`、 `'p'`、 `'r'` 、`.`；当最高位有效时，即 0x80，同时显示点。

```C
    max7219_write(1,1);
    max7219_write(2,2);
    max7219_write(3,3);
    max7219_write(4,4);
    max7219_write(5,5);
    max7219_write(6,6);
    max7219_write(7,7);
    max7219_write(8,8);
    max7219_write(9,9);
    max7219_write(10,0xa);
    max7219_write(11,0xb);
    max7219_write(12,0xc);
    max7219_write(13,0xd);
    max7219_write(14,0xe);
    max7219_write(15,0xf);
    max7219_write(16,0x80);
    max7219_write(17,'H');
    max7219_write(18,'P');
    max7219_write(19,'_');
    max7219_write(20,' ');
    max7219_write(21,'-');
    max7219_write(22,'L');
    max7219_write(23,'r');
    max7219_write(24,'.');
```

![all_display.png](https://github.com/redocCheng/max7219/blob/master/examples/figer/all_display.png)


4. 亮度设置

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

芯片手册在 [max7219.pdf](https://www.ic37.com/pdf/pdf_download.asp?id=13030329_46310) 下。

## 注意事项

1. 配置参数中的dig和硬件设计的数码管顺序有关。
2. 建议只用非编码模式，所以不加编码配置选项，可以在[drv_max7219.h](./inc/drv_max7219.h)的 `MAX7219_INFO_DEFAULT` 选择配置,并且采用`max7219_write_dig`函数去操作。
3. 其他自定义字符，编码要去手册里面找，操作方式同上。
4. 还没有用到点阵的驱动，后续可能会在这块优化更新。

## 联系方式 & 感谢

* 维护：redoccheng
* 主页：<https://github.com/redocCheng/max7219>
* 邮箱：<619675912@qq.com>
