--TEST--
arc_booster: ppull iterable iteration without key or value method
--EXTENSIONS--
arc_booster
--FILE--
<?php

class Getter {
  public function __construct(public $key, public $value) {}
}

function generate() {
  foreach (range(1, 5) as $i) {
    yield $i => new Getter("key $i", "value $i");
  }
}
$res = ArcBooster\mpull(generate(), null, null);

var_dump(iterator_to_array(generate()) == $res);

?>
--EXPECT--
bool(true)
