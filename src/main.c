#include <stm32f4xx.h>
#include "actuators.h"

int main(void)
{
    for (;;)
    {
        cooling_fan();
    }
}
