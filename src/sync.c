#include "cmsis/stm32f4xx.h"
#include "sync.h"
#include "config.h"
#include "injection.h"
#include "status.h"

/*
    Реализация для датчика Nissan CAS SR20DET/VE/VET: 360, 16, 12, 8, 4.

    TIM10: захват опорного сигнала текущего цилиндра; измерение интервала времени
    между циклами.

    TIM9: канал 1 в режиме счётчика импульсов сигнала положения; канал 2 в режиме сравнения 
    генерирует прерывание каждые 6 градусов положения коленчатого вала (эмуляция шкива ДПКВ
    с 60 зубьями).

    TIM1: 4 канала в режиме сравнения для выполнения событий в промежутке между 
    зубьями (начало накопления энергии в катушке зажигания, окончание накопления, 
    начало впрыска, фазовое окно контроля детонации).
*/

/**
 * Инициализация синхронизации
 */
void sync_init(void)
{
    // Опорный вход сигнала текущего цилиндра
    GPIOB->MODER    |= GPIO_MODER_MODER8_1;
    GPIOB->PUPDR    &= ~GPIO_PUPDR_PUPDR8;
    GPIOB->AFR[1]   |= (0x03 << 0); // TIM10_CH1

    // Вход сигнала положения
    GPIOE->MODER    |= GPIO_MODER_MODER5_1;
    GPIOE->PUPDR    &= ~GPIO_PUPDR_PUPDR5;
    GPIOE->AFR[0]   |= (0x03 << 20); // TIM9_CH1

    // Таймер опорного сигнала
    TIM10->PSC      = 99;
    TIM10->CCMR1    |= TIM_CCMR1_CC1S_0;
    TIM10->CCER     |= TIM_CCER_CC1E;
    TIM10->DIER     |= (TIM_DIER_CC1IE | TIM_DIER_UIE);
    TIM10->CR1      |= (TIM_CR1_URS | TIM_CR1_CEN);

    // Таймер сигнала положения
    TIM9->PSC   = 0;
    TIM9->ARR   = 179;
    TIM9->CCR2  = 0;
    TIM9->SMCR  |= (TIM_SMCR_TS_2 | (TIM_SMCR_SMS_2 | TIM_SMCR_SMS_1 | TIM_SMCR_SMS_0));
    TIM9->DIER  |= (TIM_DIER_CC2IE | TIM_DIER_UIE);
    TIM9->CCMR1 |= TIM_CCMR1_CC1S_0;
    TIM9->CCER  |= (TIM_CCER_CC1NP | TIM_CCER_CC1P);
    TIM9->CR1   |= (TIM_CR1_URS | TIM_CR1_CEN);

    // Таймер событий между зубьями
    TIM1->PSC   = 99;
    TIM1->DIER  |= TIM_EGR_UG;
    TIM1->CR1   |= TIM_CR1_CEN;

    // Настройка прерываний
    NVIC_SetPriority(TIM1_BRK_TIM9_IRQn, 1);
    NVIC_EnableIRQ(TIM1_BRK_TIM9_IRQn);

    NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 2);
    NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);

    NVIC_SetPriority(TIM1_CC_IRQn, 3);
    NVIC_EnableIRQ(TIM1_CC_IRQn);
}

/**
 * Вычисление и сохранение текущего числа оборотов КВ
 */
void sync_freq_calc(void)
{
    uint8_t i;
    uint32_t stroke_period, freq_sum;

    // Вычисляем и сохраняем мгновенное значение числа оборотов 
    stroke_period = status.sync.stroke_period;
    status.sync.inst_freq = (stroke_period > 0) ? (30000000L / stroke_period) : 0;

    // Вычисляем и сохраняем среднее значение числа оборотов за 4 такта
    status.sync.freq_buf[status.sync.freq_head] = status.sync.inst_freq;

    if ((++status.sync.freq_head) == SYNC_STROKE_COUNT)
    {
        status.sync.freq_head = 0;
    }

    freq_sum = 0;

    for (i = 0; i < SYNC_STROKE_COUNT; i++)
    {
        freq_sum += status.sync.freq_buf[i];
    }

    status.sync.filt_freq = freq_sum / SYNC_STROKE_COUNT;
    status.rpm = status.sync.inst_freq; // or filt_freq ?
}

/**
 * Инициализация очереди событий
 */
void event_queue_init(sync_event_t events[], uint8_t n, uint16_t timing)
{
    uint8_t i, k;
    sync_event_t *event;

    for (i = 0; i < n; i++)
    {
        event = &events[i];
        k = (i < (n - 1)) ? (i + 1) : 0;
        event->next = &events[k];
        event->offset = i;
        event_update(event, timing, 720);
    }
}

/**
 * Обновление положения события с заданным шагом
 */
void event_update(sync_event_t *event, uint16_t target, uint16_t step)
{
    int16_t current;
    uint8_t q, r;

    current = event->timing;

    // Если событие сдвигается вперёд
    if (target > current)
    {
        current += step;

        if (current > target)
        {
            current = target;
        }
    }
    // Если событие сдвигается назад
    else if (target < current)
    {
        current -= step;

        if (current < target)
        {
            current = target;
        }
    }

    // Обновляем номер цилиндра и угол между тактами
    q = current / 180;
    r = current % 180;

    event->timing   = current;
    event->stroke   = q + event->offset;
    event->cogs     = r / 6;
    event->ang_mod  = r % 6;

    if (event->stroke >= 4)
    {
        event->stroke -= 4;
    }
}

/**
 * Обработчик прерывания TIM1_UP_TIM10_IRQn
 * @todo Добавить обработку ошибок
 */
void TIM1_UP_TIM10_IRQHandler(void)
{
    if ((TIM10->SR & TIM_SR_CC1IF))
    {
        TIM10->SR = ~TIM_SR_CC1IF;

        // Если синхронизировано
        if (TSTBIT(status.sync.flags1, SYNC_FLAGS1_SYNCED))
        {
            // Передний фронт
            if ((GPIOB->IDR & GPIO_IDR_IDR_8))
            {
                //TIM10->CNT = 0;
                status.sync.cogs = 0;

                /*if (status.sync.ref == 3)
                {
                    status.sync.ref = 0;
                    TIM10->CCER |= TIM_CCER_CC1P;
                }
                else
                {
                    status.sync.ref++;
                }*/

                SETBIT(status.flags1, FLAGS1_RUN);
                SETBIT(status.flags1, FLAGS1_STROKE);

                status.sync.prev_stroke_period = status.sync.stroke_period;
                status.sync.stroke_period = (TIM10->CCR1 < status.sync.prev_stroke_time) 
                    ? ((uint32_t)status.sync.stroke_ovf << 16) - (status.sync.prev_stroke_time - TIM10->CCR1)
                    : ((uint32_t)status.sync.stroke_ovf << 16) + (TIM10->CCR1 - status.sync.prev_stroke_time);
                status.sync.prev_stroke_time = TIM10->CCR1;
                status.sync.stroke_ovf = 0;

                TIM10->SR = ~TIM_SR_UIF;
            }
            // Задний фронт
            /*else
            {
                if (status.sync.ref == 0)
                {
                    uint8_t pos;

                    pos = TIM9->CNT;
                    TIM10->CCER &= ~TIM_CCER_CC1P;

                    if ((pos < 15) || (pos > 17))
                    {
                        CLEARBIT(status.sync.flags1, SYNC_FLAGS1_SYNCED);
                    }
                }
            }*/
        }
        // Если не синхронизировано
        else
        {
            // Передний фронт
            if ((GPIOB->IDR & GPIO_IDR_IDR_8))
            {
                TIM9->CNT = 0;
                TIM10->CNT = 0;
                TIM10->CCER |= TIM_CCER_CC1P;
                status.sync.cogs = 0;
                status.sync.stroke_period = 0;
                status.sync.stroke_ovf = 0;
            }
            // Задний фронт
            else
            {
                uint8_t pos, i;
                sync_event_t *event;

                TIM10->CCER &= ~TIM_CCER_CC1P;
                pos = TIM9->CNT;

                // Цил. 1
                if ((pos >= 15) && (pos <= 17))
                {
                    status.sync.stroke = 0;
                }
                // Цил. 2
                else if ((pos >= 11) && (pos <= 13))
                {
                    status.sync.stroke = 1;
                }
                // Цил. 3
                else if ((pos >= 7) && (pos <= 9))
                {
                    status.sync.stroke = 2;
                }
                // Цил. 4
                else if ((pos >= 3) && (pos <= 5))
                {
                    status.sync.stroke = 3;
                }

                // Устанавливаем события для текущего такта
                // События впрыска
                for (i = 0; i < INJ_COUNT; i++)
                {
                    event = &status.inj.events[i];

                    if (event->stroke == status.sync.stroke)
                    {
                        status.inj.event = event->next;
                    }
                }

                // События зажигания
                for (i = 0; i < IGN_COUNT; i++)
                {
                    // Начало накопления катушки зажигания
                    event = &status.ign.dwell_events[i];

                    if (event->stroke == status.sync.stroke)
                    {
                        status.ign.dwell_event = event->next;
                    }

                    // Завершение накоплнения катушки зажигания (искра)
                    event = &status.ign.spark_events[i];

                    if (event->stroke == status.sync.stroke)
                    {
                        status.ign.spark_event = event->next;
                    }
                }

                SETBIT(status.sync.flags1, SYNC_FLAGS1_SYNCED);
            }
        }
    }

    if ((TIM10->SR & TIM_SR_UIF))
    {
        TIM10->SR = ~TIM_SR_UIF;

        status.sync.stroke_ovf++;

        if (status.sync.stroke_ovf >= 16)
        {
            CLRBIT(status.flags1, FLAGS1_RUN);
        }
    }
}

/**
 * Обработчик прерывания TIM1_BRK_TIM9_IRQn
 * @todo Что, если время между зубьями больше 65535 мкс?
 */
void TIM1_BRK_TIM9_IRQHandler(void)
{
    /* Прерывание по совпадению канала 2 TIM9 */
    if ((TIM9->SR & TIM_SR_CC2IF))
    {
        uint8_t i, k;
        uint16_t cogs_time, cogs_period, angle;
        int16_t period;
        static uint16_t prev_cogs_time;
        static uint16_t prev_cogs_period;
        uint16_t mask;
        sync_event_t *event;

        TIM9->SR = ~TIM_SR_CC2IF;

        // Выполняем события только после синхронизации
        if (TSTBIT(status.sync.flags1, SYNC_FLAGS1_SYNCED))
        {
            // Обновляем регистр сравнения для генерирования следующего события 
            // через 6 градусов КВ
            angle = TIM9->CCR2;
            TIM9->CCR2 = (angle < 174) ? (angle + 6) : 0;

            // Вычисляем предполагаемое время между зубьями с учётом ускорения КВ
            cogs_time = TIM10->CNT;
            cogs_period = (cogs_time > prev_cogs_time) 
                ? (cogs_time - prev_cogs_time) 
                : (65536 - (prev_cogs_time - cogs_time));

            period = (cogs_period << 1) - prev_cogs_period;

            /*if (period < 0)
            {
                period = 0;
            }*/

            prev_cogs_time  = cogs_time;
            prev_cogs_period = cogs_period;

            // Сохраняем маску прерываний таймера событий
            mask = TIM1->DIER;

            // Завершение накопления катушки зажигания (искра)
            event = status.ign.spark_event;

            if (
                (event->cogs == status.sync.cogs)
                && (event->stroke == status.sync.stroke)
            )
            {
                TIM1->CCR1 = (event->ang_mod != 0) 
                    ? (((uint32_t)event->ang_mod * period) / 6) : 0;
                SETREG(TIM1->DIER, 1/*TIM_DIER_CC1IE*/);
            }

            // Начало накопления катушки зажигания
            event = status.ign.dwell_event;

            if (
                (event->cogs == status.sync.cogs)
                && (event->stroke == status.sync.stroke)
            )
            {
                TIM1->CCR2 = (event->ang_mod != 0)
                    ? (((uint32_t)event->ang_mod * period) / 6) : 0;
                SETREG(TIM1->DIER, 2/*TIM_DIER_CC2IE*/);
            }

            // Открытие форсунки
            event = status.inj.event;

            if (
                (event->cogs == status.sync.cogs)
                && (event->stroke == status.sync.stroke)
            )
            {
                TIM1->CCR3 = (event->ang_mod != 0) 
                    ? (((uint32_t)event->ang_mod * period) / 6) : 0;
                SETREG(TIM1->DIER, 3/*TIM_DIER_CC3IE*/);
            }

            // Фазовое окно контроля детонации
            /*event = status.knock.event;

            if (
                (event->cogs == status.sync.cogs)
                && (event->stroke == status.sync.stroke)
            )
            {
                TIM1->CCR4 = (event->ang_mod != 0) 
                    ? (((uint32_t)event->ang_mod * period) / 6) : 0;
                SETREG(TIM1->DIER, 4);
            }*/

            // Применяем сохранённую маску прерываний таймера событий для того, 
            // чтобы не пропустить события из-за ускорения КВ 
            TIM1->EGR |= mask;

            // Увеличиваем номер зуба
            if ((++status.sync.cogs) == 30)
            {
                status.sync.cogs = 0;

                // Увеличиваем номер текущего цилиндра через каждые 30 зубьев (180 градусов)
                if ((++status.sync.stroke) == 4)
                {
                    status.sync.stroke = 0;
                }
            }
        }
    }

    /* Прерывание по переполнению TIM9 */
    if ((TIM9->SR & TIM_SR_UIF))
    {
        TIM9->SR = ~TIM_SR_UIF;
    }
}

/**
 * Events ISR
 */
void TIM1_CC_IRQHandler(void)
{
    uint16_t sr;
    sync_event_t *event;

    sr = TIM1->SR & TIM1->DIER;

    // Завершение накопления катушки зажигания (искра)
    if ((sr & TIM_SR_CC1IF))
    {
        TIM1->SR = ~TIM_SR_CC1IF;
        CLRREG(TIM1->DIER, 1/*TIM_DIER_CC1IE*/);

        event = status.ign.spark_event;
        GPIOD->BSRRH = GPIO_ODR_ODR_15;
        status.ign.spark_event = event->next;

        if (event->timing != status.ign.spark_timing)
        {
            event_update(event, status.ign.spark_timing, config.ign_timing_step);
        }
    }

    // Начало накопления катушки зажигания
    if ((sr & TIM_SR_CC2IF))
    {
        TIM1->SR = ~TIM_SR_CC2IF;
        CLRREG(TIM1->DIER, 2/*TIM_DIER_CC2IE*/);

        event = status.ign.dwell_event;
        GPIOD->BSRRL = GPIO_ODR_ODR_15;
        status.ign.dwell_event = event->next;

        if (event->timing != status.ign.dwell_timing)
        {
            event_update(event, status.ign.dwell_timing, config.ign_timing_step);
        }
    }

    // Открытие форсунки
    if ((sr & TIM_SR_CC3IF))
    {
        TIM1->SR = ~TIM_SR_CC3IF;
        CLRREG(TIM1->DIER, 3/*TIM_DIER_CC3IE*/);

        event = status.inj.event;
        inj_start(event->offset);
        status.inj.event = event->next;

        if (event->timing != status.inj.timing)
        {
            event_update(event, status.inj.timing, config.inj_timing_step);
        }
    }

    // Фазовое окно контроля детонации
    /*if ((sr & TIM_SR_CC4IF))
    {
        TIM1->SR = ~TIM_SR_CC4IF;
        CLRREG(TIM1->DIER, 4);

        event = status.knock.event;
        status.knock.event = event->next;

        if (event->timing != status.knock.timing)
        {
            event_update(event, status.knock.timing, 6);
        }
    }*/
}
