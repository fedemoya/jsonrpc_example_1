
#include "ejsonrpc.h"

#include <string.h>
#include <stdlib.h>

#include "sapi.h"

#include "allocator.h"


int execute_jsonrpc(char *jsonrpc_req, int jsonrpc_size, jsonrpc_method_t *methods, int methods_size, char *jsonrpc_res, int response_size) {
	
	char *jsonrpc_version, *method_name,*params;
	int id = 0;
	int read_bytes = 0;
	int written_bytes = 0;

    struct json_out out = JSON_OUT_BUF(jsonrpc_res, response_size);

	const char *jsonrpc_request_template = "{jsonrpc:%Q, method:%Q, params:%Q, id:%d}";
	const char *error_response_template = "{jsonrpc: \"2.0\", error: {code:%d, message:%Q}, id:%d}";
	const char *success_response_template = "{jsonrpc: \"2.0\", result: %M, id: %d}";

	// Parse the jsonrpc request
	read_bytes = json_scanf(jsonrpc_req, jsonrpc_size, jsonrpc_request_template, &jsonrpc_version,	&method_name, &params, &id);

	if (read_bytes <= 0) {
		written_bytes = json_printf(&out, error_response_template, PARSE_ERROR, "", id);
		return read_bytes;
	}

	if (jsonrpc_version == NULL || strcmp(jsonrpc_version, "2.0") != 0 || method_name == NULL) {
		written_bytes = json_printf(&out, error_response_template, INVALID_REQUEST, "", id);
		return written_bytes;
	}

	void *method_response;
	json_printf_callback_t response_printer;

	int i;
	for (i = 0; i < methods_size; ++i) {
		if (strcmp(methods[i].method_name, method_name) == 0) {
		    gen_free(method_name);
			// Execute the corresponding method
			int result = methods[i].method(params, &method_response, &response_printer);

			// TODO compare written_bytes with response_size
			if (result >= 0) {
				written_bytes = json_printf(&out, success_response_template, response_printer, method_response, id);
				return written_bytes;
			} else {
				written_bytes = json_printf(&out, error_response_template, result, "", id);
				return written_bytes;
			}
		}
	}

	written_bytes = json_printf(&out, error_response_template, METHOD_NOT_FOUND, "", id);
	return written_bytes;
}
