#include "php_arc_booster.h"
#include "arc_booster_arginfo.h"
#include "php_ini.h"
#include "ext/spl/spl_exceptions.h"
#include "ext/spl/spl_iterators.h"
#include "ext/standard/info.h"
#include "zend_exceptions.h"
#include "zend_hash.h"
#include "zend_language_parser.h"
#include "zend_language_scanner.h"
#include "zend_language_scanner_defs.h"
#include "zend_portability.h"
#include "zend_smart_str.h"

PHP_INI_BEGIN()
	STD_ZEND_INI_BOOLEAN("arc_booster.disable_ast_modifications", "0", PHP_INI_PERDIR, OnUpdateBool, disable_ast_mod, zend_arc_booster_globals, arc_booster_globals)
	STD_ZEND_INI_BOOLEAN("arc_booster.use_mbstring", "1", PHP_INI_PERDIR, OnUpdateBool, use_mbstring, zend_arc_booster_globals, arc_booster_globals)
PHP_INI_END()

// zend_ast_is_decl was introduced in PHP 8.5.
#if PHP_VERSION_ID < 80500
static zend_always_inline bool zend_ast_is_decl(zend_ast *ast) {
	return zend_ast_is_special(ast) && ast->kind >= ZEND_AST_FUNC_DECL;
}
#endif

/**
 * Fetch a value from a HashTable for a given index.
 * This function does not add to the retrieved value's refcount.
 *
 * @param ht HashTable
 * @param dim Index
 * @param dest Destination in which to put the found value. It will be NULL if there is no such value.
 * @param null_is_deprecated Indicates if using null as the index issues deprecation warnings. Many of Arcanist's functions do not.
 * @return Whether or not the fetch was successful. This doesn't indicate that there was an element, only that the fetch happened without an Error.
 */
static zend_always_inline zend_result arc_booster_ht_fetch(const HashTable *ht, const zval *dim, zval **dest, const bool null_is_deprecated) {
	switch (Z_TYPE_P(dim)) {
		case IS_STRING:
			*dest = zend_symtable_find(ht, Z_STR_P(dim));
			break;
		case IS_LONG:
			*dest = zend_hash_index_find(ht, Z_LVAL_P(dim));
			break;
		case IS_NULL:
			if (null_is_deprecated) {
				zend_error(E_DEPRECATED, "Using null as an array offset is deprecated, use an empty string instead");
			}
			*dest = zend_hash_find(ht, ZSTR_EMPTY_ALLOC());
			break;
		case IS_DOUBLE: {
			zend_long lval = zend_dval_to_lval_safe(Z_DVAL_P(dim));
			if (UNEXPECTED(EG(exception))) {
				return FAILURE;
			}

			*dest = zend_hash_index_find(ht, lval);
			break;
		}
		case IS_FALSE:
			*dest = zend_hash_index_find(ht, 0);
			break;
		case IS_TRUE:
			*dest = zend_hash_index_find(ht, 1);
			break;
		case IS_RESOURCE:
			zend_use_resource_as_offset(dim);
			if (UNEXPECTED(EG(exception))) {
				return FAILURE;
			}

			*dest = zend_hash_index_find(ht, Z_RES_HANDLE_P(dim));
			break;
		default:
			return FAILURE;
	}

	return SUCCESS;
}

enum ARC_BOOSTER_REPLACEMENT_TYPE {
	WHOLESALE,
	MBSTRING_WITH_CAST,
	MBSTRING,
	DROP_FUNCTION_CALL,
	NEWV,
	IDX,
	CALLABLE,
	INVALID,
};

typedef struct {
	char *original_function_name;
	char *replacement_name;
	enum ARC_BOOSTER_REPLACEMENT_TYPE replacement_type;
	uint32_t arg_count;
	bool arg_count_is_minimum;
} arc_booster_replacement;

static arc_booster_replacement replacements[] = {
	{
		.original_function_name = "head_key",
		.replacement_name = "array_key_first",
		.replacement_type = WHOLESALE,
		.arg_count = 1,
		.arg_count_is_minimum = false,
	},
	{
		.original_function_name = "last_key",
		.replacement_name = "array_key_last",
		.replacement_type = WHOLESALE,
		.arg_count = 1,
		.arg_count_is_minimum = false,
	},
	{
		.original_function_name = "phutil_fwrite_nonblocking_stream",
		.replacement_name = "fwrite",
		.replacement_type = WHOLESALE,
		.arg_count = 2,
		.arg_count_is_minimum = false,
	},
	{
		.original_function_name = "phutil_hashes_are_identical",
		.replacement_name = "hash_equals",
		.replacement_type = WHOLESALE,
		.arg_count = 2,
		.arg_count_is_minimum = false,
	},
	{
		.original_function_name = "phutil_is_natural_list",
		.replacement_name = "array_is_list",
		.replacement_type = WHOLESALE,
		.arg_count = 1,
		.arg_count_is_minimum = false,
	},
	{
		.original_function_name = "head",
		.replacement_name = "ArcBooster\\head",
		.replacement_type = WHOLESALE,
		.arg_count = 1,
		.arg_count_is_minimum = false,
	},
	{
		.original_function_name = "last",
		.replacement_name = "ArcBooster\\last",
		.replacement_type = WHOLESALE,
		.arg_count = 1,
		.arg_count_is_minimum = false,
	},
	{
		.original_function_name = "phutil_string_cast",
		.replacement_name = "ArcBooster\\string_cast",
		.replacement_type = WHOLESALE,
		.arg_count = 1,
		.arg_count_is_minimum = false,
	},
	{
		.original_function_name = "phutil_nonempty_string",
		.replacement_name = "ArcBooster\\nonempty_string",
		.replacement_type = WHOLESALE,
		.arg_count = 1,
		.arg_count_is_minimum = false,
	},
	{
		.original_function_name = "phutil_nonempty_scalar",
		.replacement_name = "ArcBooster\\nonempty_scalar",
		.replacement_type = WHOLESALE,
		.arg_count = 1,
		.arg_count_is_minimum = false,
	},
	{
		.original_function_name = "coalesce",
		.replacement_name = "ArcBooster\\coalesce",
		.replacement_type = WHOLESALE,
		.arg_count = 0,
		.arg_count_is_minimum = true,
	},
	{
		.original_function_name = "nonempty",
		.replacement_name = "ArcBooster\\nonempty",
		.replacement_type = WHOLESALE,
		.arg_count = 0,
		.arg_count_is_minimum = true,
	},
	{
		.original_function_name = "mpull",
		.replacement_name = "ArcBooster\\mpull",
		.replacement_type = WHOLESALE,
		.arg_count = 2,
		.arg_count_is_minimum = true,
	},
	{
		.original_function_name = "ppull",
		.replacement_name = "ArcBooster\\ppull",
		.replacement_type = WHOLESALE,
		.arg_count = 2,
		.arg_count_is_minimum = true,
	},
	{
		.original_function_name = "ipull",
		.replacement_name = "ArcBooster\\ipull",
		.replacement_type = WHOLESALE,
		.arg_count = 2,
		.arg_count_is_minimum = true,
	},
	{
		.original_function_name = "array_mergev",
		.replacement_name = "ArcBooster\\array_mergev",
		.replacement_type = WHOLESALE,
		.arg_count = 1,
		.arg_count_is_minimum = false,
	},
	{
		.original_function_name = "array_fuse",
		.replacement_name = "ArcBooster\\array_fuse",
		.replacement_type = WHOLESALE,
		.arg_count = 1,
		.arg_count_is_minimum = false,
	},
	{
		.original_function_name = "id",
		.replacement_name = NULL,
		.replacement_type = DROP_FUNCTION_CALL,
		.arg_count = 1,
		.arg_count_is_minimum = false,
	},
	{
		.original_function_name = "idx",
		.replacement_name = "ArcBooster\\idx",
		.replacement_type = IDX,
		.arg_count = 2,
		.arg_count_is_minimum = true,
	},
	{
		.original_function_name = "newv",
		.replacement_name = NULL,
		.replacement_type = NEWV,
		.arg_count = 2,
		.arg_count_is_minimum = false,
	},
	{
		.original_function_name = "phutil_is_utf8",
		.replacement_name = "mb_check_encoding",
		.replacement_type = MBSTRING_WITH_CAST,
		.arg_count = 1,
		.arg_count_is_minimum = false,
	},
	{
		.original_function_name = "phutil_utf8_strlen",
		.replacement_name = "mb_strlen",
		.replacement_type = MBSTRING,
		.arg_count = 1,
		.arg_count_is_minimum = false,
	},
	{
		.original_function_name = "call_user_func",
		.replacement_name = NULL,
		.replacement_type = CALLABLE,
		.arg_count = 1,
		.arg_count_is_minimum = true,
	},
	{
		.original_function_name = "call_user_func_array",
		.replacement_name = NULL,
		.replacement_type = CALLABLE,
		.arg_count = 2,
		.arg_count_is_minimum = false,
	},
	{
		.original_function_name = "phutil_is_utf8_slowly",
		.replacement_name = "ArcBooster\\is_utf8_slowly",
		.replacement_type = WHOLESALE,
		.arg_count = 1,
		.arg_count_is_minimum = true,
	},
	{
		.original_function_name = "phutil_utf8ize",
		.replacement_name = "ArcBooster\\utf8ize",
		.replacement_type = WHOLESALE,
		.arg_count = 1,
		.arg_count_is_minimum = false,
	},
	{
		.original_function_name = "phutil_utf8v_codepoints",
		.replacement_name = "ArcBooster\\utf8v_codepoints",
		.replacement_type = WHOLESALE,
		.arg_count = 1,
		.arg_count_is_minimum = false,
	},
	{
		.original_function_name = "phutil_utf8v",
		.replacement_name = "ArcBooster\\utf8v",
		.replacement_type = WHOLESALE,
		.arg_count = 1,
		.arg_count_is_minimum = true,
	},
};

static const size_t replacements_count = sizeof(replacements) / sizeof(replacements[0]);

static HashTable replacement_map;

/**
 * Sentinel that indicates that whatever replacement is identified is not actually valid.
 * For example, imports like `use function index as idx` should not be replaced.
 */
static arc_booster_replacement invalid_sentinel = {
	.original_function_name = NULL,
	.replacement_name = NULL,
	.replacement_type = INVALID,
};

#define ARC_BOOSTER_INVALID_REPLACEMENT &invalid_sentinel

#define ARC_BOOSTER_UNWRAP_POSSIBLE_NAMED_ARG(a) if (a->kind == ZEND_AST_NAMED_ARG) { \
	zend_ast_destroy(a->child[0]); \
	a = a->child[1]; \
}

static zend_ast **arc_booster_get_children(zend_ast *node, uint32_t *amount) {
	if (zend_ast_is_decl(node)) {
		*amount = 5;
		zend_ast_decl *decl = (zend_ast_decl*)node;
		return decl->child;
	} else if (zend_ast_is_list(node)) {
		zend_ast_list *list = zend_ast_get_list(node);
		*amount = list->children;
		return list->child;
	} else if (!zend_ast_is_special(node)) {
		*amount = zend_ast_get_num_children(node);
		return node->child;
	}

	*amount = 0;
	return NULL;
}

static void arc_booster_rename_function_call(zend_ast *call, const arc_booster_replacement *replacement) {
	ZEND_ASSERT(call->kind == ZEND_AST_CALL);

	zend_string *str = zend_string_init(replacement->replacement_name, strlen(replacement->replacement_name), false);
	zend_ast *new_name = zend_ast_create_zval_from_str(str);
	new_name->attr = ZEND_NAME_FQ;
	new_name->lineno = call->child[0]->lineno;

	// Per the AST RFC (https://wiki.php.net/rfc/abstract_syntax_tree#ast_allocation_destruction_and_copy),
	// ast nodes are allocated using an arena allocator, which allows freeing them in bulk, even if they are orphaned.
	// This doesn't apply to the zvals held by zend_ast_zval, though. Ensure they are free'd when they are orphaned.
	zend_ast_destroy(call->child[0]);

	call->child[0] = new_name;
}

static zend_ast *arc_booster_create_array_unpack(zend_ast *arg) {
	return zend_ast_create_list_1(ZEND_AST_ARG_LIST, zend_ast_create_1(ZEND_AST_UNPACK, arg));
}

static zend_ast *arc_booster_convert_array_elem_to_arg(const zend_ast *elem) {
	if (elem->kind == ZEND_AST_UNPACK) {
		return NULL;
	}

	ZEND_ASSERT(elem->kind == ZEND_AST_ARRAY_ELEM);

	if (elem->child[1] == NULL) {
		return elem->child[0];
	}

	if (
		elem->child[1]->kind != ZEND_AST_ZVAL ||
		Z_TYPE_P(zend_ast_get_zval(elem->child[1])) != IS_STRING) {

		return NULL;
	}

	zend_ast *named_arg = zend_ast_create_2(ZEND_AST_NAMED_ARG, elem->child[1], elem->child[0]);
	named_arg->lineno = elem->lineno;

	return named_arg;
}

static void arc_booster_process_call(zend_ast *parent, zend_ast *node, const HashTable *imports, const bool use_mbstring, const bool is_globalspace) {
	ZEND_ASSERT(node->kind == ZEND_AST_CALL);

	zend_ast *name_node = node->child[0];
	if (name_node->kind != ZEND_AST_ZVAL || Z_TYPE_P(zend_ast_get_zval(name_node)) != IS_STRING) {
		// Can't evaluate dynamic function calls.
		return;
	}

	zend_string *orig_name = zend_ast_get_str(name_node);
	arc_booster_replacement *replacement = NULL;

	if (name_node->attr == ZEND_NAME_FQ) {
		// Ensure there is no leading \ for fully-qualified names.
		// This can happen if something like ('\bar')() is used, which the AST represents identical to bar().
		if (ZSTR_VAL(orig_name)[0] == '\\') {
			zend_string *name = zend_string_init(ZSTR_VAL(orig_name) + 1, ZSTR_LEN(orig_name) - 1, false);

			replacement = zend_hash_find_ptr_lc(&replacement_map, name);

			zend_string_release_ex(name, false);
		} else {
			replacement = zend_hash_find_ptr_lc(&replacement_map, orig_name);
		}
	} else {
		replacement = zend_hash_find_ptr_lc(imports, orig_name);

		if (replacement == NULL && is_globalspace) {
			// In the global namespace, unqualified functions always refer to global functions.
			replacement = zend_hash_find_ptr_lc(&replacement_map, orig_name);
		}
	}

	if (replacement == NULL || replacement == ARC_BOOSTER_INVALID_REPLACEMENT) {
		return;
	}

	if (node->child[1]->kind != ZEND_AST_ARG_LIST) {
		if (
			node->child[1]->kind == ZEND_AST_CALLABLE_CONVERT &&
			(replacement->replacement_type == WHOLESALE || replacement->replacement_type == IDX)) {
			arc_booster_rename_function_call(node, replacement);
		}

		return;
	}

	zend_ast_list *args = zend_ast_get_list(node->child[1]);

	if (replacement->arg_count_is_minimum) {
		if (args->children < replacement->arg_count) {
			return;
		}
	} else if (replacement->arg_count != args->children) {
		return;
	}

	switch (replacement->replacement_type) {
		case WHOLESALE:
			arc_booster_rename_function_call(node, replacement);
			break;
		case MBSTRING_WITH_CAST:
			// If mbstring is disabled, we can use our own function.
			if (!use_mbstring) {
				ZEND_ASSERT(strcmp(replacements[24].original_function_name, "phutil_is_utf8_slowly") == 0);

				arc_booster_rename_function_call(node, &replacements[24]);
				break;
			}
			ZEND_FALLTHROUGH;
		case MBSTRING:
			if (!use_mbstring) {
				break;
			}

			ARC_BOOSTER_UNWRAP_POSSIBLE_NAMED_ARG(args->child[0]);

			if (replacement->replacement_type == MBSTRING_WITH_CAST) {
				const uint32_t lineno = args->child[0]->lineno;
				args->child[0] = zend_ast_create_cast(IS_STRING, args->child[0]);
				args->child[0]->lineno = lineno;
			}

			arc_booster_rename_function_call(node, replacement);

			node->child[1] = zend_ast_list_add((zend_ast*)args, zend_ast_create_zval_from_str(ZSTR_INIT_LITERAL("UTF-8", false)));
			break;
		case DROP_FUNCTION_CALL: {
			ZEND_ASSERT(args->children == 1);

			ARC_BOOSTER_UNWRAP_POSSIBLE_NAMED_ARG(args->child[0]);
			zend_ast *arg = args->child[0];
			args->child[0] = NULL;

			uint32_t amount;
			zend_ast **children = arc_booster_get_children(parent, &amount);
			for (uint32_t i = 0; i < amount; i++) {
				if (children[i] == node) {
					children[i] = arg;
					break;
				}
			}

			zend_ast_destroy(node);
			break;
		}
		case IDX: {
			if (args->children >= 3) {
				arc_booster_rename_function_call(node, replacement);
				break;
			}

			ZEND_ASSERT(node->kind >> ZEND_AST_NUM_CHILDREN_SHIFT == 2);

			zend_ast *collection = args->child[0];
			ARC_BOOSTER_UNWRAP_POSSIBLE_NAMED_ARG(collection);
			args->child[0] = NULL;

			zend_ast *dim = args->child[1];
			ARC_BOOSTER_UNWRAP_POSSIBLE_NAMED_ARG(dim);
			args->child[1] = NULL;

			zend_ast_destroy(node->child[0]);
			zend_ast_destroy(node->child[1]);

			zend_ast *null_const = zend_ast_create_zval_from_str(ZSTR_KNOWN(ZEND_STR_NULL_LOWERCASE));

			node->kind = ZEND_AST_COALESCE;
			node->attr = 0;
			node->child[0] = zend_ast_create_2(ZEND_AST_DIM, collection, dim);
			node->child[1] = zend_ast_create_ex_1(ZEND_AST_CONST, ZEND_NAME_FQ, null_const);

			break;
		}
		case NEWV: {
			ZEND_ASSERT(node->kind >> ZEND_AST_NUM_CHILDREN_SHIFT == 2);

			zend_ast *class_type = args->child[0];
			ARC_BOOSTER_UNWRAP_POSSIBLE_NAMED_ARG(class_type);
			args->child[0] = NULL;

			zend_ast *arg = args->child[1];
			ARC_BOOSTER_UNWRAP_POSSIBLE_NAMED_ARG(arg);
			args->child[1] = NULL;

			zend_ast_destroy(node->child[0]);
			zend_ast_destroy(node->child[1]);

			zend_ast *new_arg_list;
			if (arg->kind != ZEND_AST_ARRAY) {
				new_arg_list = arc_booster_create_array_unpack(arg);
			} else {
				const zend_ast_list *array = zend_ast_get_list(arg);

				if (array->children == 0) {
					new_arg_list = zend_ast_create_list_0(ZEND_AST_ARG_LIST);
				} else if (array->children == 1) {
					zend_ast *new_arg = arc_booster_convert_array_elem_to_arg(array->child[0]);

					if (new_arg) {
						new_arg_list = zend_ast_create_list_1(ZEND_AST_ARG_LIST, new_arg);
					} else {
						new_arg_list = arc_booster_create_array_unpack(arg);
					}
				} else {
					zend_ast *new_arg_1 = arc_booster_convert_array_elem_to_arg(array->child[0]);
					zend_ast *new_arg_2 = arc_booster_convert_array_elem_to_arg(array->child[1]);

					if (new_arg_1 && new_arg_2) {
						new_arg_list = zend_ast_create_list_2(ZEND_AST_ARG_LIST, new_arg_1, new_arg_2);

						for (uint32_t i = 2; i < array->children; i++) {
							zend_ast *new_arg = arc_booster_convert_array_elem_to_arg(array->child[i]);
							if (!new_arg) {
								new_arg_list = arc_booster_create_array_unpack(arg);
								break;
							}
							new_arg_list = zend_ast_list_add(new_arg_list, new_arg);
						}
					} else {
						new_arg_list = arc_booster_create_array_unpack(arg);
					}
				}
			}
			new_arg_list->lineno = args->lineno;

			node->kind = ZEND_AST_NEW;
			node->attr = 0;
			node->child[0] = class_type;
			node->child[1] = new_arg_list;

			break;
		}
		case CALLABLE: {
			zend_ast *arg = args->child[0];

			if (arg->kind == ZEND_AST_NAMED_ARG) {
				arg = arg->child[1];
			}

			if (arg->kind != ZEND_AST_ZVAL) {
				break;
			}

			zval *callback_arg = zend_ast_get_zval(arg);
			if (Z_TYPE_P(callback_arg) != IS_STRING) {
				break;
			}

			zend_string *callback = Z_STR_P(callback_arg);

			if (ZSTR_VAL(callback)[0] == '\\') {
				zend_string *name = zend_string_init(ZSTR_VAL(callback) + 1, ZSTR_LEN(callback) - 1, false);

				replacement = zend_hash_find_ptr_lc(&replacement_map, name);

				zend_string_release(name);
			} else if (!is_globalspace) {
				// If this isn't the global namespace and the callback isn't fully qualified,
				// PHP will resolve this call at runtime.
				break;
			} else {
				replacement = zend_hash_find_ptr_lc(&replacement_map, callback);
			}

			if (
				replacement == NULL ||
				replacement == ARC_BOOSTER_INVALID_REPLACEMENT ||
				replacement->replacement_type != WHOLESALE) {
				break;
			}

			zend_string_release(callback);

			// If the first argument was named, we'd have unwrapped it earlier.
			// Make sure it doesn't leak because we won't be keeping it.
			if (arg != args->child[0]) {
				zend_ast_destroy(args->child[0]);
			}

			zend_string *replacement_name = zend_string_concat2(
				"\\", 1,
				replacement->replacement_name, strlen(replacement->replacement_name));
			ZVAL_NEW_STR(callback_arg, replacement_name);

			break;
		}
		default:
			ZEND_UNREACHABLE();
	}
}

/**
 * @param ast ZEND_AST_USE_ELEM node
 * @param imports HashTable of imports in the current scope
 * @param has_namespace If the use has a namespace prefix not set on node (group use elements)
 */
static void arc_booster_process_use_elem(const zend_ast *ast, HashTable *imports, bool has_namespace) {
	ZEND_ASSERT(ast->kind == ZEND_AST_USE_ELEM);

	zend_string *function_name;
	zend_string *fqn = zend_ast_get_str(ast->child[0]);
	const char *ns_separator = zend_memrchr(ZSTR_VAL(fqn), '\\', ZSTR_LEN(fqn));

	if (ns_separator) {
		has_namespace = true;
		const char *offset = ns_separator + 1;
		fqn = zend_string_init(offset, ZSTR_VAL(fqn) + ZSTR_LEN(fqn) - offset, false);
		function_name = zend_string_tolower(fqn);
		zend_string_release(fqn);
	} else {
		function_name = zend_string_tolower(fqn);
	}

	arc_booster_replacement *replacement = zend_hash_find_ptr(&replacement_map, function_name);

	if (ast->child[1]) {
		zend_string *alias = zend_string_tolower(zend_ast_get_str(ast->child[1]));

		// If there is an alias and it conflicts with one of arcanist's functions,
		// put it in imports.
		// This prevents calls imported through "use function index as idx;" from being changed.
		if (zend_hash_find_ptr(&replacement_map, alias)) {
			// If using "use function idx as idx", this will mark idx as invalid replacement.
			// However, that is such an unlikely edge case that we can just ignore it.
			replacement = ARC_BOOSTER_INVALID_REPLACEMENT;
			zend_string_release(function_name);
			function_name = alias;
		} else {
			zend_string_release(alias);
		}
	} else if (has_namespace && replacement) {
		// If there's a namespace, then any matching function will be a non-arcanist function.
		// Make sure it doesn't get replaced.
		replacement = ARC_BOOSTER_INVALID_REPLACEMENT;
	}

	if (replacement) {
		zend_hash_add_ptr(imports, function_name, replacement);
	}

	zend_string_release(function_name);
}

static void arc_booster_process_use(const zend_ast_list *ast, HashTable *imports) {
	ZEND_ASSERT(ast->kind == ZEND_AST_USE);

	if (ast->attr != ZEND_SYMBOL_FUNCTION) {
		return;
	}

	for (size_t i = 0; i < ast->children; i++) {
		arc_booster_process_use_elem(ast->child[i], imports, false);
	}
}

static void arc_booster_process_group_use(const zend_ast *ast, HashTable *imports) {
	ZEND_ASSERT(ast->kind == ZEND_AST_GROUP_USE);

	const zend_ast_list *uses = zend_ast_get_list(ast->child[1]);

	for (size_t i = 0; i < uses->children; i++) {
		const zend_ast *use = uses->child[i];

		if (use->attr != ZEND_SYMBOL_FUNCTION && ast->attr != ZEND_SYMBOL_FUNCTION) {
			continue;
		}

		arc_booster_process_use_elem(use, imports, true);
	}
}

static void arc_booster_process_statement_list(zend_ast *ast, bool is_globalspace, const bool use_mbstring) {
	ZEND_ASSERT(ast->kind == ZEND_AST_STMT_LIST);

	zend_ptr_stack node_stack;
	HashTable imports;

	zend_hash_init(&imports, replacements_count, NULL, NULL, false);
	zend_hash_real_init_mixed(&imports);

	zend_ptr_stack_init(&node_stack);
	zend_ptr_stack_push(&node_stack, ast);

	while (zend_ptr_stack_num_elements(&node_stack) != 0) {
		zend_ast *node = (zend_ast*)zend_ptr_stack_pop(&node_stack);

		if (node == NULL) {
			continue;
		}

		switch (node->kind) {
			case ZEND_AST_NAMESPACE:
				is_globalspace = node->child[0] == NULL;
				if (node->child[1]) {
					arc_booster_process_statement_list(node->child[1], is_globalspace, use_mbstring);
					continue;
				}
				// Clean imports when evaluating bodyless namespaces.
				// This prevents multiple namespace declarations in the same file from
				// inheriting their predecessor's imports.
				if (zend_hash_num_elements(&imports) != 0) {
					zend_hash_clean(&imports);
				}
				break;
			case ZEND_AST_GROUP_USE:
				arc_booster_process_group_use(node, &imports);
				continue;
			case ZEND_AST_USE:
				arc_booster_process_use(zend_ast_get_list(node), &imports);
				continue;
			default:
				break;
		}

		uint32_t index;
		zend_ast **children = arc_booster_get_children(node, &index);
		while (index--) {
			zend_ast *child = children[index];

			if (child == NULL) {
				continue;
			}

			if (child->kind == ZEND_AST_CALL) {
				arc_booster_process_call(node, child, &imports, use_mbstring, is_globalspace);
			}
			zend_ptr_stack_push(&node_stack, child);
		}
	}

	zend_hash_destroy(&imports);
	zend_ptr_stack_destroy(&node_stack);
}

static void arc_booster_ast_process(zend_ast *ast) {
	if (ast && ast->kind == ZEND_AST_STMT_LIST) {
		arc_booster_process_statement_list(ast, true, ARC_BOOSTER_G(use_mbstring));
	}

	if (original_zend_ast_process) {
		original_zend_ast_process(ast);
	}
}

PHP_FUNCTION(ArcBooster_idx)
{
	HashTable *array = NULL;
	zval *key = NULL;
	zval *default_val = NULL;
	zval *value = NULL;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_ARRAY_HT(array)
		Z_PARAM_ZVAL(key)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(default_val)
	ZEND_PARSE_PARAMETERS_END();

	if (arc_booster_ht_fetch(array, key, &value, false) == FAILURE) {
		if (EXPECTED(!EG(exception))) {
			zend_argument_type_error(2, "must be a valid array offset type");
		}
		RETURN_THROWS();
	}

	if (value) {
		RETURN_COPY_DEREF(value);
	} else if (default_val) {
		RETURN_COPY_DEREF(default_val);
	} else {
		RETURN_NULL();
	}
}

PHP_FUNCTION(ArcBooster_string_cast)
{
	zval *value = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	switch (Z_TYPE_P(value)) {
		case IS_STRING:
		case IS_LONG:
		case IS_NULL:
		case IS_DOUBLE:
		case IS_FALSE:
		case IS_TRUE:
		case IS_RESOURCE:
		case IS_OBJECT:
		case IS_REFERENCE:
			break;
		default:
			zend_argument_type_error(1, "must be convertable to string, %s given", zend_zval_value_name(value));
			return;
	}

	RETURN_STR(zval_get_string(value));
}

PHP_FUNCTION(ArcBooster_nonempty_string)
{
	zval *value = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		// We're not using Z_PARAM_STR_OR_NULL here, as that accepts Stringable.
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	switch (Z_TYPE_P(value)) {
		case IS_NULL:
			RETURN_FALSE;
		case IS_STRING:
			RETURN_BOOL(Z_STRLEN_P(value) > 0);
		default:
			zend_argument_type_error(1, "must be of type ?string, %s given", zend_zval_value_name(value));
			break;
	}
}

PHP_FUNCTION(ArcBooster_nonempty_stringlike)
{
	zend_string *string = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR_OR_NULL(string)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_BOOL(string && ZSTR_LEN(string) > 0);
}

PHP_FUNCTION(ArcBooster_nonempty_scalar)
{
	zval *value = NULL;
	zval string;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	switch (Z_TYPE_P(value)) {
		case IS_DOUBLE:
		case IS_LONG:
		case IS_TRUE:
			RETURN_TRUE;
		case IS_NULL:
		case IS_FALSE:
			RETURN_FALSE;
		case IS_STRING:
			RETURN_BOOL(Z_STRLEN_P(value) > 0);
		case IS_OBJECT:
			// This uses the cast handler directly, to bypass the default exception thrown if
			// the conversion to string fails.
			// We cannot distinguish between user-land exceptions and cast handler failures.
			// We want to throw our own exception in the latter case, but keep any user-land exceptions.
			if (Z_OBJ_HANDLER_P(value, cast_object)(Z_OBJ_P(value), &string, IS_STRING) == SUCCESS) {
				RETVAL_BOOL(Z_STRLEN(string) > 0);
				zend_string_release(Z_STR(string));
				return;
			}

			if (EG(exception)) {
				RETURN_THROWS();
			}
			ZEND_FALLTHROUGH;
		default:
			zend_argument_type_error(1, "must be a scalar, %s given", zend_zval_value_name(value));
			break;
	}
}

PHP_FUNCTION(ArcBooster_coalesce) {
	zval *args = NULL;
	uint32_t argc = 0;

	ZEND_PARSE_PARAMETERS_START(0, -1)
		Z_PARAM_VARIADIC('+', args, argc)
	ZEND_PARSE_PARAMETERS_END();

	for (uint32_t i = 0; i < argc; i++) {
		zval *arg = args + i;
		if (Z_TYPE_P(arg) != IS_NULL) {
			RETURN_COPY_DEREF(arg);
		}
	}

	RETURN_NULL();
}

PHP_FUNCTION(ArcBooster_nonempty) {
	zval *args = NULL;
	zval *arg = NULL;
	uint32_t argc = 0;

	ZEND_PARSE_PARAMETERS_START(0, -1)
		Z_PARAM_VARIADIC('+', args, argc)
	ZEND_PARSE_PARAMETERS_END();

	for (uint32_t i = 0; i < argc; i++) {
		arg = args + i;
		if (zend_is_true(arg)) {
			RETURN_COPY_DEREF(arg);
		}
	}

	if (arg) {
		RETURN_COPY_DEREF(arg);
	}

	RETURN_NULL();
}

/**
 * Call a method on an object.
 *
 * @param obj Object to call the method on
 * @param method Name of the method to call
 * @param retval Return value zval for the method to use
 * @return Whether or not the call succeeded. On returning FAILURE, an exception is set.
 */
static zend_result arc_booster_call_method(const zval *obj, zend_string *method, zval *retval) {
	if (Z_TYPE_P(obj) != IS_OBJECT) {
#if PHP_VERSION_ID >= 80600
		zend_throw_error(NULL, "Call to a member function %pS() on %s", method, zend_zval_value_name(obj));
#else
		zend_throw_error(NULL, "Call to a member function %s() on %s", ZSTR_VAL(method), zend_zval_value_name(obj));
#endif
		return FAILURE;
	}

	zval zval_method;
	zend_fcall_info_cache fcc;

	ZVAL_STR(&zval_method, method);

	if (UNEXPECTED(!zend_is_callable_ex(&zval_method, Z_OBJ_P(obj), IS_CALLABLE_SUPPRESS_DEPRECATIONS, NULL, &fcc, NULL))) {
		if (EXPECTED(!EG(exception))) {
#if PHP_VERSION_ID >= 80500
			zend_undefined_method(Z_OBJCE_P(obj), method);
#else
			zend_throw_error(NULL, "Call to undefined method %s::%s()", ZSTR_VAL(Z_OBJCE_P(obj)->name), ZSTR_VAL(method));
#endif
		}

		return FAILURE;
	}

	zend_call_known_fcc(&fcc, retval, 0, NULL, NULL);
	// Need to free potential trampoline (__call/__callStatic) copied function handler before releasing the closure
	zend_release_fcall_info_cache(&fcc);

	if (EG(exception)) {
		return FAILURE;
	} else if (UNEXPECTED(Z_TYPE_P(retval) == IS_UNDEF)) {
		ZEND_ASSERT("zend_call_known_fcc shouldn't fail without an exception.");
		ZVAL_NULL(retval);
	} else {
		ZVAL_DEREF(retval);
	}

	return SUCCESS;

}

typedef struct {
	zend_string *method;
	zend_string *key_method;
	zval *ret_arr;
} arc_booster_mpull_apply_info;

static int arc_booster_mpull_apply(zend_object_iterator *iter, void *puser) {
	arc_booster_mpull_apply_info *apply_info = (arc_booster_mpull_apply_info*)puser;
	zval *data = iter->funcs->get_current_data(iter);
	zval value;

	if (EG(exception)) {
		return ZEND_HASH_APPLY_STOP;
	}
	if (data == NULL) {
		return ZEND_HASH_APPLY_STOP;
	}

	if (apply_info->method == NULL) {
		ZVAL_COPY_DEREF(&value, data);
	} else {
		ZVAL_DEREF(data);

		if (arc_booster_call_method(data, apply_info->method, &value) == FAILURE) {
			return ZEND_HASH_APPLY_STOP;
		}
	}

	if (apply_info->key_method == NULL) {
		if (iter->funcs->get_current_key) {
			zval key;
			iter->funcs->get_current_key(iter, &key);
			if (EG(exception)) {
				zval_ptr_dtor(&value);
				return ZEND_HASH_APPLY_STOP;
			}

			zend_result status = array_set_zval_key(Z_ARRVAL_P(apply_info->ret_arr), &key, &value);
			zval_ptr_dtor(&key);

			if (status == FAILURE) {
				zval_ptr_dtor(&value);
				return ZEND_HASH_APPLY_STOP;
			}
		} else {
			Z_TRY_ADDREF(value);
			add_next_index_zval(apply_info->ret_arr, &value);
		}
	} else {
		zval key;

		if (arc_booster_call_method(data, apply_info->key_method, &key) == FAILURE) {
			return ZEND_HASH_APPLY_STOP;
		}

		// The "pull" family of functions coerces null values for keys to an empty string.
		if (Z_TYPE(key) == IS_NULL) {
			ZVAL_EMPTY_STRING(&key);
		}

		zend_result status = array_set_zval_key(Z_ARRVAL_P(apply_info->ret_arr), &key, &value);
		zval_ptr_dtor(&key);

		if (status == FAILURE) {
			zval_ptr_dtor(&value);
			return ZEND_HASH_APPLY_STOP;
		}
	}
	zval_ptr_dtor(&value);

	return ZEND_HASH_APPLY_KEEP;
}

PHP_FUNCTION(ArcBooster_mpull)
{
	zval *list = NULL;
	zend_string *method = NULL;
	zend_string *key_method = NULL;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_ITERABLE(list)
		Z_PARAM_STR_OR_NULL(method)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR_OR_NULL(key_method)
	ZEND_PARSE_PARAMETERS_END();

	if (Z_TYPE_P(list) == IS_ARRAY) {
		HashTable *arrval = Z_ARRVAL_P(list);
		uint32_t elem_count = zend_hash_num_elements(arrval);

		if (elem_count == 0) {
			RETURN_EMPTY_ARRAY();
		}

		if (method == NULL && key_method == NULL) {
			RETURN_COPY(list);
		}

		array_init_size(return_value, elem_count);

		if (key_method == NULL) {
			ZEND_HASH_FOREACH_KEY_VAL(arrval, zend_ulong num_key, zend_string* str_key, zval *entry) {
				zval value;
				zval *result = NULL;

				ZVAL_DEREF(entry);

				if (arc_booster_call_method(entry, method, &value) == FAILURE) {
					RETURN_THROWS();
				}

				if (str_key) {
					result = zend_hash_add_new(Z_ARRVAL_P(return_value), str_key, &value);
				} else {
					result = zend_hash_index_add_new(Z_ARRVAL_P(return_value), num_key, &value);
				}

				if (result) {
					Z_TRY_ADDREF_P(result);
				} else {
					RETURN_THROWS();
				}

				zval_ptr_dtor(&value);
			} ZEND_HASH_FOREACH_END();
		} else {
			ZEND_HASH_FOREACH_VAL(arrval, zval *entry) {
				zval key;
				zval value;

				if (method == NULL) {
					ZVAL_COPY_DEREF(&value, entry);
				} else {
					ZVAL_DEREF(entry);

					if (arc_booster_call_method(entry, method, &value) == FAILURE) {
						RETURN_THROWS();
					}
				}

				if (arc_booster_call_method(entry, key_method, &key) == FAILURE) {
					zval_ptr_dtor(&value);
					RETURN_THROWS();
				}

				// The "pull" family of functions coerces null values for keys to an empty string.
				if (Z_TYPE(key) == IS_NULL) {
					ZVAL_EMPTY_STRING(&key);
				}

				zend_result status = array_set_zval_key(Z_ARRVAL_P(return_value), &key, &value);
				zval_ptr_dtor(&key);
				zval_ptr_dtor(&value);

				if (status == FAILURE) {
					RETURN_THROWS();
				}
			} ZEND_HASH_FOREACH_END();
		}
	} else {
		array_init(return_value);

		arc_booster_mpull_apply_info apply_info = {
			.ret_arr = return_value,
			.method = method,
			.key_method = key_method,
		};

		if (spl_iterator_apply(list, arc_booster_mpull_apply, (void*)&apply_info) == FAILURE) {
			RETURN_THROWS();
		}
	}
}

/**
 * Fetch a property value from an object.
 *
 * @param obj Object from which to fetch the property
 * @param property Name of the property
 * @param retval Retrieved zval
 * @param cache_slot
 */
static void arc_booster_fetch_prop(const zval *obj, zend_string *property, zval *retval, void **cache_slot) {
	if (Z_TYPE_P(obj) != IS_OBJECT) {
#if PHP_VERSION_ID >= 80600
		zend_error(E_WARNING, "Attempt to ready property \"%pS\" on %s", property, zend_zval_value_name(obj));
#else
		zend_error(E_WARNING, "Attempt to ready property \"%s\" on %s", ZSTR_VAL(property), zend_zval_value_name(obj));
#endif

		ZVAL_NULL(retval);
		return;
	}

  // The has_property check is first performed in "exists" mode
	// (which returns true for properties that are null but exist)
	// and then in "has" mode to handle objects that implement __isset
	// (which is not called in "exists" mode).
	if (
		Z_OBJ_HANDLER_P(obj, has_property)(Z_OBJ_P(obj), property, ZEND_PROPERTY_EXISTS, cache_slot)
			|| Z_OBJ_HANDLER_P(obj, has_property)(Z_OBJ_P(obj), property, ZEND_PROPERTY_ISSET, cache_slot)) {
		zval *prop = Z_OBJ_HANDLER_P(obj, read_property)(Z_OBJ_P(obj), property, BP_VAR_R, cache_slot, retval);

		if (prop) {
			ZVAL_DEREF(prop);
			if (prop != retval) {
				ZVAL_COPY(retval, prop);
			}
		}
	} else {
#if PHP_VERSION_ID >= 80600
		zend_error(E_WARNING, "Undefined property: %pS::$%pS", Z_OBJCE_P(obj)->name, property);
#else
		zend_error(E_WARNING, "Undefined property: %s::$%s", ZSTR_VAL(Z_OBJ_P(obj)->ce->name), ZSTR_VAL(property));
#endif

		ZVAL_NULL(retval);
	}
}

typedef struct {
	zend_string *property;
	zend_string *key_property;
	zval *ret_arr;
	void *cache_slot_property[3];
	void *cache_slot_key_property[3];
} arc_booster_ppull_apply_info;

static int arc_booster_ppull_apply(zend_object_iterator *iter, void *puser) {
	arc_booster_ppull_apply_info *apply_info = (arc_booster_ppull_apply_info*)puser;
	zval *data = iter->funcs->get_current_data(iter);
	zval value;

	if (EG(exception)) {
		return ZEND_HASH_APPLY_STOP;
	}
	if (data == NULL) {
		return ZEND_HASH_APPLY_STOP;
	}

	if (apply_info->property == NULL) {
		ZVAL_COPY_DEREF(&value, data);
	} else {
		ZVAL_DEREF(data);

		arc_booster_fetch_prop(data, apply_info->property, &value, apply_info->cache_slot_property);
	}

	if (apply_info->key_property == NULL) {
		if (iter->funcs->get_current_key) {
			zval key;
			iter->funcs->get_current_key(iter, &key);
			if (EG(exception)) {
				zval_ptr_dtor(&value);
				return ZEND_HASH_APPLY_STOP;
			}

			zend_result status = array_set_zval_key(Z_ARRVAL_P(apply_info->ret_arr), &key, &value);
			zval_ptr_dtor(&key);

			if (status == FAILURE) {
				zval_ptr_dtor(&value);
				return ZEND_HASH_APPLY_STOP;
			}
		} else {
			Z_TRY_ADDREF(value);
			add_next_index_zval(apply_info->ret_arr, &value);
		}
	} else {
		zval key;

		arc_booster_fetch_prop(data, apply_info->key_property, &key, apply_info->cache_slot_key_property);

		// The "pull" family of functions coerces null values for keys to an empty string.
		if (Z_TYPE(key) == IS_NULL) {
			ZVAL_EMPTY_STRING(&key);
		}

		zend_result status = array_set_zval_key(Z_ARRVAL_P(apply_info->ret_arr), &key, &value);
		zval_ptr_dtor(&key);

		if (status == FAILURE) {
			zval_ptr_dtor(&value);
			return ZEND_HASH_APPLY_STOP;
		}
	}
	zval_ptr_dtor(&value);

	return ZEND_HASH_APPLY_KEEP;
}

PHP_FUNCTION(ArcBooster_ppull)
{
	zval *list = NULL;
	zend_string *property = NULL;
	zend_string *key_property = NULL;
	void *cache_slot_property[3] = { NULL, NULL, NULL };
	void *cache_slot_key_property[3] = { NULL, NULL, NULL };

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_ITERABLE(list)
		Z_PARAM_STR_OR_NULL(property)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR_OR_NULL(key_property)
	ZEND_PARSE_PARAMETERS_END();

	if (Z_TYPE_P(list) == IS_ARRAY) {
		HashTable *arrval = Z_ARRVAL_P(list);
		uint32_t elem_count = zend_hash_num_elements(arrval);

		if (elem_count == 0) {
			RETURN_EMPTY_ARRAY();
		}

		if (property == NULL && key_property == NULL) {
			RETURN_COPY(list);
		}

		array_init_size(return_value, elem_count);

		if (key_property == NULL) {
			ZEND_HASH_FOREACH_KEY_VAL(arrval, zend_ulong num_key, zend_string* str_key, zval *entry) {
				zval value;
				zval *result = NULL;

				ZVAL_DEREF(entry);

				arc_booster_fetch_prop(entry, property, &value, cache_slot_property);

				if (str_key) {
					result = zend_hash_add_new(Z_ARRVAL_P(return_value), str_key, &value);
				} else {
					result = zend_hash_index_add_new(Z_ARRVAL_P(return_value), num_key, &value);
				}

				if (result) {
					Z_TRY_ADDREF_P(result);
				} else {
					RETURN_THROWS();
				}

				zval_ptr_dtor(&value);
			} ZEND_HASH_FOREACH_END();
		} else {
			ZEND_HASH_FOREACH_VAL(arrval, zval *entry) {
				zval key;
				zval value;

				if (property == NULL) {
					ZVAL_COPY_DEREF(&value, entry);
				} else {
					ZVAL_DEREF(entry);
					arc_booster_fetch_prop(entry, property, &value, cache_slot_property);
				}

				arc_booster_fetch_prop(entry, key_property, &key, cache_slot_key_property);

				// The "pull" family of functions coerces null values for keys to an empty string.
				if (Z_TYPE(key) == IS_NULL) {
					ZVAL_EMPTY_STRING(&key);
				}

				zend_result status = array_set_zval_key(Z_ARRVAL_P(return_value), &key, &value);
				zval_ptr_dtor(&key);
				zval_ptr_dtor(&value);

				if (status == FAILURE) {
					RETURN_THROWS();
				}

			} ZEND_HASH_FOREACH_END();
		}
	} else {
		array_init(return_value);

		arc_booster_ppull_apply_info apply_info = {
			.ret_arr = return_value,
			.property = property,
			.key_property = key_property,
			.cache_slot_property = cache_slot_property,
			.cache_slot_key_property = cache_slot_key_property,
		};

		if (spl_iterator_apply(list, arc_booster_ppull_apply, (void*)&apply_info) == FAILURE) {
			RETURN_THROWS();
		}
	}
}

static ZEND_COLD void arc_booster_undefined_array_index(const zend_ulong idx) {
	zend_error(E_WARNING, "Undefined array key " ZEND_LONG_FMT, idx);
}

/**
 * Fetch an indexed value from a container (array, string or indexable object).
 *
 * @param container Container from where to fetch
 * @param arg_num Argument index. Used for error messages
 * @param dim Dimension
 * @param value Retrieved zval
 * @return If the fetch was successful. On returning FAILURE, an exception is set.
 */
static int arc_booster_container_fetch(const zval *container, const uint32_t arg_num, zval *dim, zval *value) {
	switch (Z_TYPE_P(container)) {
		case IS_ARRAY: {
			zval *tmp = NULL;
			if (arc_booster_ht_fetch(Z_ARR_P(container), dim, &tmp, true) == FAILURE) {
				if (EXPECTED(!EG(exception))) {
					zend_argument_type_error(arg_num, "must be a valid array offset type");
				}
				return FAILURE;
			}

			if (tmp == NULL) {
				switch (Z_TYPE_P(dim)) {
					case IS_STRING: {
						zend_ulong idx;

						if (ZEND_HANDLE_NUMERIC(Z_STR_P(dim), idx)) {
							arc_booster_undefined_array_index(idx);
						} else {
							zend_error(E_WARNING, "Undefined array key \"%s\"", Z_STRVAL_P(dim));
						}
						break;
					}
					case IS_LONG:
						arc_booster_undefined_array_index(Z_LVAL_P(dim));
						break;
					case IS_NULL:
						zend_error(E_WARNING, "Undefined array key \"\"");
						break;
					case IS_DOUBLE:
						arc_booster_undefined_array_index(zend_dval_to_lval_safe(Z_DVAL_P(dim)));
						break;
					case IS_FALSE:
						arc_booster_undefined_array_index(0);
						break;
					case IS_TRUE:
						arc_booster_undefined_array_index(1);
						break;
					case IS_RESOURCE:
						arc_booster_undefined_array_index(Z_RES_HANDLE_P(dim));
						break;
					default:
						ZEND_UNREACHABLE();
				}
				ZVAL_NULL(value);
			} else {
				ZVAL_COPY_DEREF(value, tmp);
			}

			return SUCCESS;
		}
		case IS_OBJECT:
			break;
		case IS_STRING: {
			zend_long offset;

			if (Z_TYPE_P(dim) == IS_STRING) {
				bool trailing_data = false;
				// For BC reasons errors are allowed so that leading numeric string warns
				if (IS_LONG == is_numeric_string_ex(Z_STRVAL_P(dim), Z_STRLEN_P(dim), &offset, NULL, /* allow errors */ true, NULL, &trailing_data)) {
					if (UNEXPECTED(trailing_data)) {
						zend_error(E_WARNING, "Illegal string offset \"%s\"", Z_STRVAL_P(dim));
					}
				} else {
					zend_illegal_container_offset(ZSTR_KNOWN(ZEND_STR_STRING), dim, BP_VAR_R);
				}
			} else if (Z_TYPE_P(dim) != IS_LONG) {
				zend_illegal_container_offset(Z_STR_P(container), dim, BP_VAR_R);
				return FAILURE;
			} else {
				offset = Z_LVAL_P(dim);
			}

			if (Z_LVAL_P(dim) > Z_STRLEN_P(container)) {
				ZVAL_EMPTY_STRING(value);
				return SUCCESS;
			}

			const zend_long strlen = Z_STRLEN_P(container);

			if (offset < 0) {
				if (-offset > strlen) {
					offset = 0;
				} else {
					offset = strlen - offset;
				}
			}

			ZVAL_CHAR(value, Z_STRVAL_P(container)[Z_LVAL_P(dim)]);
			return SUCCESS;
		}
		default:
			zend_error(E_WARNING, "Trying to access array offset on %s", zend_zval_value_name(container));
			ZVAL_NULL(value);
			return SUCCESS;
	}

	zend_object *obj = Z_OBJ_P(container);

	if (!obj->handlers->read_dimension) {
#if PHP_VERSION_ID >= 80600
		zend_throw_error(NULL, "Cannot use object of type %pS as array", obj->ce->name);
#else
		zend_throw_error(NULL, "Cannot use object of type %s as array", ZSTR_VAL(obj->ce->name));
#endif

		return FAILURE;
	}

	GC_ADDREF(obj);

	zval *retval = obj->handlers->read_dimension(obj, dim, BP_VAR_R, value);

	if (retval) {
		if (value != retval) {
			ZVAL_COPY_DEREF(value, retval);
		} else if (UNEXPECTED(Z_ISREF_P(value))) {
			zend_unwrap_reference(value);
		}
	} else {
		ZVAL_NULL(value);
	}

	GC_DELREF(obj);

	return SUCCESS;
}

typedef struct {
	zval *index;
	zval *key_index;
	zval *ret_arr;
} arc_booster_ipull_apply_info;

static int arc_booster_ipull_apply(zend_object_iterator *iter, void *puser) {
	arc_booster_ipull_apply_info *apply_info = (arc_booster_ipull_apply_info*)puser;
	zval *data = iter->funcs->get_current_data(iter);
	zval value;

	if (EG(exception)) {
		return ZEND_HASH_APPLY_STOP;
	}
	if (data == NULL) {
		return ZEND_HASH_APPLY_STOP;
	}

	if (apply_info->index == NULL) {
		ZVAL_COPY_DEREF(&value, data);
	} else {
		ZVAL_DEREF(data);

		if (arc_booster_container_fetch(data, 2, apply_info->index, &value) == FAILURE) {
			return ZEND_HASH_APPLY_STOP;
		}
	}

	if (apply_info->key_index == NULL) {
		if (iter->funcs->get_current_key) {
			zval key;
			iter->funcs->get_current_key(iter, &key);
			if (EG(exception)) {
				zval_ptr_dtor(&value);
				return ZEND_HASH_APPLY_STOP;
			}

			zend_result status = array_set_zval_key(Z_ARRVAL_P(apply_info->ret_arr), &key, &value);
			zval_ptr_dtor(&key);

			if (status == FAILURE) {
				zval_ptr_dtor(&value);
				return ZEND_HASH_APPLY_STOP;
			}
		} else {
			Z_TRY_ADDREF(value);
			add_next_index_zval(apply_info->ret_arr, &value);
		}
	} else {
		zval key;

		if (arc_booster_container_fetch(data, 3, apply_info->key_index, &key) == FAILURE) {
			zval_ptr_dtor(&value);
			return ZEND_HASH_APPLY_STOP;
		}

		// The "pull" family of functions coerces null values for keys to an empty string.
		if (Z_TYPE(key) == IS_NULL) {
			ZVAL_EMPTY_STRING(&key);
		}

		zend_result status = array_set_zval_key(Z_ARRVAL_P(apply_info->ret_arr), &key, &value);
		zval_ptr_dtor(&key);

		if (status == FAILURE) {
			zval_ptr_dtor(&value);
			return ZEND_HASH_APPLY_STOP;
		}
	}
	zval_ptr_dtor(&value);

	return ZEND_HASH_APPLY_KEEP;
}

PHP_FUNCTION(ArcBooster_ipull)
{
	zval *list = NULL;
	zval *index = NULL;
	zval *key_index = NULL;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_ITERABLE(list)
		Z_PARAM_ZVAL_OR_NULL(index)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL_OR_NULL(key_index)
	ZEND_PARSE_PARAMETERS_END();

	if (Z_TYPE_P(list) == IS_ARRAY) {
		HashTable *arrval = Z_ARRVAL_P(list);
		uint32_t elem_count = zend_hash_num_elements(arrval);

		if (elem_count == 0) {
			RETURN_EMPTY_ARRAY();
		}

		if (index == NULL && key_index == NULL) {
			RETURN_COPY(list);
		}

		array_init_size(return_value, elem_count);

		if (key_index == NULL) {
			ZEND_HASH_FOREACH_KEY_VAL(arrval, zend_ulong num_key, zend_string* str_key, zval *entry) {
				zval value;
				zval *result = NULL;

				ZVAL_DEREF(entry);

				if (arc_booster_container_fetch(entry, 2, index, &value) == FAILURE) {
					RETURN_THROWS();
				}

				if (str_key) {
					result = zend_hash_add_new(Z_ARRVAL_P(return_value), str_key, &value);
				} else {
					result = zend_hash_index_add_new(Z_ARRVAL_P(return_value), num_key, &value);
				}

				if (result) {
					Z_TRY_ADDREF_P(result);
				} else {
					RETURN_THROWS();
				}

				zval_ptr_dtor(&value);
			} ZEND_HASH_FOREACH_END();
		} else {
			ZEND_HASH_FOREACH_VAL(arrval, zval *entry) {
				zval key;
				zval value;

				if (index == NULL) {
					ZVAL_COPY_DEREF(&value, entry);
				} else {
					ZVAL_DEREF(entry);

					if (arc_booster_container_fetch(entry, 2, index, &value) == FAILURE) {
						RETURN_THROWS();
					}
				}

				if (arc_booster_container_fetch(entry, 3, key_index, &key) == FAILURE) {
					zval_ptr_dtor(&value);
					RETURN_THROWS();
				}

				// The "pull" family of functions coerces null values for keys to an empty string.
				if (Z_TYPE(key) == IS_NULL) {
					ZVAL_EMPTY_STRING(&key);
				}

				zend_result status = array_set_zval_key(Z_ARRVAL_P(return_value), &key, &value);
				zval_ptr_dtor(&key);
				zval_ptr_dtor(&value);

				if (status == FAILURE) {
					RETURN_THROWS();
				}
			} ZEND_HASH_FOREACH_END();
		}
	} else {
		array_init(return_value);

		arc_booster_ipull_apply_info apply_info = {
			.ret_arr = return_value,
			.index = index,
			.key_index = key_index,
		};

		if (spl_iterator_apply(list, arc_booster_ipull_apply, (void*)&apply_info) == FAILURE) {
			RETURN_THROWS();
		}
	}
}

PHP_FUNCTION(ArcBooster_head)
{
	HashTable *array = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(array)
	ZEND_PARSE_PARAMETERS_END();

	ZEND_HASH_FOREACH_VAL(array, zval *zv) {
		RETURN_COPY_DEREF(zv);
	} ZEND_HASH_FOREACH_END();

	RETURN_FALSE;
}

PHP_FUNCTION(ArcBooster_last)
{
	HashTable *array = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(array)
	ZEND_PARSE_PARAMETERS_END();

	ZEND_HASH_REVERSE_FOREACH_VAL(array, zval *zv) {
		RETURN_COPY_DEREF(zv);
	} ZEND_HASH_FOREACH_END();

	RETURN_FALSE;
}

static zend_always_inline void arc_booster_array_merge_packed(HashTable *dest, const HashTable *src) {
	ZEND_ASSERT(HT_IS_PACKED(src));

	ZEND_HASH_FILL_PACKED(dest) {
		ZEND_HASH_PACKED_FOREACH_VAL(src, zval *entry) {
			if (UNEXPECTED(Z_ISREF_P(entry)) && UNEXPECTED(Z_REFCOUNT_P(entry) == 1)) {
				entry = Z_REFVAL_P(entry);
			}
			Z_TRY_ADDREF_P(entry);

			ZEND_HASH_FILL_ADD(entry);
		} ZEND_HASH_FOREACH_END();
	} ZEND_HASH_FILL_END();
}

static zend_always_inline void arc_booster_array_merge(HashTable *dest, const HashTable *src) {
	ZEND_HASH_FOREACH_STR_KEY_VAL(src, zend_string *string_key, zval *entry) {
		if (UNEXPECTED(Z_ISREF_P(entry) && Z_REFCOUNT_P(entry) == 1)) {
			entry = Z_REFVAL_P(entry);
		}
		Z_TRY_ADDREF_P(entry);

		if (string_key) {
			zend_hash_update(dest, string_key, entry);
		} else {
			zend_hash_next_index_insert_new(dest, entry);
		}
	} ZEND_HASH_FOREACH_END();
}

PHP_FUNCTION(ArcBooster_array_mergev) {
	HashTable *array = NULL;
	uint64_t count = 0;
	bool is_packed = true;
	zval *entry = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(array)
	ZEND_PARSE_PARAMETERS_END();

	ZEND_HASH_FOREACH_KEY_VAL(array, zend_ulong num_key, zend_string* str_key, entry) {
		if (UNEXPECTED(Z_TYPE_P(entry) != IS_ARRAY)) {
			if (str_key) {
#if PHP_VERSION_ID >= 80600
				zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "Offset \"%pS\" must be of type array, %s given", str_key, zend_zval_value_name(entry));
#else
				zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "Offset \"%s\" must be of type array, %s given", ZSTR_VAL(str_key), zend_zval_value_name(entry));
#endif
			} else {
				zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "Offset #%lu must be of type array, %s given", num_key, zend_zval_value_name(entry));
			}
			RETURN_THROWS();
		}

		if (!HT_IS_PACKED(Z_ARRVAL_P(entry))) {
			is_packed = false;
		}

		count += zend_hash_num_elements(Z_ARRVAL_P(entry));
	} ZEND_HASH_FOREACH_END();

	if (count == 0) {
		RETURN_EMPTY_ARRAY();
	}

	if (zend_hash_num_elements(array) == 1) {
		ZEND_HASH_FOREACH_VAL(array, entry) {
			RETURN_COPY_DEREF(entry);
		} ZEND_HASH_FOREACH_END();
	}

	if (UNEXPECTED(count >= HT_MAX_SIZE)) {
		zend_throw_error(NULL, "The total number of elements must be lower than %u", HT_MAX_SIZE);
		RETURN_THROWS();
	}

	array_init_size(return_value, count);
	if (is_packed) {
		zend_hash_real_init_packed(Z_ARR_P(return_value));

		ZEND_HASH_FOREACH_VAL(array, entry) {
			arc_booster_array_merge_packed(Z_ARR_P(return_value), Z_ARR_P(entry));
		} ZEND_HASH_FOREACH_END();
	} else {
		zend_hash_real_init_mixed(Z_ARR_P(return_value));

		ZEND_HASH_FOREACH_VAL(array, entry) {
			arc_booster_array_merge(Z_ARR_P(return_value), Z_ARR_P(entry));
		} ZEND_HASH_FOREACH_END();
	}
}

PHP_FUNCTION(ArcBooster_array_fuse) {
	HashTable *array = NULL;
	uint32_t count = 0;
	zval *entry = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY_HT_OR_NULL(array)
	ZEND_PARSE_PARAMETERS_END();

	if (array == NULL) {
		RETURN_EMPTY_ARRAY();
	}

	count = zend_hash_num_elements(array);

	if (count == 0) {
		RETURN_EMPTY_ARRAY();
	}

	array_init_size(return_value, count);

	ZEND_HASH_FOREACH_VAL(array, entry) {
		if (Z_TYPE_P(entry) != IS_UNDEF) {
			if (Z_TYPE_P(entry) == IS_LONG) {
				entry = zend_hash_index_update(Z_ARRVAL_P(return_value), Z_LVAL_P(entry), entry);
			} else {
				zend_string *tmp_key;
				zend_string *key = zval_get_tmp_string(entry, &tmp_key);
				entry = zend_symtable_update(Z_ARRVAL_P(return_value), key, entry);
				zend_tmp_string_release(tmp_key);
			}
			zval_add_ref(entry);
		}
	} ZEND_HASH_FOREACH_END();
}

PHP_FUNCTION(ArcBooster_render_ast) {
	zend_string *str = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(str)
	ZEND_PARSE_PARAMETERS_END();

	// zend_compile_string_to_ast does not call zend_ast_process.
	// Inline the function here to call our ast processing function at the right moment.
	zend_arena *ast_arena;

	zval code_zv;
	bool original_in_compilation = CG(in_compilation);
	zend_lex_state original_lex_state;
	zend_ast *ast = NULL;

	ZVAL_STR_COPY(&code_zv, str);

	CG(in_compilation) = true;

	zend_save_lexical_state(&original_lex_state);
	zend_prepare_string_for_scanning(&code_zv, ZSTR_EMPTY_ALLOC());
	CG(ast) = NULL;
	CG(ast_arena) = zend_arena_create(1024 * 32);
	LANG_SCNG(yy_state) = yycINITIAL;

	if (zendparse() != 0) {
		zend_ast_destroy(CG(ast));
		zend_arena_destroy(CG(ast_arena));
		CG(ast) = NULL;
	}

	// restore_lexical_state changes CG(ast) and CG(ast_arena)
	ast = CG(ast);
	ast_arena = CG(ast_arena);

	arc_booster_ast_process(ast);

	zend_restore_lexical_state(&original_lex_state);
	CG(in_compilation) = original_in_compilation;

	zval_ptr_dtor_str(&code_zv);

	if (!ast) {
		RETURN_THROWS();
	}

	zend_string *result = zend_ast_export("", ast, "");

	zend_ast_destroy(ast);
	zend_arena_destroy(ast_arena);

	RETURN_STR(result);
}

#define ZSTR_MARK_AS_VALID_UTF8(str) if (!ZSTR_IS_INTERNED(str)) GC_ADD_FLAGS(str, IS_STR_VALID_UTF8)

static bool validate_utf8_codepoints(const zend_string *str, size_t *codepoint_offset, size_t *offset, const bool only_bmp) {
	ZEND_ASSERT(ZSTR_LEN(str) > *offset);

	for (; *offset < ZSTR_LEN(str); (*offset)++) {
		*codepoint_offset = *offset;
		unsigned char chr = ZSTR_VAL(str)[*offset];

		if (chr <= 0x7F) {
			continue;
		} else if (chr >= 0xC2 && chr <= 0xDF) {
			if ((*offset) + 1 >= ZSTR_LEN(str)) {
				return false;
			}
			chr = ZSTR_VAL(str)[(*offset) + 1];
			if (chr >= 0x80 && chr <= 0xBF) {
				(*offset)++;
				continue;
			}
			return false;
		} else if (chr == 0xED) {
			// Some sequences in this block are surrogate codepoints that are reserved for use in UTF16.
			int codepoint = (chr & 0x0F) << 12;
			if ((*offset) + 1 >= ZSTR_LEN(str)) {
				return false;
			}
			chr = ZSTR_VAL(str)[(*offset) + 1];
			codepoint += (chr & 0x3F) << 6;
			if (chr >= 0x80 && chr <= 0xBF) {
				(*offset)++;
				if ((*offset) + 1 >= ZSTR_LEN(str)) {
					return false;
				}
				chr = ZSTR_VAL(str)[(*offset) + 1];
				codepoint += chr & 0x3F;

				if (chr >= 0x80 && chr <= 0xBF) {
					(*offset)++;

					if (codepoint >= 0xD800 && codepoint <= 0xDFFF) {
						// Reject these surrogate codepoints.
						return false;
					}

					continue;
				}
			}
			return false;
		} else if (chr > 0xE0 && chr <= 0xEF) {
			if ((*offset) + 1 >= ZSTR_LEN(str)) {
				return false;
			}
			chr = ZSTR_VAL(str)[(*offset) + 1];
			if (chr >= 0x80 && chr <= 0xBF) {
				(*offset)++;
				if ((*offset) + 1 >= ZSTR_LEN(str)) {
					return false;
				}
				chr = ZSTR_VAL(str)[(*offset) + 1];
				if (chr >= 0x80 && chr <= 0xBF) {
					(*offset)++;
					continue;
				}
				// Backtrack. The previous character is valid for the sequence,
				// but the sequence is incomplete. phutil_utf8ize considers this two invalid codepoints.
				(*offset)--;
			}
			return false;
		} else if (chr == 0xE0) {
			if ((*offset) + 1 >= ZSTR_LEN(str)) {
				return false;
			}
			chr = ZSTR_VAL(str)[(*offset) + 1];

			// NOTE: This range starts at 0xA0, not 0x80. The values 0x80-0xA0 are
			// "valid", but not minimal representations, and MySQL rejects them.
			// We're special casing this part of the range.

			if (chr >= 0xA0 && chr <= 0xBF) {
				(*offset)++;
				if ((*offset) + 1 >= ZSTR_LEN(str)) {
					return false;
				}
				chr = ZSTR_VAL(str)[(*offset) + 1];
				if (chr >= 0x80 && chr <= 0xBF) {
					(*offset)++;
					continue;
				}
				// Backtrack. The previous character is valid for the sequence,
				// but the sequence is incomplete. phutil_utf8ize considers this two invalid codepoints.
				(*offset)--;
			}
			return false;
		} else if (!only_bmp) {
			if (chr > 0xF0 && chr <= 0xF4) {
				if ((*offset) + 1 >= ZSTR_LEN(str)) {
					return false;
				}
				chr = ZSTR_VAL(str)[(*offset) + 1];
				if (chr >= 0x80 && chr <= 0xBF) {
					(*offset)++;
					if ((*offset) + 1 >= ZSTR_LEN(str)) {
						return false;
					}
					chr = ZSTR_VAL(str)[(*offset) + 1];
					if (chr >= 0x80 && chr <= 0xBF) {
						(*offset)++;
						if ((*offset) + 1 >= ZSTR_LEN(str)) {
							return false;
						}
						chr = ZSTR_VAL(str)[(*offset) + 1];
						if (chr >= 0x80 && chr <= 0xBF) {
							(*offset)++;
							continue;
						}
						// Backtrack. The previous character is valid for the sequence,
						// but the sequence is incomplete. phutil_utf8ize considers this four invalid codepoints.
						(*offset)--;
					}
					// Backtrack. The previous character is valid for the sequence,
					// but the sequence is incomplete. phutil_utf8ize considers this three invalid codepoints.
					(*offset)--;
				}
			} else if (chr == 0xF0) {
				if ((*offset) + 1 >= ZSTR_LEN(str)) {
					return false;
				}
				chr = ZSTR_VAL(str)[(*offset) + 1];

				// NOTE: As above, this range starts at 0x90, not 0x80.
				// The values 0x80-0x90 are not minimal representations.

				if (chr >= 0x90 && chr <= 0xBF) {
					(*offset)++;
					if ((*offset) + 1 >= ZSTR_LEN(str)) {
						return false;
					}
					chr = ZSTR_VAL(str)[(*offset) + 1];
					if (chr >= 0x80 && chr <= 0xBF) {
						(*offset)++;
						if ((*offset) + 1 >= ZSTR_LEN(str)) {
							return false;
						}
						chr = ZSTR_VAL(str)[(*offset) + 1];
						if (chr >= 0x80 && chr <= 0xBF) {
							(*offset)++;
							continue;
						}
						// Backtrack. The previous character is valid for the sequence,
						// but the sequence is incomplete. phutil_utf8ize considers this four invalid codepoints.
						(*offset)--;
					}
					// Backtrack. The previous character is valid for the sequence,
					// but the sequence is incomplete. phutil_utf8ize considers this three invalid codepoints.
					(*offset)--;
				}
			}
		}

		return false;
	}

	return true;
}

PHP_FUNCTION(ArcBooster_is_utf8_slowly) {
	zend_string *str = NULL;
	bool only_bmp = false;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR_OR_NULL(str)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(only_bmp)
	ZEND_PARSE_PARAMETERS_END();

	if (!str || ZSTR_LEN(str) == 0 || ZSTR_IS_VALID_UTF8(str)) {
		RETURN_TRUE;
	}

	size_t p = 0;
	size_t codepoint = 0;
	if (validate_utf8_codepoints(str, &codepoint, &p, only_bmp)) {
		ZSTR_MARK_AS_VALID_UTF8(str);
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}

static void arc_booster_invalid_utf8(const zend_string *function_name) {
#if PHP_VERSION_ID >= 80600
	zend_value_error("Invalid UTF-8 string passed to %pS.", function_name);
#else
	zend_value_error("Invalid UTF-8 string passed to %s.", ZSTR_VAL(function_name));
#endif
}

static zend_result arc_booster_utf8v(zval *array, zend_string *string, bool byte_limit_is_null, const zend_long byte_limit, const zend_string *function_name) {
	array_init_size(array, ZSTR_LEN(string));
	zend_hash_real_init_packed(Z_ARR_P(array));

	size_t i = 0;
	while (i < ZSTR_LEN(string)) {
		const unsigned char chr = ZSTR_VAL(string)[i];
		size_t sequence_length;

		if (chr <= 0x7F) {
			add_next_index_str(array, ZSTR_CHAR(chr));
			i++;

			if (!byte_limit_is_null && i > byte_limit) {
				break;
			}

			continue;
		} else if (chr <= 0xC0) {
			arc_booster_invalid_utf8(function_name);
			return FAILURE;
		} else if (chr <= 0xDF) {
			sequence_length = 2;
		} else if (chr <= 0xEF) {
			sequence_length = 3;
		} else if (chr <= 0xF7) {
			sequence_length = 4;
		} else if (chr <= 0xFB) {
			sequence_length = 5;
		} else if (chr <= 0xFD) {
			sequence_length = 6;
		} else {
			arc_booster_invalid_utf8(function_name);
			return FAILURE;
		}

		if (i + sequence_length > ZSTR_LEN(string)) {
			arc_booster_invalid_utf8(function_name);
			return FAILURE;
		}

		for (size_t j = 1; j < sequence_length; j++) {
			if ((unsigned char)ZSTR_VAL(string)[i + j] >= 0xC0) {
				arc_booster_invalid_utf8(function_name);
				return FAILURE;
			}
		}

		add_next_index_str(array, zend_string_init(ZSTR_VAL(string) + i, sequence_length, false));
		i += sequence_length;

		if (!byte_limit_is_null && i > byte_limit) {
			break;
		}
	}

	ZSTR_MARK_AS_VALID_UTF8(string);
	return SUCCESS;
}

PHP_FUNCTION(ArcBooster_utf8v) {
	zend_string *str = NULL;
	zend_long byte_limit;
	bool byte_limit_is_null;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR_OR_NULL(str)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(byte_limit, byte_limit_is_null)
	ZEND_PARSE_PARAMETERS_END();

	if (!str || ZSTR_LEN(str) == 0) {
		RETURN_EMPTY_ARRAY();
	}

	// No need to act on the return value of arc_booster_utf8v.
	// FAILURE sets an exception, which PHP will throw when leaving this scope.
	arc_booster_utf8v(return_value, str, byte_limit_is_null, byte_limit, execute_data->func->common.function_name);
}

PHP_FUNCTION(ArcBooster_utf8v_codepoints) {
	zend_string *str = NULL;
	zval char_array;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR_OR_NULL(str)
	ZEND_PARSE_PARAMETERS_END();

	if (!str || ZSTR_LEN(str) == 0) {
		RETURN_EMPTY_ARRAY();
	}

	if (arc_booster_utf8v(&char_array, str, true, -1, execute_data->func->common.function_name) == FAILURE) {
		zval_ptr_dtor(&char_array);
		RETURN_THROWS();
	}

	array_init_size(return_value, zend_hash_num_elements(Z_ARR(char_array)));
	zend_hash_real_init_packed(Z_ARR_P(return_value));

	ZEND_HASH_PACKED_FOREACH_VAL(Z_ARR(char_array), zval *chr) {
		ZEND_ASSERT(Z_TYPE_P(chr) == IS_STRING);

		const unsigned char c = Z_STRVAL_P(chr)[0];
		zend_long codepoint = 0;

		if ((c & 0x80) == 0) {
			codepoint = c;
		} else if ((c & 0xE0) == 0xC0) {
			codepoint = ((c & 0x1F) << 6)
				 + (((unsigned char)Z_STRVAL_P(chr)[1] & 0x3F));
		} else if ((c & 0xF0) == 0xE0) {
			codepoint = ((c & 0x0F) << 12)
				 + (((unsigned char)Z_STRVAL_P(chr)[1] & 0x3F) << 6)
				 + (((unsigned char)Z_STRVAL_P(chr)[2] & 0x3F));
		} else if ((c & 0xF8) == 0xF0) {
			codepoint = ((c & 0x07) << 18)
				 + (((unsigned char)Z_STRVAL_P(chr)[1] & 0x3F) << 12)
				 + (((unsigned char)Z_STRVAL_P(chr)[2] & 0x3F) << 6)
				 + (((unsigned char)Z_STRVAL_P(chr)[3] & 0x3F));
		} else if ((c & 0xFC) == 0xF8) {
			codepoint = ((c & 0x03) << 24)
				 + (((unsigned char)Z_STRVAL_P(chr)[1] & 0x3F) << 18)
				 + (((unsigned char)Z_STRVAL_P(chr)[2] & 0x3F) << 12)
				 + (((unsigned char)Z_STRVAL_P(chr)[3] & 0x3F) << 6)
				 + (((unsigned char)Z_STRVAL_P(chr)[4] & 0x3F));
		} else if ((c & 0xFE) == 0xFC) {
			codepoint = ((c & 0x01) << 30)
				 + (((unsigned char)Z_STRVAL_P(chr)[1] & 0x3F) << 24)
				 + (((unsigned char)Z_STRVAL_P(chr)[2] & 0x3F) << 18)
				 + (((unsigned char)Z_STRVAL_P(chr)[3] & 0x3F) << 12)
				 + (((unsigned char)Z_STRVAL_P(chr)[4] & 0x3F) << 6)
				 + (((unsigned char)Z_STRVAL_P(chr)[5] & 0x3F));
		}

		add_next_index_long(return_value, codepoint);
	} ZEND_HASH_FOREACH_END();

	zval_ptr_dtor(&char_array);
}

static const char *unicode_replacement_character = "\xEF\xBF\xBD";

PHP_FUNCTION(ArcBooster_utf8ize) {
	zend_string *str = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR_OR_NULL(str)
	ZEND_PARSE_PARAMETERS_END();

	if (!str || ZSTR_LEN(str) == 0) {
		RETURN_EMPTY_STRING();
	} else if (ZSTR_IS_VALID_UTF8(str)) {
		RETURN_STR_COPY(str);
	}

	size_t offset = 0;
	size_t codepoint_offset = 0;
	if (validate_utf8_codepoints(str, &codepoint_offset, &offset, false)) {
		ZSTR_MARK_AS_VALID_UTF8(str);
		RETURN_STR_COPY(str);
	}

	// At this point we know that the string contains invalid code points.
	smart_str buf = {0};
	// Copy the progress up to the last good point into the smart_str.
	smart_str_appendl(&buf, ZSTR_VAL(str), codepoint_offset);
	// At this point, offset still points at the last byte of the invalid codepoint.
	// Move to the next byte, which will be a new codepoint.
	offset++;

	// validate_utf8_codepoints fails for 3-byte characters in one of the surrogate ranges.
	// Replace all bytes in the code point with the replacement character as it is not
	// considered a code point, but three invalid code points.
	if (
		(unsigned char)ZSTR_VAL(str)[codepoint_offset] == 0xED &&
		offset - codepoint_offset == 3) {

		smart_str_appends(&buf, unicode_replacement_character);
		smart_str_appends(&buf, unicode_replacement_character);
		smart_str_appends(&buf, unicode_replacement_character);
	} else {
		smart_str_appends(&buf, unicode_replacement_character);
	}

	while (offset < ZSTR_LEN(str)) {
		size_t last_segment_start = offset;

		// Find the next invalid code point.
		if (validate_utf8_codepoints(str, &codepoint_offset, &offset, false)) {
			// The rest of the string is valid UTF-8, so we can just append it and break out of the loop.
			smart_str_appendl(&buf, ZSTR_VAL(str) + last_segment_start, offset - last_segment_start);
			break;
		}

		smart_str_appendl(&buf, ZSTR_VAL(str) + last_segment_start, codepoint_offset - last_segment_start);
		offset++;

		if (
			(unsigned char)ZSTR_VAL(str)[codepoint_offset] == 0xED &&
			offset - codepoint_offset == 3) {

			smart_str_appends(&buf, unicode_replacement_character);
			smart_str_appends(&buf, unicode_replacement_character);
			smart_str_appends(&buf, unicode_replacement_character);
		} else {
			smart_str_appends(&buf, unicode_replacement_character);
		}
	}

	zend_string *utf8ized_str = smart_str_extract(&buf);
	ZSTR_MARK_AS_VALID_UTF8(utf8ized_str);

	RETURN_STR(utf8ized_str);
}

PHP_MINIT_FUNCTION(arc_booster) {
#if defined(COMPILE_DL_ARC_BOOSTER) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	REGISTER_INI_ENTRIES();

	if (!ARC_BOOSTER_G(disable_ast_mod)) {
		original_zend_ast_process = zend_ast_process;
		zend_ast_process = arc_booster_ast_process;
	}

	zend_hash_init(&replacement_map, replacements_count, NULL, NULL, true);
	zend_hash_real_init_mixed(&replacement_map);

	for (size_t i = 0; i < replacements_count; i++) {
		zend_hash_str_add_ptr(
			&replacement_map,
			replacements[i].original_function_name,
			strlen(replacements[i].original_function_name),
			&replacements[i]);
	}

	return SUCCESS;
}

PHP_MINFO_FUNCTION(arc_booster) {
	php_info_print_table_start();
	php_info_print_table_row(2, "arc_booster support", "enabled");
	php_info_print_table_row(2, "extension version", PHP_ARC_BOOSTER_VERSION);
	php_info_print_table_end();
	DISPLAY_INI_ENTRIES();
}

PHP_MSHUTDOWN_FUNCTION(arc_booster) {
	if (original_zend_ast_process) {
		zend_ast_process = original_zend_ast_process;
	}

	zend_hash_destroy(&replacement_map);

	UNREGISTER_INI_ENTRIES();

	return SUCCESS;
}

zend_module_entry arc_booster_module_entry = {
	STANDARD_MODULE_HEADER,
	"arc_booster",
	ext_functions,
	PHP_MINIT(arc_booster),
	PHP_MSHUTDOWN(arc_booster),
	NULL,
	NULL,
	PHP_MINFO(arc_booster),
	PHP_ARC_BOOSTER_VERSION,
	STANDARD_MODULE_PROPERTIES
};
