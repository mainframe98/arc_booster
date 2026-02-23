--TEST--
arc_booster: AST manipulations for newv (PHP >= 8.6)
--SKIPIF--
<?php
if (PHP_VERSION_ID < 80600) {
  die("skip PHP 8.6 or newer");
}
?>
--EXTENSIONS--
arc_booster
--FILE--
<?php

namespace Example {
  class Test {
    public function __construct() {}
  }
}

namespace {
  use Example\Test;

  function test() {
    $class = Test::class;
    $args = array(1, 2);

    \newv('Test', array(1, 2));
    \newv(Test::class, array(1, 2));
    \newv($class, array(1, 2));
    \newv('Example\\'.$class, array(1, 2));
    \newv('Test', array());
    \newv('Test', array(1));
    \newv('Test', array(1, 2, 3));
    \newv('Test', array(1, 2, 3, 4));
    \newv('Test', array('foo' => 'bar'));
    \newv('Test', array($class => 'bar'));
    \newv('Test', $args);
  }

  echo ArcBooster\render_ast(file_get_contents(__FILE__));
}

?>
--EXPECT--

namespace Example {
    class Test {
        public function __construct() {
        }

    }

}

namespace {
    use Example\Test;
    function test() {
        $class = Test::class;
        $args = [1, 2];
        new \Test(1, 2);
        new (Test::class)(1, 2);
        new $class(1, 2);
        new ('Example\\' . $class)(1, 2);
        new \Test();
        new \Test(1);
        new \Test(1, 2, 3);
        new \Test(1, 2, 3, 4);
        new \Test(...[$class => 'bar']);
        new \Test(...$args);
    }

    echo ArcBooster\render_ast(file_get_contents(__FILE__));
}
