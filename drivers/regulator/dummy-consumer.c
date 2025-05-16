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
    struct regulator *dummy-vdd;
};

static int dummy_probe(struct platform_device *pdev)
{
    struct dummy_consumer_data *data;

    data = devm_kzalloc(&pdev->dev, sizeof(*data), GFP_KERNEL);
    if (!data)
        return -ENOMEM;

    data->dummy-vdd = devm_regulator_get(&pdev->dev, "dummy");
    if (IS_ERR(data->dummy-vdd)) {
        dev_err(&pdev->dev, "Failed to get regulator\n");
        return PTR_ERR(data->dummy-vdd);
    }

    regulator_enable(data->dummy-vdd);
    platform_set_drvdata(pdev, data);

    dev_debug(&pdev->dev, "Dummy regulator consumer initialized\n");
    return 0;
}

static int dummy_remove(struct platform_device *pdev)
{
    struct dummy_consumer_data *data = platform_get_drvdata(pdev);
    regulator_disable(data->dummy-vdd);
    return 0;
}

#ifdef CONFIG_PM_SLEEP
static int dummy_suspend(struct device *dev)
{
    struct dummy_consumer_data *data = dev_get_drvdata(dev);
    return regulator_disable(data->dummy-vdd);
}

static int dummy_resume(struct device *dev)
{
    struct dummy_consumer_data *data = dev_get_drvdata(dev);
    return regulator_enable(data->dummy-vdd);
}

static const struct dev_pm_ops dummy_pm_ops = {
    .suspend = dummy_suspend,
    .resume  = dummy_resume,
};
#endif

static const struct of_device_id dummy_of_match[] = {
    { .compatible = "dummy,regulator-consumer", },
    { }
};
MODULE_DEVICE_TABLE(of, dummy_of_match);

static struct platform_driver dummy_driver = {
    .driver = {
        .name = "dummy-regulator-consumer",
        .of_match_table = dummy_of_match,
#ifdef CONFIG_PM_SLEEP
        .pm = &dummy_pm_ops,
#endif
    },
    .probe = dummy_probe,
    .remove = dummy_remove,
};
module_platform_driver(dummy_driver);

MODULE_AUTHOR("Aleksandrs Vinarskis");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Dummy regulator consumer driver");

