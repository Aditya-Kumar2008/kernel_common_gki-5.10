/*
 * Copyright (c) 2024, Xiaomi Corporation.
 * Xiaomi Hall Sensor Driver for POCO PAD 5G (ruan)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/device.h>
#include <linux/regulator/consumer.h>

#define XIAOMI_HALL_DRIVER_NAME"xiaomi_hall"
#define XIAOMI_HALL_CLASS_NAME"xiaomi_hall"

struct xiaomi_hall_data {
struct input_dev *input_dev;
struct device *dev;
int irq_gpio;
int irq;
bool last_state;
};

static irqreturn_t xiaomi_hall_irq_handler(int irq, void *dev_id)
{
struct xiaomi_hall_data *data = dev_id;
int state;

if (gpio_is_valid(data->irq_gpio))
= gpio_get_value(data->irq_gpio);
else
= 0;

/* Report lid state: 0 = closed, 1 = open */
input_report_switch(data->input_dev, SW_LID, !state);
input_sync(data->input_dev);

data->last_state = !state;

return IRQ_HANDLED;
}

static int xiaomi_hall_probe(struct platform_device *plat_dev)
{
struct xiaomi_hall_data *data;
struct device_node *np = plat_dev->dev.of_node;
int ret;

data = devm_kzalloc(&plat_dev->dev, sizeof(*data), GFP_KERNEL);
if (!data)
 -ENOMEM;

/* Get IRQ GPIO from device tree */
data->irq_gpio = of_get_named_gpio(np, "hall,gpio", 0);
if (!gpio_is_valid(data->irq_gpio)) {
"Invalid hall GPIO\n");
 -EINVAL;
}

ret = gpio_request(data->irq_gpio, "xiaomi_hall_irq");
if (ret) {
"Failed to request GPIO\n");
 ret;
}

data->irq = gpio_to_irq(data->irq_gpio);
data->dev = &plat_dev->dev;

/* Allocate input device */
data->input_dev = devm_input_allocate_device(&plat_dev->dev);
if (!data->input_dev) {
"Failed to allocate input device\n");
 -ENOMEM;
}

data->input_dev->name = XIAOMI_HALL_DRIVER_NAME;
data->input_dev->id.bustype = BUS_GPIO;
input_set_capability(data->input_dev, EV_SW, SW_LID);

ret = input_register_device(data->input_dev);
if (ret) {
"Failed to register input device\n");
 ret;
}

/* Request IRQ */
ret = request_threaded_irq(data->irq, NULL, xiaomi_hall_irq_handler,
  IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
  XIAOMI_HALL_DRIVER_NAME, data);
if (ret) {
"Failed to request IRQ\n");
put_unregister_device(data->input_dev);
 ret;
}

platform_set_drvdata(plat_dev, data);

return 0;
}

static int xiaomi_hall_remove(struct platform_device *plat_dev)
{
struct xiaomi_hall_data *data = platform_get_drvdata(plat_dev);

if (data) {
data);
put_unregister_device(data->input_dev);
 0;
}

static const struct of_device_id xiaomi_hall_of_match[] = {
{ .compatible = "xiaomi,hall", },
{ }
};

MODULE_DEVICE_TABLE(of, xiaomi_hall_of_match);

static struct platform_driver xiaomi_hall_driver = {
.driver = {
ame = XIAOMI_HALL_DRIVER_NAME,
= xiaomi_hall_of_match,
},
.probe = xiaomi_hall_probe,
.remove = xiaomi_hall_remove,
};

module_platform_driver(xiaomi_hall_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Xiaomi");
MODULE_DESCRIPTION("Xiaomi Hall Sensor Driver for POCO PAD 5G (ruan)");
