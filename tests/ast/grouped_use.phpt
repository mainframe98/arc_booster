--TEST--
arc_booster: AST manipulations for id
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

namespace Foo {
  function index($array, $index, $default) {
    return $array[$index] ?? $default;
  }

  function wrap($foo) {
    return $foo;
  }
}

namespace {
  use function Foo\{index as idx, wrap as id};

  $foo = id(new \stdClass());
  $bar = idx([], 'key', false);

  echo ArcBooster\render_ast(file_get_contents(__FILE__));
}

?>
--EXPECT--

namespace Foo {
    function index($array, $index, $default) {
        return $array[$index] ?? $default;
    }

    function wrap($foo) {
        return $foo;
    }

}

namespace {
    use function Foo\{index as idx, wrap as id};
    $foo = id(new \stdClass());
    $bar = idx([], 'key', false);
    echo ArcBooster\render_ast(file_get_contents(__FILE__));
}
