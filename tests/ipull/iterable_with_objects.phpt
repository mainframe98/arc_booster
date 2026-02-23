--TEST--
arc_booster: ipull iterable with objects
--EXTENSIONS--
arc_booster
--FILE--
<?php

function generate(bool $with_key) {
  foreach (range(1, 5) as $i) {
    if ($with_key) {
      yield $i => array('foo' => new stdClass);
    } else {
      yield array('foo' => new stdClass);
    }
  }
}

echo "Without key yielded:\n";

$res = ArcBooster\ipull(generate(false), 'foo');
var_dump($res);

echo "\nWith key yielded:\n";

$res = ArcBooster\ipull(generate(true), 'foo');
var_dump($res);

?>
--EXPECT--
Without key yielded:
array(5) {
  [0]=>
  object(stdClass)#3 (0) {
  }
  [1]=>
  object(stdClass)#4 (0) {
  }
  [2]=>
  object(stdClass)#5 (0) {
  }
  [3]=>
  object(stdClass)#6 (0) {
  }
  [4]=>
  object(stdClass)#7 (0) {
  }
}

With key yielded:
array(5) {
  [1]=>
  object(stdClass)#8 (0) {
  }
  [2]=>
  object(stdClass)#9 (0) {
  }
  [3]=>
  object(stdClass)#10 (0) {
  }
  [4]=>
  object(stdClass)#11 (0) {
  }
  [5]=>
  object(stdClass)#12 (0) {
  }
}
