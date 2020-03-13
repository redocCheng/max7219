void display(uint16_t value_a, uint16_t value_b)
{
    uint32_t value = 0;
    uint8_t value_ge = 0;
    uint8_t value_shi = 0;
    uint8_t value_bai = 0;

    value = value_a;
    value_ge = value%10;
    value_shi = value%100/10;
    value_bai = value%1000/100;

    max7219_write_num(1, value_bai);
    max7219_write_num(2, value_shi);
    max7219_write_num(3, value_ge);

    value = value_b;
    value_ge = value%10;
    value_shi = value%100/10;
    value_bai = value%1000/100;

    max7219_write_num(4, value_bai);
    max7219_write_num(5, value_shi);
    max7219_write_num(6, value_ge);

    log_d("display: A-%d B-%d", value_a, value_b);
}
