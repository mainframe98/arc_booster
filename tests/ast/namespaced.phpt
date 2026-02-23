--TEST--
arc_booster: Namespaced AST manipulations
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

namespace {
  function idx($array, $index, $default) {
    return $array[$index] ?? $default;
  }

  function id($foo) {
    return $foo;
  }
}

namespace Foo {
  use function idx;

  $foo = id(new \stdClass());
  $bar = idx([], 'key', false);
}

namespace Bar {
  function id() {}

  id();
}

namespace {
  echo ArcBooster\render_ast(file_get_contents(__FILE__));
}

?>
--EXPECT--
namespace {
    function idx($array, $index, $default) {
        return $array[$index] ?? $default;
    }

    function id($foo) {
        return $foo;
    }

}

namespace Foo {
    use function idx;
    $foo = id(new \stdClass());
    $bar = \ArcBooster\idx([], 'key', false);
}

namespace Bar {
    function id() {
    }

    id();
}

namespace {
    echo ArcBooster\render_ast(file_get_contents(__FILE__));
}
