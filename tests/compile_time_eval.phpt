--TEST--
Compile-time evaluation
--SKIPIF--
<?php
if (PHP_VERSION_ID >= 80500 && PHP_VERSION_ID < 80600) {
  die("skip Not for PHP 8.5");
}
?>
--EXTENSIONS--
arc_booster
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.opt_debug_level=0x20000
zend_test.observer.enabled=0
--FILE--
<?php

function test() {
    return [
        ArcBooster\string_cast(5),
        ArcBooster\nonempty_string('hello'),
        ArcBooster\nonempty_stringlike('hello'),
        ArcBooster\nonempty_scalar(42),
        ArcBooster\head([1, 2, 3]),
        ArcBooster\last([4, 5, 6]),
        ArcBooster\idx(['foo' => 'bar'], 'foo'),
        ArcBooster\array_mergev([['foo'], ['bar']]),
        ArcBooster\array_fuse(['foo', 'bar']),
    ];
}
var_dump(test());
--EXPECTF--
$_main:
     ; (lines=6, args=0, vars=0, tmps=1)
     ; (after optimizer)
     ; %scompile_time_eval.php:1-17
0000 INIT_FCALL 1 %d string("var_dump")
0001 INIT_FCALL 0 %d string("test")
0002 T0 = DO_UCALL
0003 SEND_VAL T0 1
0004 DO_ICALL
0005 RETURN int(1)

test:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %scompile_time_eval.php:3-15
0000 RETURN array(...)
array(9) {
  [0]=>
  string(1) "5"
  [1]=>
  bool(true)
  [2]=>
  bool(true)
  [3]=>
  bool(true)
  [4]=>
  int(1)
  [5]=>
  int(6)
  [6]=>
  string(3) "bar"
  [7]=>
  array(2) {
    [0]=>
    string(3) "foo"
    [1]=>
    string(3) "bar"
  }
  [8]=>
  array(2) {
    ["foo"]=>
    string(3) "foo"
    ["bar"]=>
    string(3) "bar"
  }
}
