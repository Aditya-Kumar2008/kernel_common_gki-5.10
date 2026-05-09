/*
 * Copyright (c) 2024, Xiaomi Corporation.
 * NT36532 DSI Panel Driver for POCO PAD 5G (ruan)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/of_gpio.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/regulator/consumer.h>
#include <video/mipi_display.h>
#include <dsi_panel.h>

#define NT36532_DSI_CMD_WAIT_MS120
#define NT36532_DSI_VIDEO_MODE0
#define NT36532_DSI_CMD_MODE1

static int dsi_panel_nt36532_power_on(struct dsi_panel *panel)
{
int ret = 0;
struct dsi_panel_power *power = &panel->power_info;

if (!power) {
invalid power info\n", __func__);
 -EINVAL;
}

/* Enable regulators */
ret = regulator_bulk_enable(power->num_supplies, power->supplies);
if (ret) {
failed to enable regulators\n", __func__);
 ret;
}

msleep(NT36532_DSI_CMD_WAIT_MS);

/* Toggle reset GPIO */
if (gpio_is_valid(panel->reset_gpio)) {
el->reset_gpio, 0);
ge(1000, 2000);
el->reset_gpio, 1);
 0;
}

static int dsi_panel_nt36532_power_off(struct dsi_panel *panel)
{
int ret = 0;
struct dsi_panel_power *power = &panel->power_info;

if (!power) {
invalid power info\n", __func__);
 -EINVAL;
}

/* Toggle reset GPIO */
if (gpio_is_valid(panel->reset_gpio)) {
el->reset_gpio, 0);
Disable regulators */
ret = regulator_bulk_disable(power->num_supplies, power->supplies);
if (ret) {
failed to disable regulators\n", __func__);
 ret;
}

return 0;
}

static int dsi_panel_nt36532_config(struct dsi_panel *panel)
{
int ret = 0;

if (!panel) {
invalid panel\n", __func__);
 -EINVAL;
}

/* Panel configuration for NT36532 */
panel->panel_mode = NT36532_DSI_VIDEO_MODE;
panel->dsi_cmd_sets = NULL;

return ret;
}

static struct dsi_panel_funcs nt36532_panel_funcs = {
.power_on = dsi_panel_nt36532_power_on,
.power_off = dsi_panel_nt36532_power_off,
.config = dsi_panel_nt36532_config,
};

static int dsi_panel_nt36532_parse_dt(struct dsi_panel *panel)
{
int ret = 0;

if (!panel) {
invalid panel\n", __func__);
 -EINVAL;
}

/* Parse GPIO from device tree */
panel->reset_gpio = of_get_named_gpio(panel->dev->of_node,
      "qcom,platform-reset-gpio", 0);
if (!gpio_is_valid(panel->reset_gpio)) {
reset gpio not found\n", __func__);
}

return ret;
}

struct dsi_panel *dsi_panel_nt36532_get(struct device *dev)
{
struct dsi_panel *panel;

panel = devm_kzalloc(dev, sizeof(*panel), GFP_KERNEL);
if (!panel)
 NULL;

panel->dev = dev;
panel->funcs = &nt36532_panel_funcs;
dsi_panel_nt36532_parse_dt(panel);

return panel;
}
EXPORT_SYMBOL(dsi_panel_nt36532_get);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Xiaomi");
MODULE_DESCRIPTION("NT36532 DSI Panel Driver for POCO PAD 5G (ruan)");
