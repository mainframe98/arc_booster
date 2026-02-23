/* This is a generated file, edit arc_booster.stub.php instead.
 * Stub hash: f4b64a8a4296e9b83548acae58e2e758439c6f79 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ArcBooster_idx, 0, 2, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
	ZEND_ARG_TYPE_MASK(0, key, MAY_BE_STRING|MAY_BE_BOOL|MAY_BE_LONG|MAY_BE_DOUBLE|MAY_BE_NULL, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, default, IS_MIXED, 0, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ArcBooster_string_cast, 0, 1, IS_STRING, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, value, Stringable, MAY_BE_STRING|MAY_BE_BOOL|MAY_BE_LONG|MAY_BE_DOUBLE|MAY_BE_NULL, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ArcBooster_nonempty_string, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ArcBooster_nonempty_stringlike, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, value, Stringable, MAY_BE_STRING|MAY_BE_NULL, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ArcBooster_nonempty_scalar, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, value, Stringable, MAY_BE_STRING|MAY_BE_BOOL|MAY_BE_LONG|MAY_BE_DOUBLE|MAY_BE_NULL, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ArcBooster_coalesce, 0, 0, IS_MIXED, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, args, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_ArcBooster_nonempty arginfo_ArcBooster_coalesce

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ArcBooster_mpull, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, list, Traversable, MAY_BE_ARRAY, NULL)
	ZEND_ARG_TYPE_INFO(0, method, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, key_method, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ArcBooster_ppull, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, list, Traversable, MAY_BE_ARRAY, NULL)
	ZEND_ARG_TYPE_INFO(0, property, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, key_property, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ArcBooster_ipull, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, list, Traversable, MAY_BE_ARRAY, NULL)
	ZEND_ARG_TYPE_MASK(0, index, MAY_BE_STRING|MAY_BE_BOOL|MAY_BE_LONG|MAY_BE_DOUBLE|MAY_BE_NULL, NULL)
	ZEND_ARG_TYPE_MASK(0, key_index, MAY_BE_STRING|MAY_BE_BOOL|MAY_BE_LONG|MAY_BE_DOUBLE|MAY_BE_NULL, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ArcBooster_head, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, arr, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_ArcBooster_last arginfo_ArcBooster_head

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ArcBooster_array_mergev, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, arrayv, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ArcBooster_array_fuse, 0, 0, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, list, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ArcBooster_is_utf8_slowly, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, only_bmp, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ArcBooster_utf8v, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, byte_limit, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ArcBooster_utf8v_codepoints, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ArcBooster_utf8ize, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ArcBooster_render_ast, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, source_code, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_FUNCTION(ArcBooster_idx);
ZEND_FUNCTION(ArcBooster_string_cast);
ZEND_FUNCTION(ArcBooster_nonempty_string);
ZEND_FUNCTION(ArcBooster_nonempty_stringlike);
ZEND_FUNCTION(ArcBooster_nonempty_scalar);
ZEND_FUNCTION(ArcBooster_coalesce);
ZEND_FUNCTION(ArcBooster_nonempty);
ZEND_FUNCTION(ArcBooster_mpull);
ZEND_FUNCTION(ArcBooster_ppull);
ZEND_FUNCTION(ArcBooster_ipull);
ZEND_FUNCTION(ArcBooster_head);
ZEND_FUNCTION(ArcBooster_last);
ZEND_FUNCTION(ArcBooster_array_mergev);
ZEND_FUNCTION(ArcBooster_array_fuse);
ZEND_FUNCTION(ArcBooster_is_utf8_slowly);
ZEND_FUNCTION(ArcBooster_utf8v);
ZEND_FUNCTION(ArcBooster_utf8v_codepoints);
ZEND_FUNCTION(ArcBooster_utf8ize);
ZEND_FUNCTION(ArcBooster_render_ast);

static const zend_function_entry ext_functions[] = {
	ZEND_RAW_FENTRY(ZEND_NS_NAME("ArcBooster", "idx"), zif_ArcBooster_idx, arginfo_ArcBooster_idx, ZEND_ACC_COMPILE_TIME_EVAL, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("ArcBooster", "string_cast"), zif_ArcBooster_string_cast, arginfo_ArcBooster_string_cast, ZEND_ACC_COMPILE_TIME_EVAL, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("ArcBooster", "nonempty_string"), zif_ArcBooster_nonempty_string, arginfo_ArcBooster_nonempty_string, ZEND_ACC_COMPILE_TIME_EVAL, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("ArcBooster", "nonempty_stringlike"), zif_ArcBooster_nonempty_stringlike, arginfo_ArcBooster_nonempty_stringlike, ZEND_ACC_COMPILE_TIME_EVAL, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("ArcBooster", "nonempty_scalar"), zif_ArcBooster_nonempty_scalar, arginfo_ArcBooster_nonempty_scalar, ZEND_ACC_COMPILE_TIME_EVAL, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("ArcBooster", "coalesce"), zif_ArcBooster_coalesce, arginfo_ArcBooster_coalesce, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("ArcBooster", "nonempty"), zif_ArcBooster_nonempty, arginfo_ArcBooster_nonempty, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("ArcBooster", "mpull"), zif_ArcBooster_mpull, arginfo_ArcBooster_mpull, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("ArcBooster", "ppull"), zif_ArcBooster_ppull, arginfo_ArcBooster_ppull, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("ArcBooster", "ipull"), zif_ArcBooster_ipull, arginfo_ArcBooster_ipull, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("ArcBooster", "head"), zif_ArcBooster_head, arginfo_ArcBooster_head, ZEND_ACC_COMPILE_TIME_EVAL, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("ArcBooster", "last"), zif_ArcBooster_last, arginfo_ArcBooster_last, ZEND_ACC_COMPILE_TIME_EVAL, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("ArcBooster", "array_mergev"), zif_ArcBooster_array_mergev, arginfo_ArcBooster_array_mergev, ZEND_ACC_COMPILE_TIME_EVAL, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("ArcBooster", "array_fuse"), zif_ArcBooster_array_fuse, arginfo_ArcBooster_array_fuse, ZEND_ACC_COMPILE_TIME_EVAL, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("ArcBooster", "is_utf8_slowly"), zif_ArcBooster_is_utf8_slowly, arginfo_ArcBooster_is_utf8_slowly, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("ArcBooster", "utf8v"), zif_ArcBooster_utf8v, arginfo_ArcBooster_utf8v, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("ArcBooster", "utf8v_codepoints"), zif_ArcBooster_utf8v_codepoints, arginfo_ArcBooster_utf8v_codepoints, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("ArcBooster", "utf8ize"), zif_ArcBooster_utf8ize, arginfo_ArcBooster_utf8ize, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("ArcBooster", "render_ast"), zif_ArcBooster_render_ast, arginfo_ArcBooster_render_ast, 0, NULL, NULL)
	ZEND_FE_END
};
