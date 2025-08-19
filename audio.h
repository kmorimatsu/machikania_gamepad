// Configurations
#define SOUND_PWM_MUL 1
#define AUDIO_PORT 28
#define AUDIO_SLICE 6
#define AUDIO_CHAN PWM_CHAN_A

void init_audio(void);
void set_sound(unsigned short val);
