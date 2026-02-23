--TEST--
arc_booster: array_mergev
--EXTENSIONS--
arc_booster
--FILE--
<?php

var_dump(ArcBooster\array_mergev(array()));

var_dump(ArcBooster\array_mergev(array(array(1, 2, 3, 4))));

$array = array(
  'key 1' => array(1, 2, 3, 4),
  'key 2' => array(1, 2, 3, 4),
);
var_dump(ArcBooster\array_mergev($array));

$array = array(
  array( 1,  2,  3,  4),
  array( 5,  6,  7,  8),
  array( 9, 10, 11, 12),
  array(13, 14, 15, 16),
);
var_dump(ArcBooster\array_mergev($array));

$array = array(
  array(1, 2, 3, 4),
  array(1, 2, 3, 4),
  array(1, 2, 3, 4),
  array(1, 2, 3, 4),
);
var_dump(ArcBooster\array_mergev($array));

$array = array(
  array('foo' => 1, 'bar' => 2, 1),
  array('foo' => 1, 'bar' => 2, 2),
  array('foo' => 1, 'bar' => 2, 3),
  array('foo' => 1, 'bar' => 2, 4),
);
var_dump(ArcBooster\array_mergev($array));

?>
--EXPECT--
array(0) {
}
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
}
array(8) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
  [4]=>
  int(1)
  [5]=>
  int(2)
  [6]=>
  int(3)
  [7]=>
  int(4)
}
array(16) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
  [4]=>
  int(5)
  [5]=>
  int(6)
  [6]=>
  int(7)
  [7]=>
  int(8)
  [8]=>
  int(9)
  [9]=>
  int(10)
  [10]=>
  int(11)
  [11]=>
  int(12)
  [12]=>
  int(13)
  [13]=>
  int(14)
  [14]=>
  int(15)
  [15]=>
  int(16)
}
array(16) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
  [4]=>
  int(1)
  [5]=>
  int(2)
  [6]=>
  int(3)
  [7]=>
  int(4)
  [8]=>
  int(1)
  [9]=>
  int(2)
  [10]=>
  int(3)
  [11]=>
  int(4)
  [12]=>
  int(1)
  [13]=>
  int(2)
  [14]=>
  int(3)
  [15]=>
  int(4)
}
array(6) {
  ["foo"]=>
  int(1)
  ["bar"]=>
  int(2)
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
}
