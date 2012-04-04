#define LOG_TAG "lights"

#include <cutils/log.h>

#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>

#include <sys/ioctl.h>
#include <sys/types.h>

#include <hardware/lights.h>

#include <asm/arch/cpu.h>

static int light_set(struct light_device_t* dev,
		struct light_state_t const* state)
{
	int color = state->color & 0x00ffffff;
	color = ((77*((color>>16)&0x00ff)) + (150*((color>>8)&0x00ff)) + (29*(color&0x00ff))) >> 8;

	fd = open( "/sys/class/leds/lcd-backlight/brightness", O_RDWR);
	if (fd >= 0) {
		char buffer[20];
		int bytes = sprintf(buffer, "%d\n", value);
		int amt = write(fd, buffer, bytes);
		close(fd);
		return amt == -1 ? -errno : 0;
	}
	LOGE("write_int failed to open %s\n", path);
	return -errno;
}

static int lights_open(const struct hw_module_t* module, char const* name, struct hw_device_t** device)
{
	if (strcmp(LIGHT_ID_BACKLIGHT, name)==0)
	{
		struct light_device_t *dev = malloc(sizeof(struct light_device_t));
		memset(dev, 0, sizeof(*dev));

		dev->common.tag = HARDWARE_DEVICE_TAG;
		dev->common.version = 0;
		dev->common.module = (struct hw_module_t*)module;
		dev->common.close = (int (*)(struct hw_device_t*))close_lights;
		dev->set_light = light_set;

		*device = (struct hw_device_t*)dev;
		return 0;
	}
	else
		return -EINVAL;
}

static struct hw_module_methods_t lights_methods = {
	.open =  lights_open,
};

struct hw_module_t HAL_MODULE_INFO_SYM = {
	.tag = HARDWARE_MODULE_TAG,
	.version_major = 1,
	.version_minor = 0,
	.id = LIGHTS_HARDWARE_MODULE_ID,
	.name = "Lights",
	.author = "Liu Xu",
	.methods = &lights_methods,
};

