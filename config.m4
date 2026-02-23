dnl Autotools config.m4 for PHP extension arc_booster

sinclude(./autoconf/php-executable.m4)

PHP_ARG_ENABLE([arc_booster],
  [whether to enable arc_booster support],
  [AS_HELP_STRING([--enable-arc_booster],
    [Enable arc_booster support])],
  [no])

AS_VAR_IF([PHP_ARC_BOOSTER], [no],, [
  dnl Configure extension sources and compilation flags.
  PHP_NEW_EXTENSION([arc_booster],
    [arc_booster.c],
    [$ext_shared],,
    [-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1])
  PHP_ADD_MAKEFILE_FRAGMENT
])
