#include "extension_functions.h"

void convert_do2php(DATA_OBJECT data, zval* pzv_val) {
	switch(GetType(data)) {
		case FLOAT:
			ZVAL_DOUBLE(pzv_val, DOToDouble(data));
			break;
		case INTEGER:
			ZVAL_LONG(pzv_val, DOToLong(data));
			break;
		case INSTANCE_NAME:
			// This is an object, let's try to make is a class, if can't, make it an array
			break;
		case INSTANCE_ADDRESS:
			// This is an object, let's try to make is a class, if can't, make it an array
			break;
		case FACT_ADDRESS:
			// This is a fact, let's try to make is a class, if can't, make it an array
			break;
		case STRING:
		case SYMBOL:
			ZVAL_STRING(pzv_val, DOToString(data), TRUE);
			break;
		case MULTIFIELD:
			// Let's convert this to array
			break;
	}
}

/**
 * Call the PHP's function. The first argument must be string. Follow these rules to convert type to PHP:
 * FLOAT => float
 * INTEGER => int
 * SYMBOL => string
 * STRING => string
 * MULTIFIELD => array of string | PHP class if the field is a template
 * FACT_ADDRESS => int
 */
void php_call(void* pv_env, DATA_OBJECT_PTR pdo_return_val) {

	// Test the argument count is larger than 1
	if(EnvArgCountCheck(pv_env, "php_call", AT_LEAST, 1) == -1) {
		EnvSetpType(pv_env, pdo_return_val, STRING);
		EnvSetpValue(pv_env, pdo_return_val, EnvAddSymbol(pv_env, ""));
		return ;
	}


	// Test if the first argument is string(function name)
	DATA_OBJECT do_php_function;
	if(!EnvArgTypeCheck(pv_env, "php_call", 1, STRING, &do_php_function)) {
		EnvSetpType(pv_env, pdo_return_val, STRING);
		EnvSetpValue(pv_env, pdo_return_val, EnvAddSymbol(pv_env, ""));
		return ;
	}

	// Read the function name
	const char* str_php_function = DOToString(do_php_function);

	// Copy the function name to php variable
	zval* pzv_function_name;
	MAKE_STD_ZVAL(pzv_function_name);
	ZVAL_STRING(pzv_function_name, str_php_function, 1);

	// Get the parameters count
	int c = EnvRtnArgCount(pv_env) - 1;
	zend_uint i_param_count = c;

	// Initialize the paramter array
	zval** ppzv_params = (zval**) emalloc(c * sizeof(zval*));
	zval *pzv_php_ret_val;

	// Initialize the return php value
	MAKE_STD_ZVAL(pzv_php_ret_val);

	// Setup theinput parameters
	for(int i = 0; i < c; i++) {
		// Initialize the php value
		zval* val;
		MAKE_STD_ZVAL(val);

		// Getting the Data Object
		DATA_OBJECT o;

		EnvRtnUnknown(pv_env, 2 + i, &o); // Skipping the first once since it is the function name
		convert_do2php(o, val);
		ppzv_params[i] = val;
	}

	// Setup the default return value
	EnvSetpType(pv_env, pdo_return_val, STRING);
	EnvSetpValue(pv_env, pdo_return_val, EnvAddSymbol(pv_env, ""));

	// Call the functions
	if (call_user_function( EG(function_table), NULL /* no object */, pzv_function_name, pzv_php_ret_val, i_param_count, ppzv_params TSRMLS_CC) == SUCCESS) {
		switch(Z_TYPE_P(pzv_php_ret_val)) {
			case IS_LONG:
				EnvSetpType(pv_env, pdo_return_val, INTEGER);
				EnvSetpValue(pv_env, pdo_return_val, EnvAddLong(pv_env, Z_LVAL_P(pzv_php_ret_val)));
				break;
			case IS_DOUBLE:
				EnvSetpType(pv_env, pdo_return_val, FLOAT);
				EnvSetpValue(pv_env, pdo_return_val, EnvAddDouble(pv_env, Z_DVAL_P(pzv_php_ret_val)));
				break;
			case IS_ARRAY:
				// TODO Make this a multifiled
				break;
			case IS_OBJECT:
				// TODO Make this a fact(with template or not)
				break;
			case IS_STRING:
				EnvSetpType(pv_env, pdo_return_val, STRING);
				EnvSetpValue(pv_env, pdo_return_val, EnvAddSymbol(pv_env, Z_STRVAL_P(pzv_php_ret_val)));
				break;
		}
	}

	// Destroy all the php parameter variables
	for(int i = 0; i < c; i++) {
		zval_ptr_dtor(&ppzv_params[i]);
	}
	// Destroy the php return variable
	zval_ptr_dtor(&pzv_php_ret_val);
	// Destroy the php function name variable
	zval_ptr_dtor(&pzv_function_name);
}