typedef struct {
	lock_t lock;
	small_uint_t channel;
} adc_t;

void adc_init (adc_t *v);
void adc_select_channel (adc_t *v, small_uint_t cnum);
unsigned adc_read (adc_t *u);
