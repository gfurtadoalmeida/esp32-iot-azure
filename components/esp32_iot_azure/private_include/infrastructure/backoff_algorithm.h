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
 * @file backoff_algorithm.h
 * @brief API for calculating backoff period for retry attempts using
 * exponential backoff with jitter algorithm.
 * This library represents the "Full Jitter" backoff strategy explained in the
 * following document.
 * https://aws.amazon.com/blogs/architecture/exponential-backoff-and-jitter/
 *
 */

// Reference: https://github.com/Azure-Samples/iot-middleware-freertos-samples/blob/main/demos/projects/ESPRESSIF/adu/components/sample-azure-iot/backoff_algorithm.c

#ifndef __ESP32_IOT_AZURE_INFRA_BACKOFF_ALGO_H__
#define __ESP32_IOT_AZURE_INFRA_BACKOFF_ALGO_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @ingroup backoff_algorithm_constants
 * @brief Constant to represent unlimited number of retry attempts.
 */
#define BACKOFF_ALGORITHM_RETRY_FOREVER 0

    /**
     * @ingroup backoff_algorithm_enum_types
     * @brief Status for @ref backoff_algorithm_get_next.
     */
    typedef enum
    {
        BACKOFF_ALGORITHM_SUCCESS = 0,          /** @brief The function successfully calculated the next back-off value. */
        BACKOFF_ALGORITHM_RETRIES_EXHAUSTED = 1 /** @brief The function exhausted all retry attempts. */
    } backoff_algorithm_status_t;

    /**
     * @ingroup backoff_algorithm_struct_types
     * @brief Represents parameters required for calculating the back-off delay for the
     * next retry attempt.
     */
    typedef struct
    {
        /**
         * @brief The maximum backoff delay (in milliseconds) between consecutive retry attempts.
         */
        uint16_t max_backoff_delay;

        /**
         * @brief The total number of retry attempts completed.
         * This value is incremented on every call to #BackoffAlgorithm_GetNextBackoff API.
         */
        uint32_t attempts_done;

        /**
         * @brief The maximum backoff value (in milliseconds) for the next retry attempt.
         */
        uint16_t next_jitter_max;

        /**
         * @brief The maximum number of retry attempts.
         */
        uint32_t max_retry_attempts;
    } backoff_algorithm_context_t;

    /**
     * @brief Initialize the context for using backoff algorithm. The parameters
     * are required for calculating the next retry backoff delay.
     * @note This function must be called by the application before the first new retry attempt.
     * @param[out] context The context to initialize with parameters required
     * for the next backoff delay calculation function.
     * @param[in] backoff_base The base value (in milliseconds) of backoff delay to
     * use in the exponential backoff and jitter model.
     * @param[in] max_back_off The maximum backoff delay (in milliseconds) between
     * consecutive retry attempts.
     * @param[in] max_attempts The maximum number of retry attempts. Set the value to
     * #BACKOFF_ALGORITHM_RETRY_FOREVER to retry for ever.
     */
    /* @[define_backoff_algorithm_initialize] */
    void backoff_algorithm_initialize(backoff_algorithm_context_t *context,
                                      uint16_t backoff_base,
                                      uint16_t max_back_off,
                                      uint32_t max_attempts);
    /* @[define_backoff_algorithm_initialize] */

    /**
     * @brief Simple exponential backoff and jitter function that provides the
     * delay value for the next retry attempt.
     * @details After a failure of an operation that needs to be retried, the application
     * should use this function to obtain the backoff delay value for the next retry,
     * and then wait for the backoff time period before retrying the operation.
     * @param[in] context Structure containing parameters for the next backoff
     * value calculation.
     * @param[out] next_backoff This will be populated with the backoff value (in milliseconds)
     * for the next retry attempt. The value does not exceed the maximum backoff delay
     * configured in the context.
     * @return #BACKOFF_ALGORITHM_SUCCESS after a successful sleep;
     * #BACKOFF_ALGORITHM_RETRIES_EXHAUSTED when all attempts are exhausted.
     */
    /* @[define_backoff_algorithm_get_next] */
    backoff_algorithm_status_t backoff_algorithm_get_next(backoff_algorithm_context_t *context,
                                                          uint16_t *next_backoff);
    /* @[define_backoff_algorithm_get_next] */

#endif
#ifdef __cplusplus
}
#endif