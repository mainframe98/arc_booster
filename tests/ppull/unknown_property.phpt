--TEST--
arc_booster: ppull unknown property
--EXTENSIONS--
arc_booster
--FILE--
<?php

var_dump(ArcBooster\ppull(array(new stdClass), 'missing'));

?>
--EXPECTF--
Warning: Undefined property: stdClass::$missing in %s on line %d
array(1) {
  [0]=>
  NULL
}
