--TEST--
arc_booster: mpull iterable iteration without key or value method
--EXTENSIONS--
arc_booster
--FILE--
<?php

class Getter {
  public function key() { throw new Exception(__FUNCTION__); }
  public function value() { throw new Exception(__FUNCTION__); }
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
