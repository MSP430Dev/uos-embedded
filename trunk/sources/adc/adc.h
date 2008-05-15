typedef struct {
	lock_t lock;
} adc_t;

void adc_init (adc_t *v);
void adc_select_channel (adc_t *v, unsigned char cnum);
unsigned short adc_read (adc_t *u);
