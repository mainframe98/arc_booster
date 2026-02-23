--TEST--
arc_booster: ppull array iteration without key or value method
--EXTENSIONS--
arc_booster
--FILE--
<?php

class Getter {
  public function __construct(public $key, public $value) {}
}

$array = array();

foreach (range(1, 5) as $i) {
  $array[$i] = new Getter($i, $i);
}

$res = ArcBooster\mpull($array, null, null);

var_dump($array == $res);
var_dump($array === $res);

?>
--EXPECT--
bool(true)
bool(true)

