#ifndef PHP_ARC_BOOSTER_H
#define PHP_ARC_BOOSTER_H

#ifdef ZTS
#include "TSRM.h"
#endif

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "php.h"

extern zend_module_entry arc_booster_module_entry;
#define phpext_arc_booster_ptr &arc_booster_module_entry

#define PHP_ARC_BOOSTER_VERSION "1.0.0"

#ifdef PHP_WIN32
#	define PHP_ARC_BOOSTER_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_ARC_BOOSTER_API __attribute__ ((visibility("default")))
#else
#	define PHP_ARC_BOOSTER_API
#endif

ZEND_BEGIN_MODULE_GLOBALS(arc_booster)
	bool disable_ast_mod;
	bool use_mbstring;
ZEND_END_MODULE_GLOBALS(arc_booster)

ZEND_DECLARE_MODULE_GLOBALS(arc_booster)

#ifdef COMPILE_DL_ARC_BOOSTER
ZEND_GET_MODULE(arc_booster)
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
#endif

ZEND_API void (*original_zend_ast_process)(zend_ast *ast);

#define ARC_BOOSTER_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(arc_booster, v)

#endif	/* PHP_ARC_BOOSTER_H */
