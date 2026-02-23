--TEST--
arc_booster: ppull iterable with objects
--EXTENSIONS--
arc_booster
--FILE--
<?php

class Getter {
  public function __construct(public $key, public $value) {}
}

function generate(bool $with_key) {
  foreach (range(1, 5) as $i) {
    if ($with_key) {
      yield $i => new Getter(new stdClass, new stdClass);
    } else {
      yield new Getter(new stdClass, new stdClass);
    }
  }
}

echo "Without key yielded:\n";

$res = ArcBooster\ppull(generate(false), 'value');
var_dump($res);

echo "\nWith key yielded:\n";

$res = ArcBooster\ppull(generate(true), 'value');
var_dump($res);

var_dump($res);

?>
--EXPECT--
Without key yielded:
array(5) {
  [0]=>
  object(stdClass)#5 (0) {
  }
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
}

With key yielded:
array(5) {
  [1]=>
  object(stdClass)#6 (0) {
  }
  [2]=>
  object(stdClass)#13 (0) {
  }
  [3]=>
  object(stdClass)#14 (0) {
  }
  [4]=>
  object(stdClass)#15 (0) {
  }
  [5]=>
  object(stdClass)#16 (0) {
  }
}
array(5) {
  [1]=>
  object(stdClass)#6 (0) {
  }
  [2]=>
  object(stdClass)#13 (0) {
  }
  [3]=>
  object(stdClass)#14 (0) {
  }
  [4]=>
  object(stdClass)#15 (0) {
  }
  [5]=>
  object(stdClass)#16 (0) {
  }
}
