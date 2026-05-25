#ifndef GPIO_LOGGER_H
#define GPIO_LOGGER_H

#include <QString>

namespace GpioLogger {
void init();
void shutdown();
QString logFilePath();
}

#endif // GPIO_LOGGER_H
