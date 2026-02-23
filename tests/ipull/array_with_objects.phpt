--TEST--
arc_booster: ipull using objects as values
--EXTENSIONS--
arc_booster
--FILE--
<?php

$array = array(
  array('foo' => new stdClass),
  array('foo' => new stdClass),
  array('foo' => new stdClass),
  array('foo' => new stdClass),
);

$res = ArcBooster\ipull($array, 'foo');

var_dump($res);

?>
--EXPECT--
array(4) {
  [0]=>
  object(stdClass)#1 (0) {
  }
  [1]=>
  object(stdClass)#2 (0) {
  }
  [2]=>
  object(stdClass)#3 (0) {
  }
  [3]=>
  object(stdClass)#4 (0) {
  }
}
