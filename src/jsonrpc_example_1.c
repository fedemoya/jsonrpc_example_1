/* Copyright 2017, Eric Pernia
 * All rights reserved.
 *
 * This file is part of sAPI Library.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <string.h>
#include <stdlib.h>

#include "sapi.h"

#include "allocator.h"
#include "ejsonrpc.h"
#include "frozen.h"

DEBUG_PRINT_ENABLE;


int sum_response_printer(struct json_out *out, va_list *ap) {
    int *method_response = va_arg(*ap, int *);
    return json_printf(out, "%d", *method_response);
}

int sum(char *params, void **method_response, json_printf_callback_t *response_printer) {

    struct json_token t;

    long int a, b;

    char str[10];
    char *endptr;

    json_scanf_array_elem(params, strlen(params), "", 0, &t);
    strncpy(str, t.ptr, t.len);
    a = strtol(str, &endptr, 10);

    memset(str, '\0', sizeof(str));

    json_scanf_array_elem(params, strlen(params), "", 1, &t);
    strncpy(str, t.ptr, t.len);
    b = strtol(str, &endptr, 10);

    long int *result = gen_alloc(sizeof(long int));
    *result = a + b;

    *method_response = result;
    *response_printer = sum_response_printer;

    return 0;
}

jsonrpc_method_t methods[] = {
    {
        "sum",
        sum
    }
};

int main(void)
{

   boardConfig();

   debugPrintConfigUart( UART_USB, 115200 );

   uint8_t receivedByte;
   int index = 0;
   char uart_received_string[100];
   char jsonrpc_response[100];

   memset(uart_received_string, '\0', sizeof(uart_received_string));

   uartWriteString(UART_USB, "Insert your jsonrpc request here:\n\r");

   while( TRUE )
   {
      if (uartReadByte(UART_USB, &receivedByte)) {
         if (receivedByte == '\r' && index < 100 - 1) {
             if (execute_jsonrpc(uart_received_string, strlen(uart_received_string), methods, sizeof(methods)/sizeof(jsonrpc_method_t), jsonrpc_response, sizeof(jsonrpc_response))) {
                 uartWriteString(UART_USB, "\n\rJSONRPC response:\n\r");
                 uartWriteString(UART_USB, jsonrpc_response);
             } else {
                 uartWriteString(UART_USB, "No output written by execute_jsonrpc method. Something went wrong...");
             }
             index = 0;
             memset(uart_received_string, '\0', sizeof(uart_received_string));
             uartWriteString(UART_USB, "\n\rInsert your jsonrpc request here:\n\r");
         } else if (index < 100 - 2) {
             uart_received_string[index] = (char)receivedByte;
             index++;
         }
      }
   }

   // NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa se ejecuta
   // directamenteno sobre un microcontroladore y no es llamado por ningun
   // Sistema Operativo, como en el caso de un programa para PC.
   return 0;
}

