// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2025, Aleksandrs Vinarskis
 */

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/regulator/consumer.h>
#include <linux/of.h>
#include <linux/pm.h>

struct dummy_consumer_data {
    struct regulator *dummy_vdd;
};

static int dummy_probe(struct platform_device *pdev)
{
    struct dummy_consumer_data *data;
    int ret;

    data = devm_kzalloc(&pdev->dev, sizeof(*data), GFP_KERNEL);
    if (!data)
        return -ENOMEM;

    data->dummy_vdd = devm_regulator_get(&pdev->dev, "vdd");
    if (IS_ERR(data->dummy_vdd)) {
        dev_err(&pdev->dev, "Failed to get regulator\n");
        return PTR_ERR(data->dummy_vdd);
    }

    ret = regulator_enable(data->dummy_vdd);
    if (ret) {
        dev_err(&pdev->dev, "Failed to enable regulator: %d\n", ret);
        return ret;
    }

    platform_set_drvdata(pdev, data);

    dev_dbg(&pdev->dev, "Dummy regulator consumer initialized\n");
    return 0;
}

static void dummy_remove(struct platform_device *pdev)
{
    struct dummy_consumer_data *data = platform_get_drvdata(pdev);
    regulator_disable(data->dummy_vdd);
}

static int dummy_suspend(struct device *dev)
{
    struct dummy_consumer_data *data = dev_get_drvdata(dev);
    return regulator_disable(data->dummy_vdd);
}

static int dummy_resume(struct device *dev)
{
    struct dummy_consumer_data *data = dev_get_drvdata(dev);
    return regulator_enable(data->dummy_vdd);
}

static DEFINE_SIMPLE_DEV_PM_OPS(dummy_consumer_pm, dummy_suspend, dummy_resume);

static const struct of_device_id dummy_of_match[] = {
    { .compatible = "dummy,regulator-consumer", },
    { }
};
MODULE_DEVICE_TABLE(of, dummy_of_match);

static struct platform_driver dummy_driver = {
    .driver = {
        .name = "dummy-regulator-consumer",
        .of_match_table = dummy_of_match,
        .pm = pm_sleep_ptr(&dummy_consumer_pm),
    },
    .probe = dummy_probe,
    .remove = dummy_remove,
};
module_platform_driver(dummy_driver);

MODULE_AUTHOR("Aleksandrs Vinarskis");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Dummy regulator consumer driver");

