--TEST--
arc_booster: utf8v_codepoints
--EXTENSIONS--
arc_booster
--FILE--
<?php

var_dump(ArcBooster\utf8v_codepoints(''));
var_dump(ArcBooster\utf8v_codepoints('x'));
var_dump(ArcBooster\utf8v_codepoints('quack'));
var_dump(ArcBooster\utf8v_codepoints("x\xe6\x9d\xb1y"));
var_dump(ArcBooster\utf8v_codepoints("\xC2\xBB"));
var_dump(ArcBooster\utf8v_codepoints("\xE2\x98\x83"));
var_dump(ArcBooster\utf8v_codepoints("\xEF\xBF\xBF"));
var_dump(ArcBooster\utf8v_codepoints("\xF0\x9F\x92\xA9"));
var_dump(ArcBooster\utf8v_codepoints("x\xCD\xA0y"));

?>
--EXPECT--
array(0) {
}
array(1) {
  [0]=>
  int(120)
}
array(5) {
  [0]=>
  int(113)
  [1]=>
  int(117)
  [2]=>
  int(97)
  [3]=>
  int(99)
  [4]=>
  int(107)
}
array(3) {
  [0]=>
  int(120)
  [1]=>
  int(26481)
  [2]=>
  int(121)
}
array(1) {
  [0]=>
  int(187)
}
array(1) {
  [0]=>
  int(9731)
}
array(1) {
  [0]=>
  int(65535)
}
array(1) {
  [0]=>
  int(128169)
}
array(3) {
  [0]=>
  int(120)
  [1]=>
  int(864)
  [2]=>
  int(121)
}
