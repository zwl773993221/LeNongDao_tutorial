/*
 * ISL29013 ambient light sensor driver
 *
 * Copyright (C) 2013 iVeia, LLC
 * Author: Brian Silverman <bsilverman@iveia.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "isl29013.h"

#include <linux/i2c.h>
#include <linux/mutex.h>
#include <linux/input-polldev.h>
#include <linux/slab.h>
#include <linux/interrupt.h>

/*
 * The generic device name is used to expose a device-independent name to
 * userspace(e.g. Android)
 */
#define GENERIC_DEVICE_NAME	"lightsensor-level"
#define ISL29013_DEV_NAME	"isl29013"

/*
 * Configurable values from schematic/datasheet
 */
#define RANGE_K_ZERO_BASED	1
#define R_EXT_KOHMS		100
static const unsigned long range[] = { 2000, 8000, 32000, 128000 };

#define LUX_MIN			0
#define LUX_MAX			0xFFFF
#define LUX_FUZZ		0
#define LUX_FLAT		0

#define DEFAULT_POLL_INTERVAL	1000

/*
 * ISL29013 Registers
 */
#define REG_COMMAND		0x00
	#define FLAG_ADCE		0x80
	#define FLAG_ADCPD		0x40
	#define FLAG_TIMM		0x20
	#define FLAG_ADCM1		0x08
	#define FLAG_ADCM0		0x04
	#define FLAG_RES1		0x02
	#define FLAG_RES0		0x01
#define REG_CONTROL		0x01
	#define FLAG_INT_FLAG		0x20
	#define FLAG_GAIN1		0x08
	#define FLAG_GAIN0		0x04
	#define FLAG_IC1		0x02
	#define FLAG_IC0		0x01
#define REG_INTR_THRESH_HI		0x02
#define REG_INTR_THRESH_LO		0x03
#define REG_LSB_SENSOR			0x04
#define REG_MSB_SENSOR			0x05
#define REG_LSB_TIMER			0x06
#define REG_MSB_TIMER			0x07

struct isl29013_data {
	atomic_t enabled;
	struct i2c_client *client;
	struct isl29013_platform_data *pdata;
	struct input_polled_dev *input_polled_dev;
};

static int isl29013_i2c_read(struct isl29013_data *light,
				  u8 *buf, int len)
{
	int err;

	struct i2c_msg msgs[] = {
		{
		 .addr = light->client->addr,
		 .flags = light->client->flags,
		 .len = 1,
		 .buf = buf,
		 },
		{
		 .addr = light->client->addr,
		 .flags = light->client->flags | I2C_M_RD,
		 .len = len,
		 .buf = buf,
		 },
	};

	err = i2c_transfer(light->client->adapter, msgs, 2);

	if (err != 2) {
		dev_err(&light->client->dev, "read transfer error\n");
		err = -EIO;
	} else {
		err = 0;
	}

	return err;
}

static int isl29013_i2c_write(struct isl29013_data *light,
				   u8 *buf, int len)
{
	int err;

	struct i2c_msg msgs[] = {
		{
		 .addr = light->client->addr,
		 .flags = light->client->flags,
		 .len = len,
		 .buf = buf,
		 },
	};

	err = i2c_transfer(light->client->adapter, msgs, 1);

	if (err != 1) {
		dev_err(&light->client->dev, "write transfer error\n");
		err = -EIO;
	} else {
		err = 0;
	}

	return err;
}

static int isl29013_hw_init(struct isl29013_data *light)
{
	int err = 0;
	u8 data[2];
	int gain_flags;

        data[0] = REG_COMMAND;
        data[1] = FLAG_ADCE | FLAG_ADCM1;
        err = isl29013_i2c_write(light, data, 2);
	if (err < 0)
		return err;

	switch (RANGE_K_ZERO_BASED) {
		case 0: gain_flags = 0; break;
		case 1: gain_flags = FLAG_GAIN0; break;
		case 2: gain_flags = FLAG_GAIN1; break;
		case 3: gain_flags = FLAG_GAIN1 | FLAG_GAIN0; break;
		default: gain_flags = 0; break;
	}
        data[0] = REG_CONTROL;
        data[1] = gain_flags;
        err = isl29013_i2c_write(light, data, 2);
	if (err < 0)
		return err;

	return 0;
}

static int isl29013_hw_shutdown(struct isl29013_data *light)
{
	int err = 0;
	u8 data[2];

        data[0] = REG_COMMAND;
        data[1] = FLAG_ADCPD;
        err = isl29013_i2c_write(light, data, 2);
	if (err < 0)
		return err;

	return 0;
}

static int isl29013_enable(struct isl29013_data *light)
{
	int err;

	if (!atomic_cmpxchg(&light->enabled, 0, 1)) {
		int err;

		if (light->pdata->power_on) {
			err = light->pdata->power_on();
			if (err < 0)
				goto err_power_on;
		}

		err = isl29013_hw_init(light);
		if (err < 0)
			goto err_isl29013_hw_init;
	}

	return 0;

err_isl29013_hw_init:
	isl29013_hw_shutdown(light);
err_power_on:
	atomic_set(&light->enabled, 0);
	return err;
}

static int isl29013_disable(struct isl29013_data *light)
{
	int err = 0;
	if (atomic_cmpxchg(&light->enabled, 1, 0)) {
		err = isl29013_hw_shutdown(light);
		if (err < 0) {
			dev_err(&light->input_polled_dev->input->dev, 
				"Could not shutdown device\n");
		}

		if (light->pdata->power_off)
			light->pdata->power_off();
	}

	return err;
}

static ssize_t attr_get_polling_rate(struct device *dev,
				     struct device_attribute *attr,
				     char *buf)
{
	struct isl29013_data *light = dev_get_drvdata(dev);
	return sprintf(buf, "%d\n", light->pdata->poll_interval);
}

static ssize_t attr_set_polling_rate(struct device *dev,
				     struct device_attribute *attr,
				     const char *buf, size_t size)
{
	struct isl29013_data *light = dev_get_drvdata(dev);
	unsigned long interval_ms;

	if (strict_strtoul(buf, 10, &interval_ms))
		return -EINVAL;
	if (!interval_ms)
		return -EINVAL;

	light->pdata->poll_interval = interval_ms;
	light->input_polled_dev->poll_interval = interval_ms;
	return size;
}

static ssize_t attr_get_enable(struct device *dev,
			       struct device_attribute *attr, char *buf)
{
	struct isl29013_data *light = dev_get_drvdata(dev);
	int val = atomic_read(&light->enabled);
	return sprintf(buf, "%d\n", val);
}

static ssize_t attr_set_enable(struct device *dev,
			       struct device_attribute *attr,
			       const char *buf, size_t size)
{
	struct isl29013_data *light = dev_get_drvdata(dev);
	unsigned long val;

	if (strict_strtoul(buf, 10, &val))
		return -EINVAL;

	if (val)
		isl29013_enable(light);
	else
		isl29013_disable(light);

	return size;
}

static struct device_attribute attributes[] = {
	__ATTR(poll_delay, 0666, attr_get_polling_rate, attr_set_polling_rate),
	__ATTR(enable, 0666, attr_get_enable, attr_set_enable),
};

static int create_sysfs_interfaces(struct device *dev)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(attributes); i++)
		if (device_create_file(dev, attributes + i))
			goto error;
	return 0;

error:
	for ( ; i >= 0; i--)
		device_remove_file(dev, attributes + i);
	dev_err(dev, "%s:Unable to create interface\n", __func__);
	return -1;
}

static int remove_sysfs_interfaces(struct device *dev)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(attributes); i++)
		device_remove_file(dev, attributes + i);
	return 0;
}


static void isl29013_input_poll(struct input_polled_dev *polled_dev)
{
	struct isl29013_data *light = polled_dev->private;
	int err;
	u8 data[2];
	unsigned long lsb, msb;
	unsigned long val;
	int lux;

        data[0] = REG_LSB_SENSOR;
        err = isl29013_i2c_read(light, data, 2);
	if (err < 0)
		return;
	lsb = data[0];
	msb = data[1];

	/*
	 * Convert to Lux, from ISL29013 datasheet.
	 *
	 * ADC was initialized as 16-bit
	 */
	val = msb << 8 | lsb;
	lux = (range[RANGE_K_ZERO_BASED] * (100/R_EXT_KOHMS) * val) / (1 << 16);

	dev_dbg(&light->client->dev, "LUX: %d\n", lux);
	input_report_abs(polled_dev->input, ABS_MISC, lux);
	input_sync(polled_dev->input);
}

static int isl29013_input_register(struct isl29013_data *light)
{
	int err;
	struct input_polled_dev *polled_dev;

	light->input_polled_dev = input_allocate_polled_device();
	if (!light->input_polled_dev) {
		err = -ENOMEM;
		dev_err(&light->client->dev, "input device allocate failed\n");
		goto out;
	}

	polled_dev = light->input_polled_dev;

	polled_dev->poll = isl29013_input_poll;
	polled_dev->poll_interval = 
		light->pdata->poll_interval ? light->pdata->poll_interval : DEFAULT_POLL_INTERVAL;
	polled_dev->private = light;
	polled_dev->input->name = GENERIC_DEVICE_NAME;
	polled_dev->input->phys = GENERIC_DEVICE_NAME "/input";
	polled_dev->input->id.bustype = BUS_HOST;
	polled_dev->input->dev.parent = &light->client->dev;

	set_bit(EV_ABS, polled_dev->input->evbit);

	input_set_abs_params(polled_dev->input, ABS_MISC, LUX_MIN, LUX_MAX, LUX_FUZZ, LUX_FLAT);

	err = input_register_polled_device(light->input_polled_dev);
	if (err) {
		dev_err(&light->client->dev,
			"unable to register input device %s\n",
			polled_dev->input->name);
		goto out;
	}

	return 0;

out:
	if (light->input_polled_dev)
		input_free_polled_device(light->input_polled_dev);
	light->input_polled_dev = NULL;
	return err;
}

static void isl29013_input_unregister(struct isl29013_data *light)
{
	input_unregister_polled_device(light->input_polled_dev);
	input_free_polled_device(light->input_polled_dev);
}

static int isl29013_probe(struct i2c_client *client,
			   const struct i2c_device_id *id)
{
	struct isl29013_data *light = NULL;
	int err = -1;

	if (client->dev.platform_data == NULL) {
		dev_err(&client->dev, "platform data is NULL. exiting.\n");
		err = -ENODEV;
		goto err_platform_data;
	}

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		dev_err(&client->dev, "client not i2c capable\n");
		err = -ENODEV;
		goto err_i2c_check_functionality;
	}

	light = kzalloc(sizeof(*light), GFP_KERNEL);
	if (light == NULL) {
		dev_err(&client->dev,
			"failed to allocate memory for module data\n");
		err = -ENOMEM;
		goto err_alloc_dev;
	}
	atomic_set(&light->enabled, 0);

	light->client = client;

	light->pdata = kmalloc(sizeof(*light->pdata), GFP_KERNEL);
	if (light->pdata == NULL)
		goto err_alloc_pdata;

	memcpy(light->pdata, client->dev.platform_data, sizeof(*light->pdata));

	i2c_set_clientdata(client, light);

	if (light->pdata->init) {
		err = light->pdata->init();
		if (err < 0)
			goto err_pdata_init;
	}

	err = isl29013_enable(light);
	if (err < 0)
		goto err_isl29013_enable;

	err = isl29013_input_register(light);
	if (err < 0)
		goto err_isl29013_input_init;

	err = create_sysfs_interfaces(&client->dev);
	if (err < 0) {
		dev_err(&client->dev, ISL29013_DEV_NAME " sysfs register failed\n");
		goto err_create_sysfs_interfaces;
	}

	return 0;

err_create_sysfs_interfaces:
	isl29013_input_unregister(light);
err_isl29013_input_init:
	isl29013_disable(light);
err_isl29013_enable:
	if (light->pdata->exit)
		light->pdata->exit();
err_pdata_init:
	kfree(light->pdata);
err_alloc_pdata:
	kfree(light);
err_alloc_dev:
err_i2c_check_functionality:
err_platform_data:
	return err;
}

static int __devexit isl29013_remove(struct i2c_client *client)
{
	struct isl29013_data *light = i2c_get_clientdata(client);
	remove_sysfs_interfaces(&client->dev);
	isl29013_input_unregister(light);
	isl29013_disable(light);
	if (light->pdata->exit)
		light->pdata->exit();
	kfree(light->pdata);
	kfree(light);

	return 0;
}

static const struct i2c_device_id isl29013_id[] = {
	{ISL29013_DEV_NAME, 0},
	{},
};

MODULE_DEVICE_TABLE(i2c, isl29013_id);

static struct i2c_driver isl29013_driver = {
	.driver = {
		.name = ISL29013_DEV_NAME,
		.owner = THIS_MODULE,
	},
	.probe = isl29013_probe,
	.remove = __devexit_p(isl29013_remove),
	.id_table = isl29013_id,
};


static int __init isl29013_init(void)
{
	pr_debug("ISL29013 driver\n");
	printk (KERN_INFO "ISL29013 driver\n");
	return i2c_add_driver(&isl29013_driver);
}

static void __exit isl29013_exit(void)
{
	i2c_del_driver(&isl29013_driver);
	return;
}

module_init(isl29013_init);
module_exit(isl29013_exit);

MODULE_DESCRIPTION(ISL29013_DEV_NAME " ambient light sensor driver");
MODULE_AUTHOR("Brian Silverman <bsilverman@iveia.com>");
MODULE_LICENSE("GPL");