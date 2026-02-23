--TEST--
arc_booster: Namespaced AST manipulations with bodyless namespaces;
--SKIPIF--
<?php
if ((PHP_VERSION_ID >= 80500 && PHP_VERSION_ID < 80513) || PHP_VERSION_ID < 80428) {
  die("skip PHP 8.4.28, PHP 8.5.13 or newer");
}
?>
--EXTENSIONS--
arc_booster
--FILE--
<?php

namespace Foo;
use function id;

function idx($array, $key, $default) {
  return $array[$key] ?? $default;
}

$foo = id(new \stdClass());
$bar = idx([], 'key', false);

namespace Bar;

function id($id) {
  return $id;
}

$baz = id(new \stdClass());

echo \ArcBooster\render_ast(file_get_contents(__FILE__));

?>
--EXPECT--
namespace Foo;
use function id;
function idx($array, $key, $default) {
    return $array[$key] ?? $default;
}

$foo = new \stdClass();
$bar = idx([], 'key', false);
namespace Bar;
function id($id) {
    return $id;
}

$baz = id(new \stdClass());
echo \ArcBooster\render_ast(file_get_contents(__FILE__));
