void parabolar(int r) {
	int pk;
	int x = 0, y = 0; // ค่าเริ่มต้น x กับ y เริ่มต้นที่ 0
	pk = 0.4;	// ค่าเริ่มต้น pk ที่ได้จากการคำนวณ ซึ่งเป็นของ x < 6
	setpixel(image, cx, x, y, 0, 0, 0);
	while (x <= r) {
		if (x < 6)	// ถ้า x < 6 คือเช็คจุดเปลี่ยนที่ x = 5, m <= 1 ซึ่งได้จากการคำนวณความชัน dy/dx = 0.2x
		{
			x++;
			if (pk < 0)
			{
				pk = pk - 0.2*x + 0.7;
				y++;
			}
			else
			{
				pk = pk - 0.2*x - 0.3;
			}
		}
		else // m > 1
		{
			if (x == 6)
			{
				pk = 0.475;
			}
			y++;
			if (pk < 0)
			{
				pk = pk + 1;
			}
			else
			{
				pk = pk - 0.2 * x + 0.8;
				x++;
			}
		}
		setpixel(image, cx, x, y, 0, 0, 0);
	}
}

void process (unsigned char *ig, long w, long h)
{
	// เรียกใช้ parabolar ส่ง x = 20 ตามโจทย์
	parabolar(20);
}
