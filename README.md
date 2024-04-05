# TM1620_C_driver

Pins configurations: STB,CLK,DATA (GPIO mode output pushpull, pull-up, maximum output speed low).
If using SPI, uncomment the line #define USE_SPI in the .c file .
SPI parameters: speed= 1Mb/s , CPOL=1,CPHA=1 (2 edge), data size 8 bits , LSB first, Transmit Only Master. DMA parameters: data width=byte , mode=normal, direction=memory to peripheral, priority=low,increment only memory address.

The driver was written using the fixed address option, what allows to address each display separately.  The automatic add address mode can be found in the following repo: Ant-Yuan
/
TM1620_driver

The tests were performed using HAL library and STM32WB55 nucleo board
