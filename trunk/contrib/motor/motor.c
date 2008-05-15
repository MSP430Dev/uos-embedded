#include "runtime/lib.h"
#include "kernel/uos.h"
#include "motor/motor.h"

void motor_set_power (motor_t *p, unsigned char motors, signed char power);
signed char motor_get_power (motor_t *p, unsigned char motor);

void motor_init (motor_t *p)
{
	lock_init (&p->lock);
	lock_take (&p->lock);

	/* Режим широтно-ипульсного модулятора.
	 * Источник синхросигнала - CK/64, получается 4000000/64/510,
	 * или примерно 125 герц. */
	outb (0x64, TCCR0);
	outb (0x63, TCCR2);

	outb (0, OCR0);		/* низкий уровень */
	outb (0, OCR2);		/* низкий уровень */

	outb (0, TCNT0);
	outb (0, TCNT2);

	/* Переключаем B5-B7 на выход. */
	setb (5, PORTB);	/* направление "вперед" */
	setb (6, PORTB);	/* направление "вперед" */
	outb (inb (DDRB) | 0xf0, DDRB);

	lock_release (&p->lock);
}

signed char motor_get_power (motor_t *p, unsigned char motor)
{
	signed char val;

	lock_take (&p->lock);
	val = (motor == MOTOR_LEFT) ? p->left_power : p->right_power;
	lock_release (&p->lock);
	return val;
}

void motor_set_power (motor_t *p, unsigned char motors, signed char power)
{
	unsigned char direction, level;

	if (power >= 0) {
		direction = 1;
		if (power == 0)
			level = 0;
		else
			level = power << 1 | 1;
	} else {
		direction = 0;
		if (power < -127)
			power = -127;
		level = (-power) << 1 | 1;
	}

	lock_take (&p->lock);

	if (motors & MOTOR_LEFT) {
		outb (level, OCR0);
		if (direction)
			setb (5, PORTB);	/* направление "вперед" */
		else
			clearb (5, PORTB);	/* направление "назад" */
		p->left_power = power;
	}
	if (motors & MOTOR_RIGHT) {
		outb (level, OCR2);
		if (direction)
			setb (6, PORTB);	/* направление "вперед" */
		else
			clearb (6, PORTB);	/* направление "назад" */
		p->right_power = power;
	}
	lock_release (&p->lock);
}
