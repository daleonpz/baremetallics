---
layout: post
title: Creating a Custom Logger with Zephyr and Kconfig
category: programming
description: Learn how to create a custom logger in Zephyr using Kconfig to manage log levels across multiple modules.
tags: [zephyr, logger, arm, kconfig]
---

In Zephyr, the typical way to enable logging in your application is by setting LOG_LEVEL and registering the module.
This works well for small projects where you have one or two modules. But if your repository has multiple source files or modules, setting the log level per file gets messy. This is especially true if all modules should have different log levels.

```c
/* main.c */
#define LOG_LEVEL LOG_LEVEL_DBG
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(my_module);
```

Wouldn't it be better to manage all log levels from one point? ..mabye `prj.conf`?

That’s exactly what this post is about.

# Setting Up a Custom Kconfig for Logging
Zephyr's CMake system automatically picks up a Kconfig file if it’s placed in the same directory as your application’s CMakeLists.txt.

```sh
$ tree --charset=utf-8 --dirsfirst -L 1
.
├── boards
├── src
├── CMakeLists.txt
├── Kconfig
└── prj.conf
```

Example Kconfig file for a custom logger

```sh
# Kconfig
# 1.
mainmenu "Custom Logger Configuration"

# 2.
source "Kconfig.zephyr"

# 3.
menu "Application Options"
config APP_LOG
    bool "Enable logging in main"
    default y if LOG
    depends on LOG
    help
      Enable logging for the main application module.
endmenu

# 4.
if APP_LOG
module = APP
module-str = "app"
source "subsys/logging/Kconfig.template.log_config"
endif
```

Let’s break file down in four parts:

1. `mainmenu` sets the title for `menuconfig`.

2. source `Kconfig.zephyr` includes the standard Zephyr config symbols. This is necessary since we’re modifying `zephyr/Kconfig` to add our symbols.

3. `APP_LOG` is a custom boolean symbol that enables logging for our custom module. Since we're building our own logger, it makes sense that our logger depends on LOG, which is the global logging switch. Also, our logger is enabled by default.

4. The definition of our custom logger. This is a defined structure in the [logger documentation](https://docs.zephyrproject.org/latest/services/logging/index.html#logging-in-a-module):
	- **module**: defines the symbol of the new logger module
	- **module-str**: defines the name or description of the module
	- **source**: the is logging template from Zephyr (`Kconfig.template.log_config`) lets you define per-module log levels using Kconfig.

You can verify the new additions with `west build -t menuconfig`. For example the menu **“Application Options”** we just defined

![zephyr log menuconfig](/images/posts/zephyr_log_menuconfig.png)

You also see that our logger is enabled by default and marked with an asterisk.

![zephyr log enabled](/images/posts/zephyr_log_enabled.png)

# Configuring Log Levels in prj.conf
Once you've got your Kconfig set up, you can define the log level of your custom logger in prj.conf with just a few lines.

```sh
CONFIG_LOG=y
CONFIG_APP_LOG=y
CONFIG_APP_LOG_LEVEL_INF=y
```

Now in your code you can use `CONFIG_APP_LOG_LEVEL` instead of `LOG_LEVEL_DBG` when defining the `LOG_LEVEL`

```c
/* main.c */
#define LOG_LEVEL CONFIG_APP_LOG_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app);
```

You can repeat this pattern for other modules too:

```sh
# prj.conf
# Enable logging
CONFIG_LOG=y

# Enable custom logger
CONFIG_APP_LOG=y
CONFIG_APP_LOG_LEVEL_INF=y

# Enable other custom logger
CONFIG_OTHER_LOG=y
CONFIG_OTHER_LOG_LEVEL_DBG=y

# Enable MCUMGR logger, defined already in Zephyr
CONFIG_MCUMGR_LOG=y
CONFIG_MCUMGR_LOG_LEVEL_DBG=y
CONFIG_MCUMGR_TRANSPORT_LOG_LEVEL_DBG=y
```

This way, you manage all log levels from one central place 😎.

# Final Thoughts

Zephyr offers an easy way to create custom and maintainable loggers using Kconfig and `Kconfig.template.log_config`.

If your project has more than a couple of modules, this setup will save you time and keep your development consistent.

# References

- [Practical Zephyr, Kconfig](https://interrupt.memfault.com/blog/practical_zephyr_kconfig)
- [Kconfig logging template](https://github.com/zephyrproject-rtos/zephyr/blob/main/subsys/logging/Kconfig.template.log_config)
- [Zephyr logger documentation](https://docs.zephyrproject.org/latest/services/logging/index.html)
