--TEST--
arc_booster: mpull array iteration without key or value method
--EXTENSIONS--
arc_booster
--FILE--
<?php

class Getter {
  public function key() { throw new Exception(__FUNCTION__); }
  public function value() { throw new Exception(__FUNCTION__); }
}

$array = array();

foreach (range(1, 5) as $i) {
  $array[$i] = new Getter();
}

$res = ArcBooster\mpull($array, null, null);

var_dump($array == $res);
var_dump($array === $res);

?>
--EXPECT--
bool(true)
bool(true)
