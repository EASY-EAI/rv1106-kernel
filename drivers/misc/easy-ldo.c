// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Driver for ldo supply controlled through GPIO-line
 *
 * Author: 2024, ChenHaiMan <chenhaiman@easy-eai.com>
 */

#include <linux/err.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/gpio/consumer.h>
#include <linux/of.h>
#include <linux/platform_device.h>

struct gpio_ldo_priv {
	struct gpio_desc	*gpiod;
};

static int gpio_ldo_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct gpio_ldo_priv *priv;

	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	platform_set_drvdata(pdev, priv);

	priv->gpiod = devm_gpiod_get(dev, NULL, GPIOD_OUT_HIGH);
	if (IS_ERR(priv->gpiod))
		return PTR_ERR(priv->gpiod);

	return 0;
}

static int __maybe_unused gpio_ldo_suspend(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct gpio_ldo_priv *priv = platform_get_drvdata(pdev);

	gpiod_set_value_cansleep(priv->gpiod, 0);
	return 0;
}

static int __maybe_unused gpio_ldo_resume(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct gpio_ldo_priv *priv = platform_get_drvdata(pdev);

	gpiod_set_value_cansleep(priv->gpiod, 1);
	return 0;
}

static SIMPLE_DEV_PM_OPS(gpio_ldo_pm_ops, gpio_ldo_suspend,
	gpio_ldo_resume);

static const struct of_device_id gpio_ldo_dt_ids[] = {
	{ .compatible = "linux,ldo-gpio", },
	{ }
};
MODULE_DEVICE_TABLE(of, gpio_ldo_dt_ids);

static struct platform_driver gpio_ldo_driver = {
	.driver	= {
		.name		= "gpio-ldo",
		.of_match_table	= gpio_ldo_dt_ids,
		.pm = &gpio_ldo_pm_ops,
	},
	.probe	= gpio_ldo_probe,
};

static int __init gpio_ldo_init(void)
{
	return platform_driver_register(&gpio_ldo_driver);
}
arch_initcall(gpio_ldo_init);

MODULE_AUTHOR("ChenHaiMan <chenhaiman@easy-eai.com>");
MODULE_DESCRIPTION("GPIO LDO");
MODULE_LICENSE("GPL");
