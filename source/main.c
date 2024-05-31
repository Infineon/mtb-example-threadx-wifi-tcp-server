/******************************************************************************
 * File Name:   main.c
 *
 * Description: This is the source code for TCP Server Example in ModusToolbox.
 * In this example, TCP server establishes a connection with a TCP client.
 * Once the connection completes successfully, the server allows the user to
 * send LED ON/OFF command to the TCP client and the client responds by sending
 * an acknowledgement message to the server.
 *
 * Related Document: See README.md
 *
 *
 *********************************************************************************
 * Copyright 2020-2023, Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software") is owned by Cypress Semiconductor Corporation
 * or one of its affiliates ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products.  Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 *******************************************************************************/

/* Header file includes */
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"

/* RTOS header file */
#include "cyabs_rtos.h"

/* TCP server task header file. */
#include "tcp_server.h"

/*******************************************************************************
 * Macros
 ********************************************************************************/
/* RTOS related macros for TCP server task. */

#define TCP_SERVER_TASK_STACK_SIZE (1024 * 5)
#define TCP_SERVER_TASK_PRIORITY (CY_RTOS_PRIORITY_NORMAL)
static uint64_t tcp_task_stack[TCP_SERVER_TASK_STACK_SIZE / 8];

cy_thread_t tcp_task_handle;

/* Queue lengths of message queues used in this project */
#define SINGLE_ELEMENT_QUEUE (1u)

/*******************************************************************************
 * Global Variables
 ********************************************************************************/
/* Queue handler */
cy_queue_t led_command_q;

/*******************************************************************************
 * Function Name: main
 ********************************************************************************
 * Summary:
 *  System entrance point. This function sets up user tasks and then starts
 *  the RTOS scheduler.
 *
 * Parameters:
 *  void
 *
 * Return:
 *  int
 *
 *******************************************************************************/
int main()
{
    cy_rslt_t result;

    /* Initialize the board support package. */
    result = cybsp_init();
    CY_ASSERT(result == CY_RSLT_SUCCESS);

    /* To avoid compiler warnings. */
    (void)result;

    /* Enable global interrupts. */
    __enable_irq();

    /* Initialize retarget-io to use the debug UART port. */
    cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);

    /* \x1b[2J\x1b[;H - ANSI ESC sequence to clear screen. */
    printf("\x1b[2J\x1b[;H");
    printf("===============================================================\n");
    printf("CE238661 - Connectivity Example: TCP Server\n");
    printf("===============================================================\n\n");

    /* Initialize a queue to receive command. */
    cy_rtos_queue_init(&led_command_q, SINGLE_ELEMENT_QUEUE, sizeof(uint8_t));

    result = cy_rtos_thread_create(&tcp_task_handle,
                                   tcp_server_task,
                                   "tcp_server_task",
                                   &tcp_task_stack,
                                   TCP_SERVER_TASK_STACK_SIZE,
                                   TCP_SERVER_TASK_PRIORITY,
                                   0);

    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }
}
/* [] END OF FILE */
