typedef struct _motor_t {
	mutex_t lock;
	signed char left_power;
	signed char right_power;
} motor_t;

#define MOTOR_LEFT	1
#define MOTOR_RIGHT	2

void motor_init (motor_t *p);
void motor_set_power (motor_t *p, unsigned char motors, signed char power);
signed char motor_get_power (motor_t *p, unsigned char motors);
