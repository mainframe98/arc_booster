--TEST--
arc_booster: ipull missing key
--EXTENSIONS--
arc_booster
--FILE--
<?php

$array = array(
  array('foo' => 1),
  array('foo' => 2),
  array('foo' => 3),
  array('foo' => 4),
  array('foo' => 5),
);

var_dump(ipull($array, 'bar'));

?>
--EXPECTF--
Warning: Undefined array key "bar" in %s on line %d

Warning: Undefined array key "bar" in %s on line %d

Warning: Undefined array key "bar" in %s on line %d

Warning: Undefined array key "bar" in %s on line %d

Warning: Undefined array key "bar" in %s on line %d
array(5) {
  [0]=>
  NULL
  [1]=>
  NULL
  [2]=>
  NULL
  [3]=>
  NULL
  [4]=>
  NULL
}
