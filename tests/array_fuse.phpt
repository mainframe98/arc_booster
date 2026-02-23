--TEST--
arc_booster: array_fuse
--EXTENSIONS--
arc_booster
--FILE--
<?php

var_dump(ArcBooster\array_fuse(array()));
var_dump(ArcBooster\array_fuse(array(1, 2, 3, 4)));
var_dump(ArcBooster\array_fuse(array('foo', 'bar', 'baz')));
var_dump(ArcBooster\array_fuse(array(null, 0, 5, false, true)));

?>
--EXPECT--
array(0) {
}
array(4) {
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
  [4]=>
  int(4)
}
array(3) {
  ["foo"]=>
  string(3) "foo"
  ["bar"]=>
  string(3) "bar"
  ["baz"]=>
  string(3) "baz"
}
array(4) {
  [""]=>
  bool(false)
  [0]=>
  int(0)
  [5]=>
  int(5)
  [1]=>
  bool(true)
}
