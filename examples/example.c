void example1(uint16_t value_a, uint16_t value_b)
{
    uint32_t value = 0;
    uint8_t value_ge = 0;
    uint8_t value_shi = 0;
    uint8_t value_bai = 0;

    value = value_a;
    value_ge = value%10;
    value_shi = value%100/10;
    value_bai = value%1000/100;

    max7219_write(1, value_bai);
    max7219_write(2, value_shi);
    max7219_write(3, value_ge);

    value = value_b;
    value_ge = value%10;
    value_shi = value%100/10;
    value_bai = value%1000/100;

    max7219_write(4, value_bai);
    max7219_write(5, value_shi);
    max7219_write(6, value_ge);
}

void example2(void)
{
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
}