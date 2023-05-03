/*
 * backoffAlgorithm v1.0.0
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/**
 * @file backoff_algorithm.c
 * @brief Implementation of the backoff algorithm API for a "Full Jitter" exponential backoff
 * with jitter strategy.
 */

#include "infrastructure/backoff_algorithm.h"
#include <assert.h>
#include <stddef.h>
#include "esp_random.h"

void backoff_algorithm_initialize(backoff_algorithm_context_t *context,
                                  uint16_t backoff_base,
                                  uint16_t max_back_off,
                                  uint32_t max_attempts)
{
    // The total number of retry attempts is zero at initialization.
    context->attempts_done = 0U;
    context->next_jitter_max = backoff_base;
    context->max_backoff_delay = max_back_off;
    context->max_retry_attempts = max_attempts;
}

backoff_algorithm_status_t backoff_algorithm_get_next(backoff_algorithm_context_t *context,
                                                      uint16_t *next_backoff)
{
    backoff_algorithm_status_t status = BACKOFF_ALGORITHM_SUCCESS;

    /* If BACKOFF_ALGORITHM_RETRY_FOREVER is set to 0, try forever. */
    if ((context->attempts_done < context->max_retry_attempts) ||
        (context->max_retry_attempts == BACKOFF_ALGORITHM_RETRY_FOREVER))
    {
        /* The next backoff value is a random value between 0 and the maximum jitter value
         * for the retry attempt. */

        uint32_t random_value = esp_random();

        /* Choose a random value for back-off time between 0 and the max jitter value. */
        *next_backoff = (uint16_t)(random_value % (context->next_jitter_max + 1U));

        /* Increment the retry attempt. */
        context->attempts_done++;

        /* Double the max jitter value for the next retry attempt, only
         * if the new value will be less than the max backoff time value. */
        if (context->next_jitter_max < (context->max_backoff_delay / 2U))
        {
            context->next_jitter_max += context->next_jitter_max;
        }
        else
        {
            context->next_jitter_max = context->max_backoff_delay;
        }
    }
    else
    {
        /* When max retry attempts are exhausted, let application know by
         * returning BackoffAlgorithmRetriesExhausted. Application may choose to
         * restart the retry process after calling BackoffAlgorithm_InitializeParams(). */
        status = BACKOFF_ALGORITHM_RETRIES_EXHAUSTED;
    }

    return status;
}
