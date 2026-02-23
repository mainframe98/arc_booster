--TEST--
arc_booster: ipull array iteration without key or value index
--EXTENSIONS--
arc_booster
--FILE--
<?php

$array = array();

foreach (range(1, 5) as $i) {
  $array[$i] = array('key' => "key $i", 'value' => "value $i");
}

$res = ArcBooster\ipull($array, null, null);

var_dump($array == $res);
var_dump($array === $res);

?>
--EXPECT--
bool(true)
bool(true)
